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
#include "OS/FileSystem/file.h"
#include "default_shaders.hpp"

using namespace DirectX;

namespace Graphics {

    #define GLOBAL_BUFFER D3D11::ConstantBufferManager::getGlobalBuffer()
    #define OBJECT_BUFFER D3D11::ConstantBufferManager::getObjectBuffer()
    #define CAMERA_BUFFER D3D11::ConstantBufferManager::getCameraBuffer()
    #define LIGHT_BUFFER  D3D11::ConstantBufferManager::getLightBuffer()

    #define LIGHT_COUNT_NAME    "lightCount"
    #define LIGHT_BUFFER_NAME   "lights"
    #define CAM_VIEW_PROJ_NAME  "gViewProj"
    #define CAM_POS_NAME        "gCameraPos"
    #define MAX_LIGHTS          16

    static StringID viewProjName = SID( CAM_VIEW_PROJ_NAME );

    //----------------------------------------------------------------------
    struct RenderContext
    {
        Camera*      camera     = nullptr;  // Current camera

        Shader*      shader     = nullptr;  // Current bound shader
        Material*    material   = nullptr;  // Current bound material

        IRenderTexture* renderTarget = nullptr; // Current render target

        I32          lightCount = 0;
        const Light* lights[MAX_LIGHTS];
        bool         lightsUpdated = false; // Set to true whenever a new light has been added
    } renderContext;

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
    static IMesh* s_cubeMesh; // Needed for cubemap rendering

    //**********************************************************************
    // INIT STUFF
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::init()
    {
        _InitD3D11();
        _CreateGlobalBuffer();
        s_cubeMesh = createMesh();
        s_cubeMesh->setVertices( cubeVertices );
        s_cubeMesh->setIndices( cubeIndices );
    } 

