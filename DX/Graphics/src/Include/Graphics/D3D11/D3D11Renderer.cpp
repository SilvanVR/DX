#include "D3D11Renderer.h"
/**********************************************************************
    class: D3D11Renderer (D3D11Renderer.cpp)

    author: S. Hau
    date: November 28, 2017
**********************************************************************/

#include "command_buffer.h"
#include "Resources/D3D11Mesh.h"
#include "Resources/D3D11Material.h"
#include "Resources/D3D11Shader.h"
#include "Resources/D3D11Texture2D.h"
#include "Resources/D3D11RenderTexture.h"
#include "Resources/D3D11Cubemap.h"
#include "Resources/D3D11Texture2DArray.h"
#include "Resources/D3D11RenderBuffer.h"
#include "OS/FileSystem/file.h"
#include "Lighting/lights.h"
#include "D3D11Utility.h"
#include "camera.h"
#include "VR/vr.h"
#include "VR/OculusRift/oculus_rift_dx.h"
#include "Common/string_utils.h"

using namespace DirectX;

namespace Graphics {

    static String GLOBAL_BUFFER_KEYWORD( "global" );
    static String OBJECT_BUFFER_KEYWORD( "object" );
    static String CAMERA_BUFFER_KEYWORD( "camera" );
    static String LIGHT_BUFFER_KEYWORD ( "light"  );

    #define SHADOW_MAP_2D_SLOT_BEGIN    9
    #define SHADOW_MAP_3D_SLOT_BEGIN    13
    #define SHADOW_MAP_ARRAY_SLOT_BEGIN 14

    static ArrayList<ShaderResourceDeclaration> SHADOW_MAP_2D_RESOURCE_DECLS{
        { ShaderType::Fragment, SHADOW_MAP_2D_SLOT_BEGIN + 0, SID("ShadowMap2D"), DataType::Texture2D },
        { ShaderType::Fragment, SHADOW_MAP_2D_SLOT_BEGIN + 1, SID("ShadowMap2D"), DataType::Texture2D },
        { ShaderType::Fragment, SHADOW_MAP_2D_SLOT_BEGIN + 2, SID("ShadowMap2D"), DataType::Texture2D },
        { ShaderType::Fragment, SHADOW_MAP_2D_SLOT_BEGIN + 3, SID("ShadowMap2D"), DataType::Texture2D },
    };
    static ArrayList<ShaderResourceDeclaration> SHADOW_MAP_3D_RESOURCE_DECLS{
        { ShaderType::Fragment, SHADOW_MAP_3D_SLOT_BEGIN + 0, SID("ShadowMapCube"), DataType::TextureCubemap }
    };
    static ArrayList<ShaderResourceDeclaration> SHADOW_MAP_ARRAY_RESOURCE_DECLS{
        { ShaderType::Fragment, SHADOW_MAP_ARRAY_SLOT_BEGIN + 0, SID("ShadowMapArray"), DataType::Texture2D },
    };

    static constexpr StringID LIGHT_COUNT_NAME              = StringID( "_LightCount" );
    static constexpr StringID LIGHT_BUFFER_NAME             = StringID( "_Lights" );
    static constexpr StringID LIGHT_VIEW_PROJ_NAME          = StringID( "_LightViewProj" );
    static constexpr StringID LIGHT_CSM_SPLITS_NAME         = StringID( "_CSMSplits" );
    static constexpr StringID CAM_POS_NAME                  = StringID( "_CameraPos" );
    static constexpr StringID POST_PROCESS_INPUT_NAME       = StringID( "_MainTex" );
    static constexpr StringID CAM_VIEW_PROJ_NAME            = StringID( "_ViewProj" );
    static constexpr StringID CAM_ZNEAR_NAME                = StringID( "_zNear" );
    static constexpr StringID CAM_ZFAR_NAME                 = StringID( "_zFar" );
    static constexpr StringID CAM_VIEW_MATRIX_NAME          = StringID( "_View" );
    static constexpr StringID CAM_PROJ_MATRIX_NAME          = StringID( "_Proj" );

