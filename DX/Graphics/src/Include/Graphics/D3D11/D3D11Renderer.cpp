#include "D3D11Renderer.h"
/**********************************************************************
    class: D3D11Renderer (D3D11Renderer.cpp)

    author: S. Hau
    date: November 28, 2017
**********************************************************************/

#include "../command_buffer.h"
#include "Resources/D3D11Mesh.h"
#include "Resources/D3D11Material.h"
#include "Resources/D3D11Shader.h"
#include "Resources/D3D11Texture2D.h"
#include "Resources/D3D11RenderTexture.h"
#include "Resources/D3D11Cubemap.h"
#include "Resources/D3D11Texture2DArray.h"
#include "D3D11ConstantBufferManager.h"
#include "Lighting/directional_light.h"
#include "Lighting/point_light.h"
#include "Lighting/spot_light.h"

using namespace DirectX;

namespace Graphics {

    #define GLOBAL_BUFFER D3D11::ConstantBufferManager::getGlobalBuffer()
    #define OBJECT_BUFFER D3D11::ConstantBufferManager::getObjectBuffer()
    #define CAMERA_BUFFER D3D11::ConstantBufferManager::getCameraBuffer()
    #define LIGHT_BUFFER  D3D11::ConstantBufferManager::getLightBuffer()

    #define LIGHT_COUNT_NAME    "lightCount"
    #define CAM_VIEW_PROJ_NAME  "gViewProj"
    #define CAM_POS_NAME        "gCameraPos"
    #define MAX_LIGHTS          16

    //----------------------------------------------------------------------
    struct RenderContext
    {
        Camera*      camera     = nullptr;

        Shader*      shader     = nullptr;
        Material*    material   = nullptr;

        I32          lightCount = 0;
        const Light* lights[MAX_LIGHTS];
        bool         lightsUpdated = false;
    } renderContext;

