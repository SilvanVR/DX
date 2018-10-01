#include "VkRenderer.h"
/**********************************************************************
    class: VkRenderer

    author: S. Hau
    date: August 9, 2018
**********************************************************************/

#include "Vulkan.hpp"
#include "command_buffer.h"
#include "OS/FileSystem/file.h"
#include "Lighting/lights.h"
#include "camera.h"
#include "Resources/VkShader.h"
#include "Resources/VkMaterial.h"
#include "Resources/VkMesh.h"
#include "Resources/VkTexture2D.h"
#include "Resources/VkCubemap.h"
#include "Resources/VkRenderBuffer.h"
#include "Resources/VkRenderTexture.h"
#include "Resources/VkTexture2DArray.h"
#include "VR/OculusRift/oculus_rift_vk.h"
#include "Common/string_utils.h"

#define SWAPCHAIN_FORMAT    VK_FORMAT_B8G8R8A8_UNORM
#define ENGINE_VS_PATH      "/engine/shaders/includes/vulkan/engineVS.glsl"
#define ENGINE_FS_PATH      "/engine/shaders/includes/vulkan/engineFS.glsl"

namespace Graphics {

    static String CAMERA_UBO_KEYWORD     ( "camera" );
    static String GLOBAL_UBO_KEYWORD     ( "global" );
    static String LIGHTS_UBO_KEYWORD     ( "lights" );

    static StringID POST_PROCESS_INPUT_NAME   = SID( "_MainTex" );
    static StringID CAM_POS_NAME              = SID( "pos" );
    static StringID CAM_ZNEAR_NAME            = SID( "zNear" );
    static StringID CAM_ZFAR_NAME             = SID( "zFar" );
    static StringID CAM_VIEW_MATRIX_NAME      = SID( "view" );
    static StringID CAM_PROJ_MATRIX_NAME      = SID( "proj" );

    static StringID LIGHT_COUNT_NAME          = SID( "count" );
    static StringID LIGHT_BUFFER_NAME         = SID( "lights" );
    static StringID LIGHT_VIEW_PROJ_NAME      = SID( "viewProj" );
    static StringID LIGHT_CSM_SPLITS_NAME     = SID( "CSMSplits" );

    #define SHADOW_MAPS_SET                 0
    #define SHADOW_MAP_2D_BINDING_BEGIN     3
    #define SHADOW_MAP_3D_BINDING_BEGIN     7
    #define SHADOW_MAP_ARRAY_BINDING_BEGIN  8

    static ArrayList<ShaderResourceDeclaration> SHADOW_MAP_2D_RESOURCE_DECLS{
        { ShaderType::Fragment, SHADOW_MAPS_SET, SHADOW_MAP_2D_BINDING_BEGIN + 0, SID("ShadowMap2D"), DataType::Texture2D },
        { ShaderType::Fragment, SHADOW_MAPS_SET, SHADOW_MAP_2D_BINDING_BEGIN + 1, SID("ShadowMap2D"), DataType::Texture2D },
        { ShaderType::Fragment, SHADOW_MAPS_SET, SHADOW_MAP_2D_BINDING_BEGIN + 2, SID("ShadowMap2D"), DataType::Texture2D },
        { ShaderType::Fragment, SHADOW_MAPS_SET, SHADOW_MAP_2D_BINDING_BEGIN + 3, SID("ShadowMap2D"), DataType::Texture2D },
    };
    static ArrayList<ShaderResourceDeclaration> SHADOW_MAP_3D_RESOURCE_DECLS{
        { ShaderType::Fragment, SHADOW_MAPS_SET,SHADOW_MAP_3D_BINDING_BEGIN + 0, SID("ShadowMapCube"), DataType::TextureCubemap }
    };
    static ArrayList<ShaderResourceDeclaration> SHADOW_MAP_ARRAY_RESOURCE_DECLS{
        { ShaderType::Fragment, SHADOW_MAPS_SET,SHADOW_MAP_ARRAY_BINDING_BEGIN + 0, SID("ShadowMapArray"), DataType::Texture2D },
    };

    using namespace Vulkan;

    //----------------------------------------------------------------------
    VkPhysicalDeviceFeatures GetDeviceFeatures()
    {
        VkPhysicalDeviceFeatures requestedFeatures{};
        requestedFeatures.fillModeNonSolid              = VK_TRUE;
        requestedFeatures.depthBiasClamp                = VK_TRUE;
        requestedFeatures.depthBounds                   = VK_TRUE;
        requestedFeatures.depthClamp                    = VK_TRUE;
        requestedFeatures.geometryShader                = VK_TRUE;
        requestedFeatures.samplerAnisotropy             = VK_TRUE;
        requestedFeatures.tessellationShader            = VK_TRUE;
        requestedFeatures.shaderStorageImageMultisample = VK_TRUE;
        return requestedFeatures;
    }

    //**********************************************************************
    // INIT STUFF
    //**********************************************************************