    //----------------------------------------------------------------------
    void D3D11Renderer::shutdown()
    {
        SAFE_DELETE( s_cubeMesh );
        D3D11::ConstantBufferManager::Destroy();
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
                    renderContext.renderTarget = cmd.target.get();
                    renderContext.renderTarget->bindForRendering();
                    break;
                }
                case GPUCommand::DRAW_FULLSCREEN_QUAD:
                {
                    auto& cmd = *reinterpret_cast<GPUC_DrawFullscreenQuad*>( command.get() );
                    renderContext.shader = cmd.material->getShader().get();
                    renderContext.material = cmd.material.get();

                    renderContext.shader->bind();
                    renderContext.material->bind();

                    D3D11_VIEWPORT vp = { 0, 0, (F32)renderContext.renderTarget->getWidth(), (F32)renderContext.renderTarget->getHeight(), 0, 1 };
                    g_pImmediateContext->RSSetViewports( 1, &vp );
                    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
                    g_pImmediateContext->Draw( 4, 0 );
                    break;
                }
                case GPUCommand::RENDER_CUBEMAP:
                {
                    auto& cmd = *reinterpret_cast<GPUC_RenderCubemap*>( command.get() );
                    _RenderCubemap( cmd.cubemap.get(), cmd.material.get(), cmd.dstMip );
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
        _LockQueue();
        for ( auto& cmd : m_pendingCmdQueue)
            _ExecuteCommandBuffer( cmd );
        m_pendingCmdQueue.clear();
        _UnlockQueue();

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

    //----------------------------------------------------------------------
    bool D3D11Renderer::setGlobalFloat( StringID name, F32 value )
    {
        if ( not D3D11::ConstantBufferManager::hasGlobalBuffer() )
            return false;

        bool success = GLOBAL_BUFFER.update( name, &value );
        if ( not success)
            LOG_WARN_RENDERING( "Global-Float '" + name.toString() + "' does not exist. Did you spell it correctly?" );

        return success;
    }

    //----------------------------------------------------------------------
    bool D3D11Renderer::setGlobalInt( StringID name, I32 value )
    {
        if ( not D3D11::ConstantBufferManager::hasGlobalBuffer() )
            return false;

        bool success = GLOBAL_BUFFER.update( name, &value );
        if ( not success)
            LOG_WARN_RENDERING( "Global-Int '" + name.toString() + "' does not exist. Did you spell it correctly?" );

        return success;
    }

    //----------------------------------------------------------------------
    bool D3D11Renderer::setGlobalVector4( StringID name, const Math::Vec4& vec4 )
    {
        if ( not D3D11::ConstantBufferManager::hasGlobalBuffer() )
            return false;

        bool success = GLOBAL_BUFFER.update( name, &vec4 );
        if ( not success)
            LOG_WARN_RENDERING( "Global-Vec4 '" + name.toString() + "' does not exist. Did you spell it correctly?" );

        return success;
    }

    //----------------------------------------------------------------------
    bool D3D11Renderer::setGlobalColor( StringID name, Color color )
    {
        if ( not D3D11::ConstantBufferManager::hasGlobalBuffer() )
            return false;

        bool success = GLOBAL_BUFFER.update( name, color.normalized().data() );
        if ( not success)
            LOG_WARN_RENDERING( "Global-Color '" + name.toString() + "' does not exist. Did you spell it correctly?" );

        return success;
    }

    //----------------------------------------------------------------------
    bool D3D11Renderer::setGlobalMatrix( StringID name, const DirectX::XMMATRIX& matrix )
    {
        if ( not D3D11::ConstantBufferManager::hasGlobalBuffer() )
            return false;

        bool success = GLOBAL_BUFFER.update( name, &matrix);
        if ( not success)
            LOG_WARN_RENDERING( "Global-Matrix '" + name.toString() + "' does not exist. Did you spell it correctly?" );

        return success;
    }

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
        renderContext.lightsUpdated = true;
        renderContext.material = nullptr;
        renderContext.shader = nullptr;

        // Set rendertarget
        auto renderTarget = camera->getRenderTarget();
        renderContext.renderTarget = renderTarget.get();
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
        XMMATRIX viewProj = camera->getViewMatrix() * camera->getProjectionMatrix();
        if ( not CAMERA_BUFFER.update( viewProjName, &viewProj ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the view-projection matrix in the camera buffer!" );

        static StringID camPosName = SID( CAM_POS_NAME );
        auto modelMatrix = camera->getModelMatrix();
        auto translation = modelMatrix.r[3];
        if ( not CAMERA_BUFFER.update( camPosName, &translation ) )
            LOG_ERROR_RENDERING( "D3D11: Could not update the camera buffer. Fix this!" );

        CAMERA_BUFFER.flush();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_DrawMesh( IMesh* mesh, IMaterial* material, const DirectX::XMMATRIX& modelMatrix, U32 subMeshIndex )
    {
        // Measuring per frame data
        m_frameInfo.drawCalls++;
        m_frameInfo.numTriangles += mesh->getIndexCount( subMeshIndex ) / 3;
        m_frameInfo.numVertices += mesh->getVertexCount(); // This is actually not correct, cause i need the vertex-count from the submesh

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
                renderContext.shader = shader;
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
        if ( not renderContext.lightsUpdated )
            return;
        renderContext.lightsUpdated = false;

        m_frameInfo.numLights = renderContext.lightCount;

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
            F32         range;                  // 4 bytes
            Math::Vec2  PADDING;                // 8 bytes
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
                lights[i].position  = pointLight->getPosition();
                lights[i].range     = pointLight->getRange();
                break;
            }
            case LightType::Spot:
            {
                auto spotLight = reinterpret_cast<const SpotLight*>( renderContext.lights[i] );
                lights[i].position  = spotLight->getPosition();
                lights[i].range     = spotLight->getRange();
                lights[i].spotAngle = spotLight->getAngle();
                lights[i].direction = spotLight->getDirection();
                break;
            }
            default:
                ASSERT( false && "Unknown light type!" );
            }
        }

        // Update gpu buffer
        static StringID lightBufferName = SID( LIGHT_BUFFER_NAME );
        if ( not LIGHT_BUFFER.update( lightBufferName, &lights ) )
            LOG_ERROR_RENDERING( "Failed to update light-buffer. Something is horribly broken! Fix this!" );

        LIGHT_BUFFER.flush();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_CreateGlobalBuffer()
    {
        // This is a small hack in order to create the global buffer as early as possible.
        // Otherwise calling setGlobalFloat(...) etc. does not work before the first shader using the global buffers has been compiled.
        try {
            OS::BinaryFile file( "/shaders/includes/enginePS.hlsl" );

            String fragSrc = file.readAll();
            fragSrc += "float4 main() : SV_Target       \
            {                                           \
                return float4(1,1,1,1) * gTime;         \
            }";

            Shader* shader = createShader();
            if ( not shader->compileFragmentShaderFromSource( fragSrc, "main" ) )
                LOG_WARN_RENDERING( "Could not precompile enginePS.hlsl for buffer creation. This might cause some issues." );
            delete shader;
        } 
        catch (const std::runtime_error& ex)
        {
            LOG_WARN_RENDERING( String( "Could not open enginePS.hlsl. This might cause some issues. Reason: " ) + ex.what() );
        }
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
    void D3D11Renderer::_RenderCubemap( ICubemap* cubemap, IMaterial* material, U32 dstMip )
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
        auto renderTexture = createRenderTexture();
        renderTexture->create( width, height, 0, cubemap->getFormat(), 1 );
        renderTexture->bindForRendering();

        // Setup viewport matching the render texture
        D3D11_VIEWPORT vp = { 0, 0, (F32)renderTexture->getWidth(), (F32)renderTexture->getHeight(), 0, 1 };
        g_pImmediateContext->RSSetViewports( 1, &vp );

        // Render into render texture for each face and copy the result into the cubemaps face
        auto projection = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians( 90.0f ), 1.0f, 0.1f, 10.0f );
        for (I32 face = 0; face < 6; face++)
        {
            renderTexture->clear( Color::BLACK, 1, 0 );

            auto view = DirectX::XMMatrixLookToLH( { 0, 0, 0, 0 }, directions[face], ups[face] );
            auto viewProj = view * projection;
            if ( not CAMERA_BUFFER.update( viewProjName, &viewProj ) )
                LOG_ERROR_RENDERING( "D3D11: Could not update the view-projection matrix in the camera buffer!" );
            CAMERA_BUFFER.flush();

            _DrawMesh( s_cubeMesh, material, DirectX::XMMatrixIdentity(), 0 );
            _CopyTexture( renderTexture, 0, 0, cubemap, face, dstMip );
        }

        SAFE_DELETE( renderTexture );
    }

} // End namespaces