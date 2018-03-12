#include "D3D11Renderer.h"
/**********************************************************************
    class: D3D11Renderer (D3D11Renderer.cpp)

    author: S. Hau
    date: November 28, 2017
**********************************************************************/

#include "Pipeline/Shaders/D3D11Shaders.h"
#include "Pipeline/Buffers/D3D11Buffers.h"
#include "../command_buffer.h"
#include "../render_texture.h"
#include "Resources/D3D11Mesh.h"
#include "Resources/D3D11Material.h"
#include "Resources/D3D11Shader.h"

using namespace DirectX;

namespace Graphics {

    ID3D11DepthStencilState*    pDepthStencilState;
    ID3D11RasterizerState*      pRSState;

    D3D11::ConstantBuffer*  pConstantBufferObject = nullptr;
    D3D11::ConstantBuffer*  pConstantBufferCamera = nullptr;

    //**********************************************************************
    // INIT STUFF
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::init()
    {
        _InitD3D11();

        {
            // Buffers
            pConstantBufferCamera = new D3D11::ConstantBuffer( sizeof(XMMATRIX), BufferUsage::FREQUENTLY );
            pConstantBufferObject = new D3D11::ConstantBuffer( sizeof(XMMATRIX), BufferUsage::FREQUENTLY );
        }

        {
            D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};

            depthStencilStateDesc.DepthEnable = TRUE;
            depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
            depthStencilStateDesc.StencilEnable = FALSE;

            HR( g_pDevice->CreateDepthStencilState(&depthStencilStateDesc, &pDepthStencilState) );
        }

        {
            D3D11_RASTERIZER_DESC rsDesc = {};
            rsDesc.FillMode = D3D11_FILL_SOLID;
            rsDesc.CullMode = D3D11_CULL_NONE;
            rsDesc.FrontCounterClockwise = false;
            rsDesc.DepthClipEnable = true;
            rsDesc.MultisampleEnable = true;
            HR(g_pDevice->CreateRasterizerState(&rsDesc, &pRSState));
        }
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::shutdown()
    {
        SAFE_RELEASE(pDepthStencilState);
        SAFE_RELEASE(pRSState);
        SAFE_DELETE(pConstantBufferCamera);
        SAFE_DELETE(pConstantBufferObject);
        _DeinitD3D11();
    }

    static RenderTexture* s_currentRenderTarget = nullptr;

