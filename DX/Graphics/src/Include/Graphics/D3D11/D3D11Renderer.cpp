#include "D3D11Renderer.h"
/**********************************************************************
    class: D3D11Renderer (D3D11Renderer.cpp)

    author: S. Hau
    date: November 28, 2017
**********************************************************************/

#include "Pipeline/Shaders/D3D11Shader.h"
#include "Pipeline/Buffers/D3D11Buffers.h"
#include "Pipeline/D3D11Pass.h"
#include "../command_buffer.h"
#include "../render_texture.h"
#include "Resources/D3D11Mesh.h"
#include "Resources/D3D11Material.h"

using namespace DirectX;

namespace Graphics {

    // @TODO: Query from shader interface
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    ID3D11InputLayout*          pInputLayout;
    ID3D11DepthStencilState*    pDepthStencilState;
    ID3D11RasterizerState*      pRSState;

    D3D11::VertexShader*    pVertexShader = nullptr;
    D3D11::PixelShader*     pPixelShader = nullptr;
    D3D11::ConstantBuffer*  pConstantBufferObject = nullptr;
    D3D11::ConstantBuffer*  pConstantBufferCamera = nullptr;

    D3D11::Pass*            pBasicRenderpass = nullptr;

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
            // Shaders
            pVertexShader = new D3D11::VertexShader( "../DX/res/shaders/basicVS.hlsl" );
            pVertexShader->compile( "main" );
            pPixelShader = new D3D11::PixelShader( "../DX/res/shaders/basicPS.hlsl" );
            pPixelShader->compile("main" );
        }
        {
            auto blob = pVertexShader->getShaderBlob();
            HR( g_pDevice->CreateInputLayout(vertexDesc, _countof(vertexDesc), blob->GetBufferPointer(), blob->GetBufferSize(), &pInputLayout) );
        }

        {
            // Renderpasses
            pBasicRenderpass = new D3D11::Pass();
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
            rsDesc.CullMode = D3D11_CULL_BACK;
            rsDesc.FrontCounterClockwise = false;
            rsDesc.DepthClipEnable = true;
            rsDesc.MultisampleEnable = true;
            HR(g_pDevice->CreateRasterizerState(&rsDesc, &pRSState));
        }
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::shutdown()
    {
        SAFE_DELETE(pVertexShader);
        SAFE_DELETE(pPixelShader);
        SAFE_RELEASE(pDepthStencilState);
        SAFE_RELEASE(pInputLayout);
        SAFE_RELEASE(pRSState);
        SAFE_DELETE(pConstantBufferCamera);
        SAFE_DELETE(pConstantBufferObject);
        SAFE_DELETE(pBasicRenderpass);
        _DeinitD3D11();
    }

    static RenderTexture* s_currentRenderTarget = nullptr;

    //----------------------------------------------------------------------
    void D3D11Renderer::dispatch( const CommandBuffer& cmd )
    {
        // Depth Prepass first
        // Render in offscreen framebuffer and blit result to the swapchain

        // Set Pipeline States
        g_pImmediateContext->IASetInputLayout(pInputLayout);
        g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        pConstantBufferCamera->bind(0);
        pConstantBufferObject->bind(1);

        pVertexShader->bind();

        g_pImmediateContext->OMSetDepthStencilState(pDepthStencilState, 0);
        pPixelShader->bind();

        g_pImmediateContext->RSSetState(pRSState);

        for ( auto& command : cmd.getGPUCommands() )
        {
            switch ( command->getType() )
            {
                case GPUCommand::SET_RENDER_TARGET:
                {
                    GPUC_SetRenderTarget& c = *dynamic_cast<GPUC_SetRenderTarget*>( command.get() );
                    _SetRenderTarget( c.renderTarget );
                    break;
                }
                case GPUCommand::CLEAR_RENDER_TARGET:
                {
                    GPUC_ClearRenderTarget& c = *dynamic_cast<GPUC_ClearRenderTarget*>( command.get() );
                    _ClearRenderTarget( c.clearColor );
                    break;
                }
                case GPUCommand::SET_VIEWPORT:
                {
                    GPUC_SetViewport& c = *dynamic_cast<GPUC_SetViewport*>( command.get() );
                    _SetViewport( c.viewport );
                    break;
                }
                case GPUCommand::SET_CAMERA_PERSPECTIVE:
                {
                    GPUC_SetCameraPerspective& c = *dynamic_cast<GPUC_SetCameraPerspective*>( command.get() );
                    _SetCameraPerspective( c.view, c.fov, c.zNear, c.zFar );
                    break;
                }
                case GPUCommand::SET_CAMERA_ORTHO:
                {
                    GPUC_SetCameraOrtho& c = *dynamic_cast<GPUC_SetCameraOrtho*>( command.get() );
                    _SetCameraOrtho( c.view, c.left, c.right, c.bottom, c.top, c.zNear, c.zFar );
                    break;
                }
                case GPUCommand::DRAW_MESH:
                {
                    GPUC_DrawMesh& c = *dynamic_cast<GPUC_DrawMesh*>( command.get() );
                    _DrawMesh( c.modelMatrix, c.mesh, c.subMeshIndex );
                    break;
                }
                default:
                    WARN_RENDERING( "Unknown GPU Command in given command buffer!" );
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
        return new D3D11::D3D11Mesh();
    }

    //----------------------------------------------------------------------
    IMaterial* D3D11Renderer::createMaterial()
    {
        return new D3D11::D3D11Material();
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

    //----------------------------------------------------------------------
    void D3D11Renderer::_DrawMesh( const DirectX::XMMATRIX& modelMatrix, IMesh* mesh, I32 subMeshIndex )
    {
        // Bind vertex buffer
        mesh->bind( subMeshIndex );

        // Update constant per object buffer
        pConstantBufferObject->update( &modelMatrix, sizeof( DirectX::XMMATRIX ) );

        // Submit draw call
        g_pImmediateContext->DrawIndexed( mesh->getIndexCount( subMeshIndex ), 0, mesh->getBaseVertex( subMeshIndex ) );
    }

} // End namespaces