    //----------------------------------------------------------------------
    void VkRenderer::init()
    {
        _SetLimits();

        VR::Device hmd = VR::GetFirstSupportedHMDAndInitialize();
        switch (hmd)
        {
        case VR::Device::OculusRift:
        {
            auto vkOculus = new VR::OculusRiftVk();
            g_vulkan.CreateInstance( vkOculus->getRequiredInstanceExtentions() );
            m_swapchain.createSurface( g_vulkan.instance, m_window );
            g_vulkan.SelectPhysicalDevice( vkOculus->getPhysicalDevice( g_vulkan.instance ) );
            g_vulkan.CreateDevice( vkOculus->getRequiredDeviceExtentions(), GetDeviceFeatures() );
            vkOculus->setSynchronizationQueueVk( g_vulkan.graphicsQueue );
            vkOculus->createEyeBuffers( g_vulkan.device );
            m_swapchain.createSwapchain( g_vulkan.device, m_window->getWidth(), m_window->getHeight(), SWAPCHAIN_FORMAT );
            m_hmd = vkOculus;
            break;
        }
        default:
            LOG_WARN_RENDERING( "VR not supported on your system." );
        }

        if ( not hasHMD() )
        {
            g_vulkan.CreateInstance({ VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME });
            m_swapchain.createSurface( g_vulkan.instance, m_window );
            g_vulkan.SelectPhysicalDevice();
            g_vulkan.CreateDevice({ VK_KHR_SWAPCHAIN_EXTENSION_NAME }, GetDeviceFeatures() );
            m_swapchain.createSwapchain( g_vulkan.device, m_window->getWidth(), m_window->getHeight(), SWAPCHAIN_FORMAT );
        }

        g_vulkan.ctx.Init();

        _SetGPUDescription();
        _CreateRequiredUniformBuffersFromFile( ENGINE_VS_PATH, ENGINE_FS_PATH );
        _CreateCubeMesh();
        _CreateFakeShadowMaps();

        LOG_RENDERING( "Done initializing Vulkan... (Using " + getGPUDescription().name + ")" );
    }

    //----------------------------------------------------------------------
    void VkRenderer::shutdown()
    {
        vkDeviceWaitIdle( g_vulkan.device );
        for (auto i = 0; i < MAX_SHADOWMAPS_2D; ++i) SAFE_DELETE( m_fakeShadowMaps2D[i] );
        for (auto i = 0; i < MAX_SHADOWMAPS_3D; ++i) SAFE_DELETE( m_fakeShadowMaps3D[i] );
        for (auto i = 0; i < MAX_SHADOWMAPS_ARRAY; ++i) SAFE_DELETE( m_fakeShadowMaps2DArray[i] );
        _DestroyAllTempRenderTargets();
        SAFE_DELETE( m_globalBuffer );
        SAFE_DELETE( m_cameraBuffer );
        SAFE_DELETE( m_lightBuffer );
        g_vulkan.ctx.Shutdown();
        m_swapchain.shutdown( g_vulkan.instance, g_vulkan.device );
        SAFE_DELETE( m_hmd );
        SAFE_DELETE( m_cubeMesh );
        renderContext.Reset();
        g_vulkan.Shutdown();
    }