    //**********************************************************************
    // INIT STUFF
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::init()
    {
        _InitD3D11();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::shutdown()
    {
        D3D11::ConstantBufferManager::Destroy();
        _DeinitD3D11();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::dispatch( const CommandBuffer& cmd )
    {
        auto& commands = cmd.getGPUCommands();

        for ( auto& command : commands )
        {
            switch ( command->getType() )
            {
                case GPUCommand::SET_CAMERA:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetCamera*>( command.get() );
                    _SetCamera( cmd.camera );
                    break;
                }
                case GPUCommand::DRAW_MESH:
                {
                    auto& cmd = *reinterpret_cast<GPUC_DrawMesh*>( command.get() );
                    _DrawMesh( cmd.mesh.get(), cmd.material.get(), cmd.modelMatrix, cmd.subMeshIndex );
                    break;
                }
                case GPUCommand::COPY_TEXTURE:
                {
                    auto& cmd = *reinterpret_cast<GPUC_CopyTexture*>( command.get() );
                    U32 dstElement = D3D11CalcSubresource( cmd.dstMip, cmd.dstElement, cmd.dstTex->getMipCount() );
                    U32 srcElement = D3D11CalcSubresource( cmd.srcMip, cmd.srcElement, cmd.srcTex->getMipCount() );
                    D3D11::IBindableTexture* dstTex = reinterpret_cast<D3D11::IBindableTexture*>( cmd.dstTex );
                    D3D11::IBindableTexture* srcTex = reinterpret_cast<D3D11::IBindableTexture*>( cmd.srcTex );
                    g_pImmediateContext->CopySubresourceRegion( dstTex->getD3D11Texture(), dstElement, 0, 0, 0, 
                                                                srcTex->getD3D11Texture(), srcElement, NULL );
                    break;
                }
                case GPUCommand::SET_GLOBAL_FLOAT:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetGlobalFloat*>( command.get() );
                    if ( not D3D11::ConstantBufferManager::hasGlobalBuffer() )
                        break;
                    if ( not GLOBAL_BUFFER.update( cmd.name, &cmd.value) )
                        LOG_WARN_RENDERING( "Global-Float '" + cmd.name.toString() + "' does not exist. Did you spell it correctly?" );
                    break;
                }
                case GPUCommand::SET_GLOBAL_VECTOR:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetGlobalVector*>( command.get() );
                    if ( not D3D11::ConstantBufferManager::hasGlobalBuffer() )
                        break;
                    if ( not GLOBAL_BUFFER.update( cmd.name, &cmd.vec ) )
                        LOG_WARN_RENDERING( "Global-Vec4 '" + cmd.name.toString() + "' does not exist. Did you spell it correctly?" );
                    break;
                }
                case GPUCommand::SET_GLOBAL_INT:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetGlobalInt*>( command.get() );
                    if ( not D3D11::ConstantBufferManager::hasGlobalBuffer() )
                        break;
                    if ( not GLOBAL_BUFFER.update( cmd.name, &cmd.value ) )
                        LOG_WARN_RENDERING( "Global-Int '" + cmd.name.toString() + "' does not exist. Did you spell it correctly?" );
                    break;
                }
                case GPUCommand::SET_GLOBAL_MATRIX:
                {
                    auto& cmd = *reinterpret_cast<GPUC_SetGlobalMatrix*>( command.get() );
                    if ( not D3D11::ConstantBufferManager::hasGlobalBuffer() )
                        break;
                    if ( not GLOBAL_BUFFER.update( cmd.name, &cmd.matrix ) )
                        LOG_WARN_RENDERING( "Global-Matrix '" + cmd.name.toString() + "' does not exist. Did you spell it correctly?" );
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

        m_pSwapchain->recreate( w, h );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::present()
    {
        m_pSwapchain->present( m_vsync );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::setMultiSampleCount( U32 numSamples )
    {
        if( not m_pSwapchain->numMSAASamplesSupported( numSamples ) )
            return;

        // Recreate Swapchain
        SAFE_DELETE( m_pSwapchain );
        _CreateSwapchain( numSamples );
    }

    //----------------------------------------------------------------------
    Mesh*               D3D11Renderer::createMesh()             { return new D3D11::Mesh(); }
    IMaterial*          D3D11Renderer::createMaterial()         { return new D3D11::Material(); }
    IShader*            D3D11Renderer::createShader()           { return new D3D11::Shader(); }
    ITexture2D*         D3D11Renderer::createTexture2D()        { return new D3D11::Texture2D(); }
    IRenderTexture*     D3D11Renderer::createRenderTexture()    { return new D3D11::RenderTexture(); }
    ICubemap*           D3D11Renderer::createCubemap()          { return new D3D11::Cubemap(); }
    ITexture2DArray*    D3D11Renderer::createTexture2DArray()   { return new D3D11::Texture2DArray(); }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::_InitD3D11()
    {
        _CreateDeviceAndContext();
        _CreateSwapchain( INITIAL_MSAA_SAMPLES );

        LOG_RENDERING( "Done initializing D3D11..." );
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
                               featureLevels, _countof( featureLevels ), D3D11_SDK_VERSION,
                               &g_pDevice, &featureLevel, &g_pImmediateContext ) );

        if ( featureLevel != featureLevels[0] )
            LOG_WARN_RENDERING( "D3D11Renderer: Latest feature level not supported. Fallback to a later version." );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_CreateSwapchain( U32 numSamples )
    {
        auto windowSize = s_window->getSize();
        m_pSwapchain = new D3D11::Swapchain( s_window->getHWND(), windowSize.x, windowSize.y, numSamples );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_ReportLiveObjects()
    {
        ID3D11Debug* pDebugDevice = nullptr;
        HR( g_pDevice->QueryInterface( __uuidof( ID3D11Debug ), reinterpret_cast<void**>( &pDebugDevice ) ) );
        HR( pDebugDevice->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL ) );
        SAFE_RELEASE( pDebugDevice );
    }

    //**********************************************************************
    // PRIVATE - COMMANDS
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::_SetCamera( Camera* camera )
    {
        renderContext.camera = camera;
        renderContext.lightCount = 0;

        // Set rendertarget
        auto renderTarget = camera->getRenderTarget();
        if ( renderTarget == nullptr )
        {
            m_pSwapchain->bind();
        }
        else
        {
            // Unbind all shader resources, because the render target might be used as a srv
            ID3D11ShaderResourceView* resourceViews[16] = {};
            g_pImmediateContext->PSSetShaderResources( 0, 16, resourceViews );
            renderTarget->bindForRendering();
        }

        // Clear rendertarget
        switch ( camera->getClearMode() )
        {
        case CameraClearMode::None: break;
        case CameraClearMode::Color:
            renderTarget == nullptr ? m_pSwapchain->clear( camera->getClearColor(), 1.0f, 0 ) : renderTarget->clear( camera->getClearColor(), 1.0f, 0 );
            break;
        case CameraClearMode::Depth:
            renderTarget == nullptr ? m_pSwapchain->clearDepthStencil( 1.0f, 0 ) : renderTarget->clearDepthStencil( 1.0f, 0 );
            break;
        default: LOG_WARN_RENDERING( "Unknown Clear-Mode in camera!" );
        }

        // Set viewport (Translate to pixel coordinates first)
        D3D11_VIEWPORT vp = {};
        auto viewport = camera->getViewport();
        if ( renderTarget == nullptr )
        {
            vp.TopLeftX = viewport.topLeftX * s_window->getWidth();
            vp.TopLeftY = viewport.topLeftY * s_window->getHeight();
            vp.Width    = viewport.width    * s_window->getWidth();
            vp.Height   = viewport.height   * s_window->getHeight();
            vp.MaxDepth = 1.0f;
        }
        else
        {
            vp.TopLeftX = viewport.topLeftX * renderTarget->getWidth();
            vp.TopLeftY = viewport.topLeftY * renderTarget->getHeight();
            vp.Width    = viewport.width    * renderTarget->getWidth();
            vp.Height   = viewport.height   * renderTarget->getHeight();
            vp.MaxDepth = 1.0f;
        }
        g_pImmediateContext->RSSetViewports( 1, &vp );

        // Update camera buffer
        static StringID viewProjName = SID( CAM_VIEW_PROJ_NAME );
        XMMATRIX viewProj = camera->getViewMatrix() * camera->getProjectionMatrix();
        CAMERA_BUFFER.update( viewProjName, &viewProj );

        static StringID camPosName = SID( CAM_POS_NAME );
        auto modelMatrix = DirectX::XMMatrixInverse( nullptr, camera->getViewMatrix() );        
        auto translation = modelMatrix.r[3];
        CAMERA_BUFFER.update( camPosName, &translation );

        CAMERA_BUFFER.flush();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_DrawMesh( IMesh* mesh, IMaterial* material, const DirectX::XMMATRIX& modelMatrix, U32 subMeshIndex )
    {
        // Measuring per frame data
        m_frameInfo.drawCalls++;
        m_frameInfo.numVertices += mesh->getVertexCount();
        for (auto i = 0; i < mesh->getSubMeshCount(); i++)
            m_frameInfo.numTriangles += mesh->getIndexCount(i) / 3;

        // Update global buffer if necessary
        if ( D3D11::ConstantBufferManager::hasGlobalBuffer() )
            GLOBAL_BUFFER.flush();

        // Update light buffer if necessary
        if ( D3D11::ConstantBufferManager::hasLightBuffer() )
            _FlushLightBuffer();

        Shader* shader = renderContext.shader;
        if (m_activeGlobalMaterial)
        {
            if (m_activeGlobalMaterial != renderContext.material)
            {
                shader = m_activeGlobalMaterial->getShader().get();
                shader->bind();
                renderContext.shader = m_activeGlobalMaterial->getShader().get();
                renderContext.material = m_activeGlobalMaterial;
            }
        }
        else
        {
            // Bind shader if not already bound
            if ( material->getShader().get() != renderContext.shader )
            {
                shader = material->getShader().get();
                shader->bind();
                renderContext.shader = material->getShader().get();
            }

            // Bind material if not already bound
            if ( material != renderContext.material )
            {
                material->bind();
                renderContext.material = material;
            }
        }

        // Update per object buffer
        OBJECT_BUFFER.update( &modelMatrix, sizeof( DirectX::XMMATRIX ) );

        // Bind mesh
        auto d3d11Mesh = reinterpret_cast<D3D11::Mesh*>( mesh );
        d3d11Mesh->bind( shader->getVertexLayout(), subMeshIndex );

        // Submit draw call
        g_pImmediateContext->DrawIndexed( mesh->getIndexCount( subMeshIndex ), 0, mesh->getBaseVertex( subMeshIndex ) );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_FlushLightBuffer()
    {
        if ( renderContext.lightCount == 0 || not renderContext.lightsUpdated )
            return;

        // Update light count
        static StringID lightCountName = SID( LIGHT_COUNT_NAME );
        if ( not LIGHT_BUFFER.update( lightCountName, &renderContext.lightCount ) )
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
            F32         constantAttenuation;    // 4 bytes
            F32         linearAttenuation;      // 4 bytes
            F32         quadraticAttenuation;   // 4 bytes
            //----------------------------------- (16 byte boundary)
        } lights[MAX_LIGHTS];

        // Update light array
        for (I32 i = 0; i < renderContext.lightCount; i++)
        {
            lights[i].color     = renderContext.lights[i]->getColor().normalized();
            lights[i].intensity = renderContext.lights[i]->getIntensity();
            lights[i].lightType = (I32)renderContext.lights[i]->getLightType();

            switch ( renderContext.lights[i]->getLightType() )
            {
            case LightType::Directional:
            {
                auto dirLight = reinterpret_cast<const DirectionalLight*>( renderContext.lights[i] );
                lights[i].direction = dirLight->getDirection();
                break;
            }
            case LightType::Point:
            {
                auto pointLight = reinterpret_cast<const PointLight*>( renderContext.lights[i] );
                lights[i].position              = pointLight->getPosition();
                lights[i].constantAttenuation   = pointLight->getConstantAttenuation();
                lights[i].linearAttenuation     = pointLight->getLinearAttenuation();
                lights[i].quadraticAttenuation  = pointLight->getQuadraticAttenuation();
                break;
            }
            case LightType::Spot:
            {
                auto spotLight = reinterpret_cast<const SpotLight*>( renderContext.lights[i] );
                lights[i].position              = spotLight->getPosition();
                lights[i].constantAttenuation   = spotLight->getConstantAttenuation();
                lights[i].linearAttenuation     = spotLight->getLinearAttenuation();
                lights[i].quadraticAttenuation  = spotLight->getQuadraticAttenuation();
                lights[i].spotAngle             = spotLight->getAngle();
                lights[i].direction             = spotLight->getDirection();
                break;
            }
            default:
                ASSERT( false && "Unknown light type!" );
            }
        }

        // Update gpu buffer
        static StringID lightName = SID( "lights" );
        if ( not LIGHT_BUFFER.update( lightName, &lights ) )
            LOG_ERROR_RENDERING( "Failed to update light-buffer. Something is horribly broken! Fix this!" );

        LIGHT_BUFFER.flush();
    }

} // End namespaces