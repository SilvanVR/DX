#include "VkRenderer.h"
/**********************************************************************
    class: VkRenderer

    author: S. Hau
    date: August 9, 2018
**********************************************************************/

#include "command_buffer.h"
#include "OS/FileSystem/file.h"
#include "Lighting/lights.h"
#include "camera.h"
#include "VR/OculusRift/oculus_rift_vk.h"
#include "Vulkan.hpp"

#include "i_material.h"
#include "i_shader.h"
#include "i_mesh.h"

namespace Graphics {

    using namespace Vulkan;

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

        //VR::Device hmd = VR::GetFirstSupportedHMDAndInitialize();
        //switch (hmd)
        //{
        //case VR::Device::OculusRift:
        //{
        //    auto vkOculus = new VR::OculusRiftVk();
        //    g_vulkan.CreateInstance( vkOculus->getRequiredInstanceExtentions() );
        //    m_swapchain.init( m_window );
        //    g_vulkan.SelectPhysicalDevice( vkOculus->getPhysicalDevice( g_vulkan.instance ) );
        //    g_vulkan.CreateDevice( m_swapchain.getSurfaceKHR(), vkOculus->getRequiredDeviceExtentions(), GetDeviceFeatures() );
        //    vkOculus->setSynchronizationQueueVk( g_vulkan.graphicsQueue );
        //    vkOculus->createEyeBuffers( g_vulkan.device );
        //    m_swapchain.create( g_vulkan.gpu.physicalDevice, g_vulkan.device );
        //    m_hmd = vkOculus; 
        //    break;
        //}
        //default:
        //    LOG_WARN_RENDERING( "VR not supported on your system." );
        //}

        if ( not hasHMD() )
        {
            g_vulkan.CreateInstance({ VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME });
            m_swapchain.init( g_vulkan.instance, m_window );
            g_vulkan.SelectPhysicalDevice();
            g_vulkan.CreateDevice( m_swapchain.getSurfaceKHR(), { VK_KHR_SWAPCHAIN_EXTENSION_NAME }, GetDeviceFeatures() );
            m_swapchain.create( g_vulkan.gpu.physicalDevice, g_vulkan.device );
        }