    //----------------------------------------------------------------------
    void VkRenderer::_ExecuteCommandBuffer( const CommandBuffer& cmd )
    {
        auto& commands = cmd.getGPUCommands();

        for ( auto& command : commands )
        {
            switch ( command->getType() )
            {
                case GPUCommand::SET_CAMERA:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetCamera*>( command.get() );
                    _SetCamera( &cmd.camera );
                    break;
                }
                case GPUCommand::END_CAMERA:
                {
                    auto& cmd = *reinterpret_cast<GPUC_EndCamera*>( command.get() );
                    g_vulkan.ctx.EndRenderPass();
                    renderContext.Reset();
                    break;
                }
                case GPUCommand::DRAW_MESH:
                {
                    auto& cmd = *reinterpret_cast<GPUC_DrawMesh*>( command.get() );
                    _DrawMesh( cmd.mesh.get(), cmd.material, cmd.modelMatrix, cmd.subMeshIndex );
                    break;
                }
                case GPUCommand::DRAW_MESH_INSTANCED:
                {
                    auto& cmd = *reinterpret_cast<GPUC_DrawMeshInstanced*>( command.get() );
                    _DrawMeshInstanced( cmd.mesh.get(), cmd.material, cmd.modelMatrix, cmd.instanceCount );
                    break;
                }
                case GPUCommand::COPY_TEXTURE:
                {
                    auto& cmd = *dynamic_cast<GPUC_CopyTexture*>( command.get() );
                    _CopyTexture( cmd.srcTex.get(), cmd.srcElement, cmd.srcMip, cmd.dstTex.get(), cmd.dstElement, cmd.dstMip );
                    break;
                }
                case GPUCommand::DRAW_LIGHT:
                {
                    auto& cmd = *reinterpret_cast<GPUC_DrawLight*>( command.get() );
                    if ( renderContext.lightCount < MAX_LIGHTS )
                    {
                        // Add light to list and update light count
                        renderContext.lights[renderContext.lightCount++] = cmd.light;
                        renderContext.lightsUpdated = true;
                    }
                    else
                    {
                        LOG_WARN_RENDERING( "Too many lights! Only " + TS( MAX_LIGHTS ) + " lights will be rendered." );
                    }
                    break;
                }
                case GPUCommand::SET_RENDER_TARGET:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetRenderTarget*>( command.get() );
                    renderContext.BindRendertarget( cmd.target, m_frameCount );
                    break;
                }
                case GPUCommand::DRAW_FULLSCREEN_QUAD:
                {
                    auto& cmd = *reinterpret_cast<GPUC_DrawFullscreenQuad*>( command.get() );
                    auto currRT = renderContext.getRenderTarget();
                    ASSERT( currRT && "No rendertarget was previously set." );
                    ViewportRect vp = { 0, 0, (F32)currRT->getWidth(), (F32)currRT->getHeight() };
                    _DrawFullScreenQuad( cmd.material, vp );
                    break;
                }
                case GPUCommand::RENDER_CUBEMAP:
                {
                    auto& cmd = *reinterpret_cast<GPUC_RenderCubemap*>( command.get() );
                    _RenderCubemap( cmd.cubemap.get(), cmd.material, cmd.dstMip );
                    break;
                }
                case GPUCommand::BLIT:
                {
                    auto& cmd = *reinterpret_cast<GPUC_Blit*>( command.get() );
                    _Blit( cmd.src, cmd.dst, cmd.material );
                    break;
                }
                case GPUCommand::SET_SCISSOR:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetScissor*>( command.get() );
                    VkRect2D scissor{};
                    scissor.offset = { cmd.rect.left, cmd.rect.top };
                    scissor.extent = { (U32)(cmd.rect.right - cmd.rect.left), (U32)(cmd.rect.bottom - cmd.rect.top) };
                    g_vulkan.ctx.RSSetScissor( scissor );
                    break;
                }
                case GPUCommand::SET_CAMERA_MATRIX:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetCameraMatrix*>( command.get() );
                    StringID name;
                    switch (cmd.member)
                    {
                    case CameraMember::View:       name = CAM_VIEW_MATRIX_NAME; break;
                    case CameraMember::Projection: name = CAM_PROJ_MATRIX_NAME; break;
                    default: LOG_WARN_RENDERING( "VkRenderer: Command [SET_CAMERA_MATRIX]: Unsupported Camera Matrix." );
                    }
                    m_cameraBuffer->beginBuffer();
                    m_cameraBuffer->update( name, &cmd.matrix );
                    m_cameraBuffer->bind();
                    break;
                }
                default:
                    LOG_WARN_RENDERING( "Unknown GPU Command in a given command buffer!" );
            }
        }
    }

    //----------------------------------------------------------------------
    void VkRenderer::OnWindowSizeChanged( U16 w, U16 h )
    {
        if ( w == 0 || h == 0 ) // Window was minimized
            return;

        // Recreate swapchain buffers
        m_swapchain.recreate( w, h );
    }

    //----------------------------------------------------------------------
    void VkRenderer::_VSyncChanged( bool enabled )
    {
        m_swapchain.setVSync( enabled );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void VkRenderer::present()
    {
        if (m_window->getWidth() == 0 || m_window->getHeight() == 0)
            return;

        _CheckAndDestroyTemporaryRenderTargets();

        // Record all commands
        g_vulkan.ctx.BeginFrame();
        {
            m_globalBuffer->bind();
            m_cameraBuffer->newFrame();
            m_lightBuffer->newFrame();
            {
                _LockQueue();
                for (auto& cmd : m_pendingCmdQueue)
                   _ExecuteCommandBuffer( cmd );
                g_vulkan.ctx.EndFrame();

                // Become resources might be uniquely stored in the command buffer it must be cleared after EndFrame()
                m_pendingCmdQueue.clear();
                _UnlockQueue();
            }
        }

        // Present rendered image to screen/hmd
        if ( hasHMD() )
            m_hmd->distortAndPresent( m_frameCount );

        m_swapchain.present( g_vulkan.graphicsQueue, g_vulkan.ctx.curFrameData().semRenderingFinished );

        m_frameCount++;
    }

    //----------------------------------------------------------------------
    IMesh*              VkRenderer::createMesh()           { return new Vulkan::Mesh; }
    IMaterial*          VkRenderer::createMaterial()       { return new Vulkan::Material; }
    IShader*            VkRenderer::createShader()         { return new Vulkan::Shader; }
    ITexture2D*         VkRenderer::createTexture2D()      { return new Vulkan::Texture2D; }
    IRenderTexture*     VkRenderer::createRenderTexture()  { return new Vulkan::RenderTexture; }
    ICubemap*           VkRenderer::createCubemap()        { return new Vulkan::Cubemap; }
    IRenderBuffer*      VkRenderer::createRenderBuffer()   { return new Vulkan::RenderBuffer; }
    ITexture2DArray*    VkRenderer::createTexture2DArray() { return new Vulkan::Texture2DArray; }

    //----------------------------------------------------------------------
    bool VkRenderer::setGlobalFloat( StringID name, F32 value )
    {
        if (not _UpdateGlobalBuffer( name, &value ))
        {
            LOG_WARN_RENDERING( "Global-Float '" + name.toString() + "' does not exist. Did you spell it correctly?" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool VkRenderer::setGlobalInt( StringID name, I32 value )
    {
        if (not _UpdateGlobalBuffer( name, &value ))
        {
            LOG_WARN_RENDERING( "Global-Int '" + name.toString() + "' does not exist. Did you spell it correctly?" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool VkRenderer::setGlobalVector4( StringID name, const Math::Vec4& vec4 )
    {
        if (not _UpdateGlobalBuffer( name, &vec4 ))
        {
            LOG_WARN_RENDERING( "Global-Vec4 '" + name.toString() + "' does not exist. Did you spell it correctly?" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool VkRenderer::setGlobalColor( StringID name, Color color )
    {
        if (not _UpdateGlobalBuffer( name, color.normalized().data() ))
        {
            LOG_WARN_RENDERING( "Global-color '" + name.toString() + "' does not exist. Did you spell it correctly?" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool VkRenderer::setGlobalMatrix( StringID name, const DirectX::XMMATRIX& matrix )
    {
        if (not _UpdateGlobalBuffer( name, &matrix ))
        {
            LOG_WARN_RENDERING( "Global-Matrix '" + name.toString() + "' does not exist. Did you spell it correctly?" );
            return false;
        }
        return true;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void VkRenderer::_SetLimits()
    {
        m_limits.maxLights      = MAX_LIGHTS;
        m_limits.maxShadowmaps  = MAX_SHADOWMAPS_2D + MAX_SHADOWMAPS_3D + MAX_SHADOWMAPS_ARRAY;
        m_limits.maxCascades    = MAX_CSM_SPLITS;
    }

    //----------------------------------------------------------------------
    void VkRenderer::_SetGPUDescription()
    {
        m_gpuDescription.name = g_vulkan.gpu.properties.deviceName;
        m_gpuDescription.maxDedicatedMemoryMB = g_vulkan.gpu.memoryProperties.memoryHeaps[0].size;
    }

    //**********************************************************************
    // PRIVATE - COMMANDS
    //**********************************************************************

    //----------------------------------------------------------------------
    void VkRenderer::_SetCamera( Camera* camera )
    {
        auto renderTarget = camera->getRenderTarget();
        if (renderTarget == nullptr)
        {
            LOG_WARN_RENDERING( "Rendertarget of a camera is NULL. This is not allowed. Please ensure that a camera always has a valid RT." );
            return;
        }

        renderContext.SetCamera( camera );

        // Clear rendertarget
        switch ( camera->getClearMode() )
        {
        case CameraClearMode::None: break;
        case CameraClearMode::Color:
            renderTarget->clear( camera->getClearColor(), 1.0f, 0 );
            break;
        case CameraClearMode::Depth:
            renderTarget->clearDepthStencil( 1.0f, 0 );
            break;
        default: LOG_WARN_RENDERING( "Unknown Clear-Mode in camera!" );
        }

        renderContext.BindRendertarget( renderTarget, m_frameCount );

        if ( camera->isBlittingToScreen() )
        {
            VkViewport vp = { 0, 0, (F32)renderTarget->getWidth(), (F32)renderTarget->getHeight(), 0, 1 };
            g_vulkan.ctx.RSSetViewports( vp );
            g_vulkan.ctx.RSSetScissor({ { (I32)vp.x, (I32)vp.y }, { (U32)vp.width, (U32)vp.height } });
        }
        else
        {
            // Set viewport (Translate to pixel coordinates first)
            VkViewport vp = {};
            auto viewport = camera->getViewport();
            vp.x        = viewport.topLeftX * renderTarget->getWidth();
            vp.y        = viewport.topLeftY * renderTarget->getHeight();
            vp.width    = viewport.width    * renderTarget->getWidth();
            vp.height   = viewport.height   * renderTarget->getHeight();
            vp.maxDepth = 1.0f;
            g_vulkan.ctx.RSSetViewports( vp );
            g_vulkan.ctx.RSSetScissor({ { (I32)vp.x, (I32)vp.y }, { (U32)vp.width, (U32)vp.height } });
        }

        m_cameraBuffer->beginBuffer();
        auto modelMatrix = camera->getModelMatrix();
        auto translation = modelMatrix.r[3];
        if ( not m_cameraBuffer->update( CAM_POS_NAME, &translation ) )
            LOG_ERROR_RENDERING( "Vulkan: Could not update the camera buffer [position]. Fix this!" );

        auto zFar = camera->getZFar();
        if ( not m_cameraBuffer->update( CAM_ZFAR_NAME, &zFar ) )
            LOG_ERROR_RENDERING( "Vulkan: Could not update the camera buffer [zFar]. Fix this!" );

        auto zNear = camera->getZNear();
        if ( not m_cameraBuffer->update( CAM_ZNEAR_NAME, &zNear ) )
            LOG_ERROR_RENDERING( "Vulkan: Could not update the camera buffer [zNear]. Fix this!" );

        if ( not m_cameraBuffer->update( CAM_VIEW_MATRIX_NAME, &camera->getViewMatrix() ) )
            LOG_ERROR_RENDERING( "Vulkan: Could not update the camera buffer [View]. Fix this!" );

        if ( not m_cameraBuffer->update( CAM_PROJ_MATRIX_NAME, &camera->getProjectionMatrix() ) )
            LOG_ERROR_RENDERING( "Vulkan: Could not update the camera buffer [Projection]. Fix this!" );
        m_cameraBuffer->bind();
    }

    //----------------------------------------------------------------------
    void VkRenderer::_BindMesh( IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
    {
        // Update global buffer if necessary
        if ( m_globalBuffer )
            m_globalBuffer->flush();

        // Update light buffer if necessary
        if ( m_lightBuffer )
            _FlushLightBuffer();

        // Bind shader, possibly a replacement shader
        auto shader = material->getShader();
        if (auto curCamera = renderContext.getCamera())
        {
            if ( auto& camShader = curCamera->getReplacementShader() )
            {
                if ( auto& matShader = material->getReplacementShader( curCamera->getReplacementShaderTag() ) )
                    shader = matShader;
                else
                    shader = camShader;
            }
        }

        // Bind shader and material
        renderContext.BindShader( shader );
        renderContext.BindMaterial( material );

        // Update per object buffer
        g_vulkan.ctx.PushConstants( 0, sizeof( DirectX::XMMATRIX ), &modelMatrix );

        // Bind mesh
        mesh->bind( shader->getVertexLayout(), subMeshIndex );
    }

    //----------------------------------------------------------------------
    void VkRenderer::_DrawMesh( IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
    {
        // Measuring per frame data
        if (auto curCamera = renderContext.getCamera())
        {
            auto& camInfo = curCamera->getFrameInfo();
            auto numIndices = mesh->getIndexCount( subMeshIndex );
            camInfo.drawCalls++;
            camInfo.numVertices += numIndices;
            camInfo.numTriangles += numIndices / 3;
        }

        _BindMesh( mesh, material, modelMatrix, subMeshIndex );
        g_vulkan.ctx.DrawIndexed( mesh->getIndexCount( subMeshIndex ), 1, 0, mesh->getBaseVertex( subMeshIndex ), 0 );
    }

    //----------------------------------------------------------------------
    void VkRenderer::_DrawMeshInstanced( IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 instanceCount )
    {
        // Measuring per frame data
        if (auto curCamera = renderContext.getCamera())
        {
            auto& camInfo = curCamera->getFrameInfo();
            auto numIndices = mesh->getIndexCount( 0 ) * instanceCount;
            camInfo.drawCalls++;
            camInfo.numVertices += numIndices;
            camInfo.numTriangles += numIndices / 3;
        }

        _BindMesh( mesh, material, modelMatrix, 0 );
        g_vulkan.ctx.DrawIndexed( mesh->getIndexCount( 0 ), instanceCount, 0, mesh->getBaseVertex( 0 ), 0 );
    }

    //----------------------------------------------------------------------
    void VkRenderer::_FlushLightBuffer()
    {
        if ( not renderContext.lightsUpdated || not renderContext.getCamera() )
            return;
        renderContext.lightsUpdated = false;

        // Bind fake shadow-maps, otherwise validation will complain
        for (auto i = 0; i < SHADOW_MAP_2D_RESOURCE_DECLS.size(); ++i)
            m_fakeShadowMaps2D[i]->bind( SHADOW_MAP_2D_RESOURCE_DECLS[i] );
        for (auto i = 0; i < SHADOW_MAP_3D_RESOURCE_DECLS.size(); ++i)
            m_fakeShadowMaps3D[i]->bind( SHADOW_MAP_3D_RESOURCE_DECLS[i] );
        for (auto i = 0; i < SHADOW_MAP_ARRAY_RESOURCE_DECLS.size(); ++i)
            m_fakeShadowMaps2DArray[i]->bind( SHADOW_MAP_ARRAY_RESOURCE_DECLS[i] );

        renderContext.getCamera()->getFrameInfo().numLights = renderContext.lightCount;

        m_lightBuffer->beginBuffer();

        // Update light count
        if ( not m_lightBuffer->update( LIGHT_COUNT_NAME, &renderContext.lightCount ) )
            LOG_ERROR_RENDERING( "Failed to update light-count. Something is horribly broken! Fix this!" );

        struct Light
        {
            Math::Vec3  position;               // 12 bytes
            I32         lightType;              // 4 bytes
            //----------------------------------- (16 byte boundary)
            Math::Vec3  direction;              // 12 bytes
            F32         intensity;              // 4 bytes
            //----------------------------------- (16 byte boundary)
            Math::Vec4  color;                  // 16 bytes
            //----------------------------------- (16 byte boundary)
            F32         spotAngle;              // 4 bytes
            F32         range;                  // 4 bytes
            I32         shadowMapIndex;         // 4 bytes
            I32         shadowType;             // 4 bytes
            //----------------------------------- (16 byte boundary)
        } lights[MAX_LIGHTS];

        I32 curShadowMap2DIndex = 0;
        I32 curShadowMap3DIndex = 0;
        I32 curShadowMapArrayIndex = 0;
        DirectX::XMMATRIX lightViewProjs[MAX_SHADOWMAPS_2D];

        // Update light array
        for (I32 i = 0; i < renderContext.lightCount; ++i)
        {
            lights[i].color     = renderContext.lights[i]->getColor().normalized();
            lights[i].intensity = renderContext.lights[i]->getIntensity();
            lights[i].lightType = (I32)renderContext.lights[i]->getLightType();
            lights[i].shadowMapIndex = -1;
            lights[i].shadowType = (I32)renderContext.lights[i]->getShadowType();

            switch ( renderContext.lights[i]->getLightType() )
            {
            case LightType::Directional:
            {
                auto dirLight = reinterpret_cast<const DirectionalLight*>( renderContext.lights[i] );
                lights[i].direction = dirLight->getDirection();

                if ( dirLight->shadowsEnabled() )
                {
                    lights[i].range = dirLight->getShadowRange();

                    switch ( dirLight->getShadowType() )
                    {
                    case ShadowType::Hard:
                    case ShadowType::Soft:
                        if ( curShadowMap2DIndex < MAX_SHADOWMAPS_2D )
                        {
                            lights[i].shadowMapIndex = curShadowMap2DIndex;
                            lightViewProjs[curShadowMap2DIndex] = renderContext.lights[i]->getShadowViewProjection();
                            renderContext.lights[i]->getShadowMap()->bind( SHADOW_MAP_2D_RESOURCE_DECLS[curShadowMap2DIndex] );
                            curShadowMap2DIndex++;
                        }
                        break;
                    case ShadowType::CSM:
                    case ShadowType::CSMSoft:
                        if (curShadowMapArrayIndex < MAX_SHADOWMAPS_ARRAY)
                        {
                            lights[i].shadowMapIndex = curShadowMapArrayIndex;

                            auto& splits = dirLight->getCSMSplits();
                            if ( not m_lightBuffer->update( LIGHT_CSM_SPLITS_NAME, splits.data() ) )
                                LOG_ERROR_RENDERING( "Failed to update light-buffer. Something is horribly broken! Fix this!" );

                            renderContext.lights[i]->getShadowMap()->bind( SHADOW_MAP_ARRAY_RESOURCE_DECLS[curShadowMapArrayIndex] );
                            curShadowMapArrayIndex++;
                        }
                        break;
                    }
                }
                break;
            }
            case LightType::Spot:
            {
                auto spotLight = reinterpret_cast<const SpotLight*>( renderContext.lights[i] );
                lights[i].position  = spotLight->getPosition();
                lights[i].range     = spotLight->getRange();
                lights[i].spotAngle = spotLight->getAngle();
                lights[i].direction = spotLight->getDirection();

                if ( spotLight->shadowsEnabled() )
                {
                    switch ( spotLight->getShadowType() )
                    {
                    case ShadowType::Hard:
                    case ShadowType::Soft:
                        if ( curShadowMap2DIndex < MAX_SHADOWMAPS_2D )
                        {
                            lights[i].shadowMapIndex = curShadowMap2DIndex;
                            lightViewProjs[curShadowMap2DIndex] = renderContext.lights[i]->getShadowViewProjection();
                            renderContext.lights[i]->getShadowMap()->bind( SHADOW_MAP_2D_RESOURCE_DECLS[curShadowMap2DIndex] );
                            curShadowMap2DIndex++;
                        }
                        break;
                    case ShadowType::CSM:
                    case ShadowType::CSMSoft:
                        LOG_WARN_RENDERING( "ShadowType CSM in Spot-Light, which is not supported!" );
                    }
                }
                break;
            }
            case LightType::Point:
            {
                auto pointLight = reinterpret_cast<const PointLight*>( renderContext.lights[i] );
                lights[i].position  = pointLight->getPosition();
                lights[i].range = pointLight->getRange();

                if ( pointLight->shadowsEnabled() )
                {
                    switch ( pointLight->getShadowType() )
                    {
                    case ShadowType::Hard:
                    case ShadowType::Soft:
                        if ( curShadowMap3DIndex < MAX_SHADOWMAPS_3D )
                        {
                            lights[i].shadowMapIndex = curShadowMap3DIndex;
                            renderContext.lights[i]->getShadowMap()->bind( SHADOW_MAP_3D_RESOURCE_DECLS[curShadowMap3DIndex] );
                            curShadowMap3DIndex++;
                        }
                        break;
                    case ShadowType::CSM:
                    case ShadowType::CSMSoft:
                        LOG_WARN_RENDERING( "ShadowType CSM in Point-Light, which is not supported!" );
                    }
                }
                break;
            }
            default:
                ASSERT( false && "Unknown light type!" );
            }
        }

        if ( not m_lightBuffer->update( LIGHT_BUFFER_NAME, &lights ) )
            LOG_ERROR_RENDERING( "Failed to update light-buffer. Something is horribly broken! Fix this!" );

        if ( not m_lightBuffer->update( LIGHT_VIEW_PROJ_NAME, &lightViewProjs ) )
            LOG_ERROR_RENDERING( "Failed to update light-buffer [ViewProjections]. Something is horribly broken! Fix this!" );

        // Update gpu buffer
        m_lightBuffer->bind();
    }

    //----------------------------------------------------------------------
    void VkRenderer::_CreateRequiredUniformBuffersFromFile( const String& engineVS, const String& engineFS )
    {
        try {
            OS::BinaryFile vertFile(engineVS, OS::EFileMode::READ );
            String vertSrc = vertFile.readAll();
            vertSrc += "                \
            void main()                 \
            {                           \
                gl_Position = vec4(0);  \
            }";

            OS::BinaryFile fragFile( engineFS, OS::EFileMode::READ );
            String fragSrc = fragFile.readAll();
            fragSrc += 
            "layout (location = 0) out vec4 outColor;   \
            void main()                                 \
            {                                           \
                outColor = vec4(1,1,1,1);               \
            }";

            auto shader = std::unique_ptr<Graphics::Shader>( createShader() );
            try {
                shader->compileVertexShaderFromSource( vertSrc, "main" );
                shader->compileFragmentShaderFromSource( fragSrc, "main" );
                shader->createPipeline();

                auto ubos = shader->getUniformBufferDeclarations();
                for (auto& ubo : ubos)
                {
                    String lower = StringUtils::toLower( ubo.getName().toString() );
                    if ( lower.find( CAMERA_UBO_KEYWORD ) != String::npos )
                        if (not m_cameraBuffer)
                            m_cameraBuffer = new Vulkan::CachedMappedUniformBuffer( ubo, BufferUsage::Frequently );
                        else LOG_WARN_RENDERING( "VkRenderer::_CreateGlobalBuffersFromFile(): Found another camera ubo." );
                    else if(lower.find( GLOBAL_UBO_KEYWORD ) != String::npos)
                        if (not m_globalBuffer)
                            m_globalBuffer = new Vulkan::MappedUniformBuffer( ubo, BufferUsage::Frequently );
                        else LOG_WARN_RENDERING( "VkRenderer::_CreateGlobalBuffersFromFile(): Found another global ubo." );
                    else if(lower.find( LIGHTS_UBO_KEYWORD ) != String::npos)
                        if (not m_lightBuffer)
                            m_lightBuffer = new Vulkan::CachedMappedUniformBuffer( ubo, BufferUsage::Frequently );
                        else LOG_WARN_RENDERING( "VkRenderer::_CreateGlobalBuffersFromFile(): Found another light ubo." );
                }
                if (not m_cameraBuffer) LOG_ERROR_RENDERING( "VkRenderer::_CreateGlobalBuffersFromFile(): Could not find camera buffer." );
                if (not m_globalBuffer) LOG_ERROR_RENDERING( "VkRenderer::_CreateGlobalBuffersFromFile(): Could not find global buffer." );
                if (not m_lightBuffer)  LOG_ERROR_RENDERING( "VkRenderer::_CreateGlobalBuffersFromFile(): Could not find light buffer." );
            }
            catch (const std::runtime_error& e) {
                LOG_ERROR_RENDERING( "Could not precompile '" + engineFS + "' for buffer creation. This is mandatory. Reason: " + e.what() );
            }
        } 
        catch (const std::runtime_error& ex)
        {
            LOG_WARN_RENDERING( String( "Could not open ' " + engineFS + "'. This might cause some issues. Reason: " ) + ex.what() );
        }
    }

    //----------------------------------------------------------------------
    void VkRenderer::_CreateCubeMesh()
    {
        ArrayList<Math::Vec3> cubeVertices =
        {
            Math::Vec3(-1.0f, -1.0f, -1.0f),
            Math::Vec3(-1.0f,  1.0f, -1.0f),
            Math::Vec3(1.0f,  1.0f, -1.0f),
            Math::Vec3(1.0f, -1.0f, -1.0f),
            Math::Vec3(-1.0f, -1.0f,  1.0f),
            Math::Vec3(-1.0f,  1.0f,  1.0f),
            Math::Vec3(1.0f,  1.0f,  1.0f),
            Math::Vec3(1.0f, -1.0f,  1.0f)
        };
        ArrayList<U32> cubeIndices = {
            0, 1, 3, 3, 1, 2,
            4, 6, 5, 4, 7, 6,
            4, 5, 1, 4, 1, 0,
            3, 2, 6, 3, 6, 7,
            1, 5, 6, 1, 6, 2,
            4, 0, 3, 4, 3, 7
        };
        m_cubeMesh = createMesh();
        m_cubeMesh->setVertices( cubeVertices );
        m_cubeMesh->setIndices( cubeIndices );
    }

    //----------------------------------------------------------------------
    void VkRenderer::_CopyTexture( ITexture* srcTex, I32 srcElement, I32 srcMip, ITexture* dstTex, I32 dstElement, I32 dstMip )
    {
        VezImageCopy region{};
        region.extent = { srcTex->getWidth(), srcTex->getHeight(), 1 };
        region.srcSubresource.mipLevel          = srcMip;
        region.srcSubresource.baseArrayLayer    = srcElement;
        region.srcSubresource.layerCount        = 1;
        region.dstSubresource.mipLevel          = dstMip;
        region.dstSubresource.baseArrayLayer    = dstElement;
        region.dstSubresource.layerCount        = 1;

        auto srcImg = reinterpret_cast<VkImage>( srcTex->getNativeTexturePtr() );
        auto dstImg = reinterpret_cast<VkImage>( dstTex->getNativeTexturePtr() );
        g_vulkan.ctx.CopyImage( srcImg, dstImg, 1, &region );
    }

    //----------------------------------------------------------------------
    void VkRenderer::_RenderCubemap( ICubemap* cubemap, const MaterialPtr& material, U32 dstMip )
    {
         DirectX::XMVECTOR directions[] = {
            { 1, 0, 0, 0 }, { -1,  0,  0, 0 },
            { 0, 1, 0, 0 }, {  0, -1,  0, 0 },
            { 0, 0, 1, 0 }, {  0,  0, -1, 0 },
        };
        DirectX::XMVECTOR ups[] = {
            { 0, 1,  0, 0 }, { 0, 1, 0, 0 },
            { 0, 0, -1, 0 }, { 0, 0, 1, 0 },
            { 0, 1,  0, 0 }, { 0, 1, 0, 0 },
        };

        U32 width = U32( cubemap->getWidth() * std::pow( 0.5, dstMip ) );
        U32 height = U32( cubemap->getHeight() * std::pow( 0.5, dstMip ) );

        // Create temporary render texture
        auto colorBuffer = _CreateTempRenderTarget();
        colorBuffer->create( width, height, cubemap->getFormat() );
        colorBuffer->bindForRendering();
        colorBuffer->clearColor( Color::BLACK );

        // Setup viewport matching the render texture
        VkViewport vp = { 0, 0, (F32)colorBuffer->getWidth(), (F32)colorBuffer->getHeight(), 0, 1 };
        g_vulkan.ctx.RSSetViewports( vp );

        // Render into render texture for each face and copy the result into the cubemaps face
        auto projection = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians( 90.0f ), 1.0f, 0.1f, 10.0f );
        for (I32 face = 0; face < 6; face++)
        {
            m_cameraBuffer->beginBuffer();
            auto view = DirectX::XMMatrixLookToLH( { 0, 0, 0, 0 }, directions[face], ups[face] );
            if ( not m_cameraBuffer->update( CAM_VIEW_MATRIX_NAME, &view ) )
                LOG_ERROR_RENDERING( "Vulkan: Could not update the camera buffer [View]. Fix this!" );
            if ( not m_cameraBuffer->update( CAM_PROJ_MATRIX_NAME, &projection ) )
                LOG_ERROR_RENDERING( "Vulkan: Could not update the camera buffer [Projection]. Fix this!" );
            m_cameraBuffer->bind();

            _DrawMesh( m_cubeMesh, material, DirectX::XMMatrixIdentity(), 0 );
            _CopyTexture( colorBuffer, 0, 0, cubemap, face, dstMip );
        }
    }

    //----------------------------------------------------------------------
    void VkRenderer::_Blit( const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material )
    {
        auto currRT = renderContext.getRenderTarget();
        if (currRT == SCREEN_BUFFER && dst == SCREEN_BUFFER)
            LOG_WARN_RENDERING( "VkRenderer[Blit]: Target texture was previously screen, so the content will probably be overriden. This can "
                                "occur if two blits in succession with both target = nullptr (to screen) were recorded." );

        // Use the src texture as the input IF not null. Otherwise use the current bound render target.
        auto input = src.get() ? src.get() : currRT;
        if (input == SCREEN_BUFFER)
        {
            LOG_WARN_RENDERING( "VkRenderer[Blit]: Previous render target was screen, which can't be used as input! This happens when a blit-command "
                                "with src=nullptr (to screen) was recorded but a previous blit had dst=nullptr (to screen)" );
            return;
        }

        // Bind render-target (Note: if dst is null, this does nothing)
        renderContext.BindRendertarget( dst, m_frameCount );

        // Set texture in material
        material->setTexture( POST_PROCESS_INPUT_NAME, input->getColorBuffer() );

        ViewportRect vp;
        if (dst == SCREEN_BUFFER) // Blit to Screen and/or HMD depending on camera setting
        {
            auto curCamera = renderContext.getCamera();
            if (curCamera->isBlittingToScreen())
            {
                // Set viewport (Translate to pixel coordinates first)
                auto viewport = curCamera->getViewport();
                vp.topLeftX = viewport.topLeftX * m_window->getWidth();
                vp.topLeftY = viewport.topLeftY * m_window->getHeight();
                vp.width    = viewport.width    * m_window->getWidth();
                vp.height   = viewport.height   * m_window->getHeight();

                m_swapchain.bindForRendering();
                _DrawFullScreenQuad( material, vp );
            }

            if (curCamera->isBlittingToHMD())
            {
                if (not hasHMD())
                {
                    LOG_WARN_RENDERING("Camera has setting render to eye, but VR is not supported!");
                    return;
                }

                // Ignore viewport from camera, always use full resolution from HMD
                auto desc = m_hmd->getDescription();
                auto eye = curCamera->getHMDEye();
                vp.width  = (F32)desc.idealResolution[eye].x;
                vp.height = (F32)desc.idealResolution[eye].y;

                m_hmd->bindForRendering( eye );
                _DrawFullScreenQuad( material, vp );
            }
        }
        else
        {
            vp = { 0, 0, (F32)dst->getWidth(), (F32)dst->getHeight() };
            _DrawFullScreenQuad( material, vp );
        }
    }

    //----------------------------------------------------------------------
    void VkRenderer::_DrawFullScreenQuad( const MaterialPtr& material, const ViewportRect& viewport )
    {
        renderContext.BindShader( material->getShader() );
        renderContext.BindMaterial( material );

        g_vulkan.ctx.RSSetViewports({ viewport.topLeftX, viewport.topLeftY, viewport.width, viewport.height, 0.0f, 1.0f });
        g_vulkan.ctx.RSSetScissor({ {(I32)viewport.topLeftX, (I32)viewport.topLeftY}, { (U32)viewport.width, (U32)viewport.height } });
        g_vulkan.ctx.IASetPrimitiveTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
        g_vulkan.ctx.Draw(3);
    }

    //----------------------------------------------------------------------
    bool VkRenderer::_UpdateGlobalBuffer( StringID name, const void* data )
    {
        if (not m_globalBuffer)
            return false;
        return m_globalBuffer->update( name, data );
    }

    //----------------------------------------------------------------------
    void VkRenderer::_CreateFakeShadowMaps()
    {
        // Gets rid of the warnings that a texture is not bound to a shadowmap slot
        for (auto i = 0; i < SHADOW_MAP_2D_RESOURCE_DECLS.size(); ++i)
        {
            m_fakeShadowMaps2D[i] = createTexture2D();
            m_fakeShadowMaps2D[i]->create( 2, 2, Graphics::TextureFormat::R8, false );
        }

        for (auto i = 0; i < SHADOW_MAP_3D_RESOURCE_DECLS.size(); ++i)
        {
            m_fakeShadowMaps3D[i] = createCubemap();
            m_fakeShadowMaps3D[i]->create( 2, Graphics::TextureFormat::R8 );
        }

        for (auto i = 0; i < SHADOW_MAP_ARRAY_RESOURCE_DECLS.size(); ++i)
        {
            m_fakeShadowMaps2DArray[i] = createTexture2DArray();
            m_fakeShadowMaps2DArray[i]->create( 2, 2, 1, Graphics::TextureFormat::R8 );
        }
    }

    //**********************************************************************
    // RENDER CONTEXT
    //**********************************************************************

    //----------------------------------------------------------------------
    void VkRenderer::RenderContext::BindShader( const std::shared_ptr<IShader>& shader )
    {
        // Don't bind same shader shader again
        if (shader == m_shader)
            return;

        // Unbind previous shader
        if (m_shader)
            m_shader->unbind();

        m_shader = shader;
        m_shader->bind();
    }

    //----------------------------------------------------------------------
    void VkRenderer::RenderContext::BindMaterial( const MaterialPtr& material )
    {
        // Don't bind same material again
        if (material == m_material)
            return;

        m_material = material;
        m_material->bind();
    }

    //----------------------------------------------------------------------
    void VkRenderer::RenderContext::Reset()
    {
        m_camera = nullptr;
        m_shader = nullptr;
        m_material = nullptr;
        lightCount = 0;
        lightsUpdated = true;
        m_renderTarget = nullptr;
    }

    //----------------------------------------------------------------------
    void VkRenderer::RenderContext::BindRendertarget( const RenderTexturePtr& rt, U64 frameCount )
    {
        m_renderTarget = rt;
        if (m_renderTarget)
            m_renderTarget->bindForRendering( frameCount );
    }

    //----------------------------------------------------------------------
    void VkRenderer::RenderContext::SetCamera( Camera* camera )
    {
        m_camera = camera;

        // Reset frame info struct
        m_camera->getFrameInfo() = {};
    }

} // End namespaces