    //----------------------------------------------------------------------
    void D3D11Renderer::dispatch( const CommandBuffer& cmd )
    {
        // Depth Prepass first
        // Render in offscreen framebuffer and blit result to the swapchain

        // Set Pipeline States
        pConstantBufferCamera->bind(0);
        pConstantBufferObject->bind(1);

        g_pImmediateContext->OMSetDepthStencilState(pDepthStencilState, 0);
        g_pImmediateContext->RSSetState(pRSState);

        // Just sort drawcalls quickly by material
        HashMap<Material*, ArrayList<U32>> sortedMaterials;

        auto& commands = cmd.getGPUCommands();
        for ( U32 i = 0; i < commands.size(); i++ )
        {
            switch ( commands[i]->getType() )
            {
                case GPUCommand::SET_RENDER_TARGET:
                {
                    GPUC_SetRenderTarget& c = *dynamic_cast<GPUC_SetRenderTarget*>( commands[i].get() );
                    _SetRenderTarget( c.renderTarget );
                    break;
                }
                case GPUCommand::CLEAR_RENDER_TARGET:
                {
                    GPUC_ClearRenderTarget& c = *dynamic_cast<GPUC_ClearRenderTarget*>( commands[i].get() );
                    _ClearRenderTarget( c.clearColor );
                    break;
                }
                case GPUCommand::SET_VIEWPORT:
                {
                    GPUC_SetViewport& c = *dynamic_cast<GPUC_SetViewport*>( commands[i].get() );
                    _SetViewport( c.viewport );
                    break;
                }
                case GPUCommand::SET_CAMERA_PERSPECTIVE:
                {
                    GPUC_SetCameraPerspective& c = *dynamic_cast<GPUC_SetCameraPerspective*>( commands[i].get() );
                    _SetCameraPerspective( c.view, c.fov, c.zNear, c.zFar );
                    break;
                }
                case GPUCommand::SET_CAMERA_ORTHO:
                {
                    GPUC_SetCameraOrtho& c = *dynamic_cast<GPUC_SetCameraOrtho*>( commands[i].get() );
                    _SetCameraOrtho( c.view, c.left, c.right, c.bottom, c.top, c.zNear, c.zFar );
                    break;
                }
                case GPUCommand::DRAW_MESH:
                {
                    GPUC_DrawMesh& c = *dynamic_cast<GPUC_DrawMesh*>(commands[i].get());
                    sortedMaterials[c.material].push_back( i );
                    break;
                }
                default:
                    WARN_RENDERING( "Unknown GPU Command in given command buffer!" );
            }
        }


        // Now render by material
        for (auto& pair : sortedMaterials)
        {
            auto shader = pair.first->getShader();
            shader->bind();

            for (auto& index : pair.second)
            {
                GPUC_DrawMesh& c = *dynamic_cast<GPUC_DrawMesh*>( commands[index].get() );

                // Update per object buffer
                pConstantBufferObject->update( &c.modelMatrix, sizeof( DirectX::XMMATRIX ) );

                // Bind buffers
                auto mesh = dynamic_cast<D3D11::Mesh*>( c.mesh );
                mesh->bind( shader, c.subMeshIndex );

                shader->drawMesh( mesh, c.subMeshIndex );
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
    Mesh* D3D11Renderer::createMesh()
    {
        return new D3D11::Mesh();
    }

    //----------------------------------------------------------------------
    IMaterial* D3D11Renderer::createMaterial()
    {
        return new D3D11::Material();
    }

    //----------------------------------------------------------------------
    IShader* D3D11Renderer::createShader()
    {
        return new D3D11::Shader();
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
            WARN_RENDERING( "D3D11Renderer: Latest feature level not supported. Fallback to a later version." );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_CreateSwapchain( U32 numSamples )
    {
        auto windowSize = m_window->getSize();
        m_pSwapchain = new D3D11::Swapchain( m_window->getHWND(), windowSize.x, windowSize.y, numSamples );
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
    void D3D11Renderer::_SetRenderTarget( RenderTexture* renderTarget )
    {
        s_currentRenderTarget = renderTarget;

        if (s_currentRenderTarget == nullptr)
        {
            auto renderTargetView = m_pSwapchain->getRenderTargetView();
            auto depthStencilView = m_pSwapchain->getDepthStencilView();
            g_pImmediateContext->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );
        }
        else
        {
            // @TODO: Set rendertexture as target
        }
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_ClearRenderTarget( const Color& clearColor )
    {
        if (s_currentRenderTarget == nullptr)
        {
            m_pSwapchain->clear( clearColor, 1.0f, 0 );
        }
        else
        {
            // @TODO: Clear rendertexture
        }
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_SetCameraPerspective( const DirectX::XMMATRIX& view, F32 fov, F32 zNear, F32 zFar )
    {
        U32 numViewports = 1;
        D3D11_VIEWPORT viewport;
        g_pImmediateContext->RSGetViewports( &numViewports, &viewport);

        F32 ar = viewport.Width / viewport.Height;
        XMMATRIX proj = XMMatrixPerspectiveFovLH( XMConvertToRadians( fov ), ar, zNear, zFar );
        XMMATRIX viewProj = view * proj;

        pConstantBufferCamera->update( &viewProj, sizeof( XMMATRIX ) );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_SetCameraOrtho( const DirectX::XMMATRIX& view, F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
    {
        XMMATRIX proj = XMMatrixOrthographicOffCenterLH( left, right, bottom, top, zNear, zFar );
        XMMATRIX viewProj = view * proj;

        pConstantBufferCamera->update( &viewProj, sizeof( XMMATRIX ) );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_SetViewport( const ViewportRect& viewport )
    {
        D3D11_VIEWPORT vp = {};
        vp.TopLeftX = viewport.topLeftX;
        vp.TopLeftY = viewport.topLeftY;
        vp.Width    = viewport.width;
        vp.Height   = viewport.height;
        vp.MaxDepth = 1.0f;
        g_pImmediateContext->RSSetViewports( 1, &vp );
    }


} // End namespaces