    //**********************************************************************
    // INIT STUFF
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::init()
    {
        _SetLimits();
        _InitD3D11();
        _CreateRequiredUniformBuffersFromFile("/engine/shaders/includes/engineVS.hlsl", "/engine/shaders/includes/enginePS.hlsl");
        _CreateCubeMesh();

        //VR::Device hmd = VR::GetFirstSupportedHMDAndInitialize();
        //switch (hmd)
        //{
        //case VR::Device::OculusRift: m_hmd = new VR::OculusRiftDX(); break;
        //default: LOG_WARN_RENDERING( "VR not supported on your system." );
        //}

        // Gets rid of the warnings that a texture is not bound to a shadowmap slot
        {
            auto tex = createTexture2D();
            tex->create(2, 2, Graphics::TextureFormat::R8, false);
            for (auto& res : SHADOW_MAP_2D_RESOURCE_DECLS)
                tex->bind( res );
            delete tex;

            auto cube = createCubemap();
            cube->create(2, Graphics::TextureFormat::R8);
            for (auto& res : SHADOW_MAP_3D_RESOURCE_DECLS)
                cube->bind( res );
            delete cube;

            auto texArray = createTexture2DArray();
            texArray->create(2, 2, 1, Graphics::TextureFormat::R8);
            for (auto& res : SHADOW_MAP_ARRAY_RESOURCE_DECLS)
                texArray->bind( res );
            delete texArray;
        }
    } 