        _SetGPUDescription();
        _CreateGlobalBuffer();
        _CreateCubeMesh();
        LOG_RENDERING( "Done initializing Vulkan... (Using " + getGPUDescription().name + ")" );
    } 

    //----------------------------------------------------------------------
    void VkRenderer::shutdown()
    {
        g_vulkan.Shutdown();
        SAFE_DELETE( m_hmd );
        SAFE_DELETE( m_cubeMesh );
        renderContext.Reset();
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
                    //D3D11_VIEWPORT vp = { 0, 0, (F32)currRT->getWidth(), (F32)currRT->getHeight(), 0, 1 };
                    //_DrawFullScreenQuad( cmd.material, vp );
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
                    //const D3D11_RECT r = { cmd.rect.left, cmd.rect.top, cmd.rect.right, cmd.rect.bottom };
                    //g_pImmediateContext->RSSetScissorRects( 1, &r );
                    break;
                }
                case GPUCommand::SET_CAMERA_MATRIX:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetCameraMatrix*>( command.get() );
    /*                if ( not CAMERA_BUFFER.update( cmd.name, &cmd.matrix ) )
                        LOG_WARN_RENDERING( "D3D11: Could not update the camera buffer ["+cmd.name.toString()+"]." );
                    else
                        CAMERA_BUFFER.flush();*/
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

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void VkRenderer::present()
    {
        // Execute command buffers
        _LockQueue();
        for (auto& cmd : m_pendingCmdQueue)
            _ExecuteCommandBuffer( cmd );
        m_pendingCmdQueue.clear();
        _UnlockQueue();

        // Present rendered images
        bool vsync = m_vsync;
        if ( hasHMD() )
        {
            m_hmd->distortAndPresent( m_frameCount );
            if ( m_hmd->isMounted() )
                vsync = false;
        }
        //m_pSwapchain->present( vsync );

        m_frameCount++;
    }

    //----------------------------------------------------------------------
    IMesh*              VkRenderer::createMesh()           { return nullptr; }//  { return new Vulkan::Mesh(); }
    IMaterial*          VkRenderer::createMaterial()       { return nullptr; }//  { return new Vulkan::Material(); }
    IShader*            VkRenderer::createShader()         { return nullptr; }//  { return new Vulkan::Shader(); }
    ITexture2D*         VkRenderer::createTexture2D()      { return nullptr; }//  { return new Vulkan::Texture2D(); }
    IRenderTexture*     VkRenderer::createRenderTexture()  { return nullptr; }//  { return new Vulkan::RenderTexture(); }
    ICubemap*           VkRenderer::createCubemap()        { return nullptr; }//  { return new Vulkan::Cubemap(); }
    ITexture2DArray*    VkRenderer::createTexture2DArray() { return nullptr; }//  { return new Vulkan::Texture2DArray(); }
    IRenderBuffer*      VkRenderer::createRenderBuffer()   { return nullptr; }//  { return new Vulkan::RenderBuffer(); }

    //----------------------------------------------------------------------
    bool VkRenderer::setGlobalFloat( StringID name, F32 value )
    {
        return false;
    }

    //----------------------------------------------------------------------
    bool VkRenderer::setGlobalInt( StringID name, I32 value )
    {
        return false;
    }

    //----------------------------------------------------------------------
    bool VkRenderer::setGlobalVector4( StringID name, const Math::Vec4& vec4 )
    {
        return false;
    }

    //----------------------------------------------------------------------
    bool VkRenderer::setGlobalColor( StringID name, Color color )
    {
        return false;
    }

    //----------------------------------------------------------------------
    bool VkRenderer::setGlobalMatrix( StringID name, const DirectX::XMMATRIX& matrix )
    {
        return false;
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
        renderContext.BindRendertarget( renderTarget, m_frameCount );

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

        if ( camera->isBlittingToScreen() )
        {
            //D3D11_VIEWPORT vp = { 0, 0, (F32)renderTarget->getWidth(), (F32)renderTarget->getHeight(), 0, 1 };
            //g_pImmediateContext->RSSetViewports( 1, &vp );
        }
        else
        {
            // Set viewport (Translate to pixel coordinates first)
            //D3D11_VIEWPORT vp = {};
            //auto viewport = camera->getViewport();
            //vp.TopLeftX = viewport.topLeftX * renderTarget->getWidth();
            //vp.TopLeftY = viewport.topLeftY * renderTarget->getHeight();
            //vp.Width    = viewport.width    * renderTarget->getWidth();
            //vp.Height   = viewport.height   * renderTarget->getHeight();
            //vp.MaxDepth = 1.0f;
            //g_pImmediateContext->RSSetViewports( 1, &vp );
        }

        // Update camera buffer
    /*    if ( not CAMERA_BUFFER.update( CAM_VIEW_PROJ_NAME, &camera->getViewProjectionMatrix() ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the view-projection matrix in the camera buffer!" );

        auto modelMatrix = camera->getModelMatrix();
        auto translation = modelMatrix.r[3];
        if ( not CAMERA_BUFFER.update( CAM_POS_NAME, &translation ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer [position]. Fix this!" );

        auto zFar = camera->getZFar();
        if ( not CAMERA_BUFFER.update( CAM_ZFAR_NAME, &zFar ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer [zFar]. Fix this!" );

        auto zNear = camera->getZNear();
        if ( not CAMERA_BUFFER.update( CAM_ZNEAR_NAME, &zNear ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer [zNear]. Fix this!" );

        if ( not CAMERA_BUFFER.update( CAM_VIEW_MATRIX_NAME, &camera->getViewMatrix() ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer [View]. Fix this!" );

        if ( not CAMERA_BUFFER.update( CAM_PROJ_MATRIX_NAME, &camera->getProjectionMatrix() ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer [Projection]. Fix this!" );

        CAMERA_BUFFER.flush();*/
    }

    //----------------------------------------------------------------------
    void VkRenderer::_BindMesh( IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
    {
    }

    //----------------------------------------------------------------------
    void VkRenderer::_DrawMesh( IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
    {
    }

    //----------------------------------------------------------------------
    void VkRenderer::_DrawMeshInstanced( IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 instanceCount )
    {
    }

    //----------------------------------------------------------------------
    void VkRenderer::_FlushLightBuffer()
    {
    }

    //----------------------------------------------------------------------
    void VkRenderer::_CreateGlobalBuffer()
    {
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
        //m_cubeMesh = createMesh();
        //m_cubeMesh->setVertices( cubeVertices );
        //m_cubeMesh->setIndices( cubeIndices );
    }

    //----------------------------------------------------------------------
    void VkRenderer::_CopyTexture( ITexture* srcTex, I32 srcElement, I32 srcMip, ITexture* dstTex, I32 dstElement, I32 dstMip )
    {
    }

    //----------------------------------------------------------------------
    void VkRenderer::_RenderCubemap( ICubemap* cubemap, const MaterialPtr& material, U32 dstMip )
    {
    }

    //----------------------------------------------------------------------
    void VkRenderer::_Blit( const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material )
    {
    }

    //----------------------------------------------------------------------
    void VkRenderer::_DrawFullScreenQuad( const MaterialPtr& material, const ViewportRect& viewport )
    {
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
        lightsUpdated = false;
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