    //----------------------------------------------------------------------
    void D3D11Renderer::shutdown()
    {
        _DestroyAllTempRenderTargets();
        SAFE_DELETE( m_objectBuffer );
        SAFE_DELETE( m_cameraBuffer );
        SAFE_DELETE( m_globalBuffer );
        SAFE_DELETE( m_lightBuffer );
        SAFE_DELETE( m_hmd );
        SAFE_DELETE( m_cubeMesh );
        renderContext.Reset();
        _DeinitD3D11();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_ExecuteCommandBuffer( const CommandBuffer& cmd )
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
                    D3D11_VIEWPORT vp = { 0, 0, (F32)currRT->getWidth(), (F32)currRT->getHeight(), 0, 1 };
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
                    const D3D11_RECT r = { cmd.rect.left, cmd.rect.top, cmd.rect.right, cmd.rect.bottom };
                    g_pImmediateContext->RSSetScissorRects( 1, &r );
                    break;
                }
                case GPUCommand::SET_CAMERA_MATRIX:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetCameraMatrix*>( command.get() );
                    StringID name;
                    switch (cmd.member)
                    {
                    case CameraMember::View:             name = CAM_VIEW_MATRIX_NAME; break;
                    case CameraMember::Projection:       name = CAM_PROJ_MATRIX_NAME; break;
                    case CameraMember::ViewProjection:   name = CAM_VIEW_PROJ_NAME;   break;
                    default: LOG_WARN_RENDERING("VkRenderer: Command [SET_CAMERA_MATRIX]: Unsupported Camera Matrix.");
                    }
                    m_cameraBuffer->update( name, &cmd.matrix );
                    m_cameraBuffer->flush();
                    break;
                }
                default:
                    LOG_WARN_RENDERING( "Unknown GPU Command in a given command buffer!" );
            }
        }
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::OnWindowSizeChanged( U16 w, U16 h )
    {
        if ( w == 0 || h == 0 ) // Window was minimized
            return;

        g_pImmediateContext->OMSetRenderTargets( 0, 0, 0 );
        m_pSwapchain->recreate( w, h );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::present()
    {
        _CheckAndDestroyTemporaryRenderTargets();

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
        m_pSwapchain->present( vsync );

        m_frameCount++;
    }

    //----------------------------------------------------------------------
    Mesh*               D3D11Renderer::createMesh()             { return new D3D11::Mesh(); }
    IMaterial*          D3D11Renderer::createMaterial()         { return new D3D11::Material(); }
    IShader*            D3D11Renderer::createShader()           { return new D3D11::Shader(); }
    ITexture2D*         D3D11Renderer::createTexture2D()        { return new D3D11::Texture2D(); }
    IRenderTexture*     D3D11Renderer::createRenderTexture()    { return new D3D11::RenderTexture(); }
    ICubemap*           D3D11Renderer::createCubemap()          { return new D3D11::Cubemap(); }
    ITexture2DArray*    D3D11Renderer::createTexture2DArray()   { return new D3D11::Texture2DArray(); }
    IRenderBuffer*      D3D11Renderer::createRenderBuffer()     { return new D3D11::RenderBuffer(); }

    //----------------------------------------------------------------------
    bool D3D11Renderer::setGlobalFloat( StringID name, F32 value )
    {
        if (not _UpdateGlobalBuffer( name, &value ))
        {
            LOG_WARN_RENDERING( "Global-Float '" + name.toString() + "' does not exist. Did you spell it correctly?" );
            return false;
        }
        return true;
    }
    
    //----------------------------------------------------------------------
    bool D3D11Renderer::setGlobalInt( StringID name, I32 value )
    {
        if (not _UpdateGlobalBuffer( name, &value ))
        {
            LOG_WARN_RENDERING( "Global-Int '" + name.toString() + "' does not exist. Did you spell it correctly?" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool D3D11Renderer::setGlobalVector4( StringID name, const Math::Vec4& vec4 )
    {
        if (not _UpdateGlobalBuffer( name, &vec4 ))
        {
            LOG_WARN_RENDERING( "Global-Vec4 '" + name.toString() + "' does not exist. Did you spell it correctly?" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool D3D11Renderer::setGlobalColor( StringID name, Color color )
    {
        if (not _UpdateGlobalBuffer( name, color.normalized().data() ))
        {
            LOG_WARN_RENDERING( "Global-color '" + name.toString() + "' does not exist. Did you spell it correctly?" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool D3D11Renderer::setGlobalMatrix( StringID name, const DirectX::XMMATRIX& matrix )
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
    void D3D11Renderer::_InitD3D11()
    {
        _CreateDeviceAndContext();
        _CreateSwapchain();
        _SetGPUDescription();

        LOG_RENDERING( "Done initializing D3D11... (Using " + getGPUDescription().name + ")" );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_DeinitD3D11()
    {
        SAFE_DELETE( m_pSwapchain );
        SAFE_RELEASE( g_pImmediateContext );
    #ifdef _DEBUG
        _ReportLiveObjects();
    #endif
        SAFE_RELEASE( g_pDevice );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_CreateDeviceAndContext()
    {
        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };

        UINT createDeviceFlags = 0;
    #ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

        D3D_FEATURE_LEVEL featureLevel;
        HR( D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
                               featureLevels, countof(featureLevels), D3D11_SDK_VERSION,
                               &g_pDevice, &featureLevel, &g_pImmediateContext ) );

        if ( featureLevel != featureLevels[0] )
            LOG_WARN_RENDERING( "D3D11Renderer: Latest feature level not supported. Fallback to a later version." );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_CreateSwapchain()
    {
        m_pSwapchain = new D3D11::Swapchain( m_window->getHWND(), m_window->getWidth(), m_window->getHeight(), 1 );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_ReportLiveObjects()
    {
        ID3D11Debug* pDebugDevice = nullptr;
        HR( g_pDevice->QueryInterface( __uuidof( ID3D11Debug ), reinterpret_cast<void**>( &pDebugDevice ) ) );
        HR( pDebugDevice->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL ) );
        SAFE_RELEASE( pDebugDevice );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_SetLimits()
    {
        m_limits.maxLights      = MAX_LIGHTS;
        m_limits.maxShadowmaps  = MAX_SHADOWMAPS_2D + MAX_SHADOWMAPS_3D + MAX_SHADOWMAPS_ARRAY;
        m_limits.maxCascades    = MAX_CSM_SPLITS;
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_SetGPUDescription()
    {
        ComPtr<IDXGIDevice2> pDXGIDevice;
        HR( g_pDevice->QueryInterface( __uuidof(IDXGIDevice2), (void **)&pDXGIDevice.get() ) );

        ComPtr<IDXGIAdapter> pDXGIAdapter;
        HR( pDXGIDevice->GetAdapter( &pDXGIAdapter.get() ) );

        ComPtr<IDXGIFactory2> pIDXGIFactory;
        HR( pDXGIAdapter->GetParent( __uuidof(IDXGIFactory2), (void **)&pIDXGIFactory.get() ) );

        DXGI_ADAPTER_DESC adapterDesc;
        HR( pDXGIAdapter->GetDesc( &adapterDesc ) );

        m_gpuDescription.name = ConvertToString( adapterDesc.Description );
        m_gpuDescription.maxDedicatedMemoryMB = adapterDesc.DedicatedVideoMemory / 1024 / 1024;
    }

    //**********************************************************************
    // PRIVATE - COMMANDS
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::_SetCamera( Camera* camera )
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
            D3D11_VIEWPORT vp = { 0, 0, (F32)renderTarget->getWidth(), (F32)renderTarget->getHeight(), 0, 1 };
            g_pImmediateContext->RSSetViewports( 1, &vp );
        }
        else
        {
            // Set viewport (Translate to pixel coordinates first)
            D3D11_VIEWPORT vp = {};
            auto viewport = camera->getViewport();
            vp.TopLeftX = viewport.topLeftX * renderTarget->getWidth();
            vp.TopLeftY = viewport.topLeftY * renderTarget->getHeight();
            vp.Width    = viewport.width    * renderTarget->getWidth();
            vp.Height   = viewport.height   * renderTarget->getHeight();
            vp.MaxDepth = 1.0f;
            g_pImmediateContext->RSSetViewports( 1, &vp );
        }

        // Update camera buffer
        if ( not m_cameraBuffer->update( CAM_VIEW_PROJ_NAME, &camera->getViewProjectionMatrix() ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the view-projection matrix in the camera buffer!" );

        auto modelMatrix = camera->getModelMatrix();
        auto translation = modelMatrix.r[3];
        if ( not m_cameraBuffer->update( CAM_POS_NAME, &translation ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer [position]. Fix this!" );

        auto zFar = camera->getZFar();
        if ( not m_cameraBuffer->update( CAM_ZFAR_NAME, &zFar ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer [zFar]. Fix this!" );

        auto zNear = camera->getZNear();
        if ( not m_cameraBuffer->update( CAM_ZNEAR_NAME, &zNear ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer [zNear]. Fix this!" );

        if ( not m_cameraBuffer->update( CAM_VIEW_MATRIX_NAME, &camera->getViewMatrix() ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer [View]. Fix this!" );

        if ( not m_cameraBuffer->update( CAM_PROJ_MATRIX_NAME, &camera->getProjectionMatrix() ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer [Projection]. Fix this!" );

        m_cameraBuffer->flush();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_BindMesh( IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
    {
        // Measuring per frame data
        auto curCamera = renderContext.getCamera();
        if (curCamera)
        {
            auto& camInfo = curCamera->getFrameInfo();
            camInfo.drawCalls++;
            camInfo.numTriangles += mesh->getIndexCount( subMeshIndex ) / 3;
            camInfo.numVertices += mesh->getVertexCount(); // This is actually not correct, cause i need the vertex-count from the submesh
        }

        // Update global buffer if necessary
        if (m_globalBuffer)
            m_globalBuffer->flush();

        // Update light buffer if necessary
        if (m_lightBuffer)
            _FlushLightBuffer();

        // Bind shader, possibly a replacement shader
        auto shader = material->getShader();
        if (curCamera)
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
        m_objectBuffer->update( &modelMatrix, sizeof( DirectX::XMMATRIX ) );

        // Bind mesh
        mesh->bind( shader->getVertexLayout(), subMeshIndex );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_DrawMesh( IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
    {
        // Bind everything and submit drawcall
        _BindMesh( mesh, material, modelMatrix, subMeshIndex );
        g_pImmediateContext->DrawIndexed( mesh->getIndexCount( subMeshIndex ), 0, mesh->getBaseVertex( subMeshIndex ) );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_DrawMeshInstanced( IMesh* mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 instanceCount )
    {
        // Bind everything and submit drawcall
        _BindMesh( mesh, material, modelMatrix, 0 );
        g_pImmediateContext->DrawIndexedInstanced( mesh->getIndexCount(0), instanceCount, 0, mesh->getBaseVertex(0), 0 );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_FlushLightBuffer()
    {
        if ( not renderContext.lightsUpdated )
            return;
        renderContext.lightsUpdated = false;

        renderContext.getCamera()->getFrameInfo().numLights = renderContext.lightCount;

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
        m_lightBuffer->flush();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_CreateRequiredUniformBuffersFromFile( const String& engineVS, const String& engineFS )
    {
        try {
            OS::BinaryFile vertFile( engineVS, OS::EFileMode::READ );
            String vertSrc = vertFile.readAll();
            vertSrc += "\
            float4 main( float3 PosL : POSITION ) : SV_POSITION \
            {                                                   \
                return TO_CLIP_SPACE( PosL );                   \
            }";

            OS::BinaryFile fragFile( engineFS, OS::EFileMode::READ );
            String fragSrc = fragFile.readAll();
            fragSrc += "float4 main() : SV_Target                       \
            {                                                           \
                return float4(1,1,1,1) * _Time * _zNear * _LightCount;  \
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
                    if (lower.find( OBJECT_BUFFER_KEYWORD ) != String::npos)
                        if (not m_objectBuffer)
                        {
                            m_objectBuffer = new D3D11::MappedConstantBuffer( ubo, BufferUsage::Frequently );
                            m_objectBuffer->bind( ShaderType::Vertex );
                        }
                        else LOG_WARN_RENDERING( "D3D11Renderer::_CreateGlobalBuffersFromFile(): Found another object ubo." );
                    else if (lower.find( CAMERA_BUFFER_KEYWORD ) != String::npos)
                        if (not m_cameraBuffer)
                        {
                            m_cameraBuffer = new D3D11::MappedConstantBuffer( ubo, BufferUsage::Frequently );
                            m_cameraBuffer->bind( ShaderType::Vertex );
                            m_cameraBuffer->bind( ShaderType::Fragment );
                            m_cameraBuffer->bind( ShaderType::Geometry );
                        }
                        else LOG_WARN_RENDERING( "D3D11Renderer::_CreateGlobalBuffersFromFile(): Found another camera ubo." );
                    else if(lower.find( GLOBAL_BUFFER_KEYWORD ) != String::npos)
                        if (not m_globalBuffer)
                        {
                            m_globalBuffer = new D3D11::MappedConstantBuffer( ubo, BufferUsage::Frequently );
                            m_globalBuffer->bind( ShaderType::Vertex );
                            m_globalBuffer->bind( ShaderType::Fragment );
                            m_globalBuffer->bind( ShaderType::Geometry );
                        }
                        else LOG_WARN_RENDERING( "D3D11Renderer::_CreateGlobalBuffersFromFile(): Found another global ubo." );
                    else if(lower.find( LIGHT_BUFFER_KEYWORD ) != String::npos)
                        if (not m_lightBuffer)
                        {
                            m_lightBuffer = new D3D11::MappedConstantBuffer( ubo, BufferUsage::Frequently );
                            m_lightBuffer->bind( ShaderType::Fragment );
                        }
                        else LOG_WARN_RENDERING( "D3D11Renderer::_CreateGlobalBuffersFromFile(): Found another light ubo." );
                }
                if (not m_objectBuffer) LOG_ERROR_RENDERING( "D3D11Renderer::_CreateGlobalBuffersFromFile(): Could not find object buffer." );
                if (not m_cameraBuffer) LOG_ERROR_RENDERING( "D3D11Renderer::_CreateGlobalBuffersFromFile(): Could not find camera buffer." );
                if (not m_globalBuffer) LOG_ERROR_RENDERING( "D3D11Renderer::_CreateGlobalBuffersFromFile(): Could not find global buffer." );
                if (not m_lightBuffer)  LOG_ERROR_RENDERING( "D3D11Renderer::_CreateGlobalBuffersFromFile(): Could not find light buffer." );
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
    void D3D11Renderer::_CreateCubeMesh()
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
    void D3D11Renderer::_CopyTexture( ITexture* srcTex, I32 srcElement, I32 srcMip, ITexture* dstTex, I32 dstElement, I32 dstMip )
    {
        U32 dstSubResource = D3D11CalcSubresource( dstMip, dstElement, dstTex->getMipCount() );
        U32 srcSubResource = D3D11CalcSubresource( srcMip, srcElement, srcTex->getMipCount() );
        D3D11::IBindableTexture* d3d11DstTex = dynamic_cast<D3D11::IBindableTexture*>( dstTex );
        D3D11::IBindableTexture* d3d11SrcTex = dynamic_cast<D3D11::IBindableTexture*>( srcTex );
        g_pImmediateContext->CopySubresourceRegion( d3d11DstTex->getD3D11Texture(), dstSubResource, 0, 0, 0,
                                                    d3d11SrcTex->getD3D11Texture(), srcSubResource, NULL );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_RenderCubemap( ICubemap* cubemap, const MaterialPtr& material, U32 dstMip )
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

        // Setup viewport matching the render texture
        D3D11_VIEWPORT vp = { 0, 0, (F32)colorBuffer->getWidth(), (F32)colorBuffer->getHeight(), 0, 1 };
        g_pImmediateContext->RSSetViewports( 1, &vp );

        // Render into render texture for each face and copy the result into the cubemaps face
        auto projection = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians( 90.0f ), 1.0f, 0.1f, 10.0f );
        for (I32 face = 0; face < 6; face++)
        {
            colorBuffer->clearColor( Color::BLACK );

            auto view = DirectX::XMMatrixLookToLH( { 0, 0, 0, 0 }, directions[face], ups[face] );
            auto viewProj = view * projection;
            if ( not m_cameraBuffer->update( CAM_VIEW_PROJ_NAME, &viewProj ) )
                LOG_ERROR_RENDERING( "D3D11::RenderCubemap(): Could not update the view-projection matrix in the camera buffer!" );
            m_cameraBuffer->flush();

            _DrawMesh( m_cubeMesh, material, DirectX::XMMatrixIdentity(), 0 );
            _CopyTexture( colorBuffer, 0, 0, cubemap, face, dstMip );

            // Unfortunately on my laptop if i dont flush here the driver crashes sometimes... 
            g_pImmediateContext->Flush();
        }
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_Blit( const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material )
    {
        auto currRT = renderContext.getRenderTarget();
        if (currRT == SCREEN_BUFFER && dst == SCREEN_BUFFER)
            LOG_WARN_RENDERING( "D3D11[Blit]: Target texture was previously screen, so the content will probably be overriden. This can"
                                " occur if two blits in succession with both target = nullptr (to screen) were recorded." );

        // Use the src texture as the input IF not null. Otherwise use the current bound render target.
        auto input = src.get() ? src.get() : currRT;
        if (input == SCREEN_BUFFER)
        {
            LOG_WARN_RENDERING( "D3D11[Blit]: Previous render target was screen, which can't be used as input! This happens when a blit-command "
                                "with src=nullptr (to screen) was recorded but a previous blit had dst=nullptr (to screen)" );
            return;
        }

        // Bind render-target (Note: if dst is null, this does nothing)
        renderContext.BindRendertarget( dst, m_frameCount );

        // Set texture in material
        material->setTexture( POST_PROCESS_INPUT_NAME, input->getColorBuffer() );

        D3D11_VIEWPORT vp = {};
        if (dst == SCREEN_BUFFER) // Blit to Screen and/or HMD depending on camera setting
        {
            auto curCamera = renderContext.getCamera();
            if ( curCamera->isBlittingToScreen() )
            {
                // Set viewport (Translate to pixel coordinates first)
                auto viewport = curCamera->getViewport();
                vp.TopLeftX = viewport.topLeftX * m_window->getWidth();
                vp.TopLeftY = viewport.topLeftY * m_window->getHeight();
                vp.Width    = viewport.width    * m_window->getWidth();
                vp.Height   = viewport.height   * m_window->getHeight();
                vp.MaxDepth = 1.0f;

                m_pSwapchain->bindForRendering();
                _DrawFullScreenQuad( material, vp );
            }

            if ( curCamera->isBlittingToHMD() )
            {
                if ( not hasHMD() )
                {
                    LOG_WARN_RENDERING( "Camera has setting render to eye, but VR is not supported!" );
                    return;
                }

                // Ignore viewport from camera, always use full resolution from HMD
                auto desc = m_hmd->getDescription();
                auto eye = curCamera->getHMDEye();
                vp.TopLeftX = 0.0f;
                vp.TopLeftY = 0.0f;
                vp.Width = (F32)desc.idealResolution[eye].x;
                vp.Height = (F32)desc.idealResolution[eye].y;
                vp.MaxDepth = 1.0f;

                m_hmd->bindForRendering( eye );
                _DrawFullScreenQuad( material, vp );
            }
        }
        else
        {
            vp = { 0, 0, (F32)dst->getWidth(), (F32)dst->getHeight(), 0, 1 };
            _DrawFullScreenQuad( material, vp );
        }
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_DrawFullScreenQuad( const MaterialPtr& material, const D3D11_VIEWPORT& viewport )
    {
        renderContext.BindShader( material->getShader() );
        renderContext.BindMaterial( material );

        g_pImmediateContext->RSSetViewports( 1, &viewport );
        g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
        g_pImmediateContext->Draw( 3, 0 );
    }

    //----------------------------------------------------------------------
    bool D3D11Renderer::_UpdateGlobalBuffer( StringID name, const void* data )
    {
        if (not m_globalBuffer)
            return false;
        return m_globalBuffer->update(name, data);
    }

    //**********************************************************************
    // RENDER CONTEXT
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::RenderContext::BindShader( const std::shared_ptr<IShader>& shader )
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
    void D3D11Renderer::RenderContext::BindMaterial( const MaterialPtr& material )
    {
        // Don't bind same material again
        if (material == m_material)
            return;

        m_material = material;
        m_material->bind();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::RenderContext::Reset()
    {
        m_camera = nullptr;
        m_shader = nullptr;
        m_material = nullptr;
        lightCount = 0;
        lightsUpdated = false;
        m_renderTarget = nullptr;
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::RenderContext::BindRendertarget( const RenderTexturePtr& rt, U64 frameCount )
    {
        // Unbind all shader resources, because the render target might be used as a srv
        ID3D11ShaderResourceView* resourceViews[16] = {};
        g_pImmediateContext->PSSetShaderResources( 0, 16, resourceViews );

        m_renderTarget = rt;
        if (m_renderTarget)
            m_renderTarget->bindForRendering( frameCount );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::RenderContext::SetCamera( Camera* camera )
    {
        m_camera = camera;

        // Reset frame info struct
        m_camera->getFrameInfo() = {};
    }

} // End namespaces