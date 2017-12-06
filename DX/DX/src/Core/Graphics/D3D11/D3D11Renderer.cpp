#include "D3D11Renderer.h"
/**********************************************************************
    class: D3D11Renderer (D3D11Renderer.cpp)

    author: S. Hau
    date: November 28, 2017

    @Consideration:
     - If implementing multithreading remember to remove the
       D3D11_CREATE_DEVICE_SINGLETHREADED flag in the device creation.
**********************************************************************/

#include "locator.h"
#include "D3D11Shader.h"

namespace Core { namespace Graphics {

    using namespace DirectX;

    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT3 color;
    };

    Vertex vertices[] =
    {
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
        { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
        { XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
        { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
        { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
        { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
        { XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
    };

    UINT indices[36] = {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        4, 5, 1, 4, 1, 0,
        3, 2, 6, 3, 6, 7,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7
    };

    ID3D11Buffer* mVB;
    ID3D11Buffer* mIB;
    ID3D11Buffer* mCB;

    ID3D11RasterizerState* pRSState;

    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    ID3D11InputLayout* pInputLayout;
    ID3D11DepthStencilState* pDepthStencilState;

    D3D11::VertexShader*    pVertexShader = nullptr;
    D3D11::PixelShader*     pPixelShader = nullptr;

    //**********************************************************************
    // INIT STUFF
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::init()
    {
        _InitD3D11();

        {
            D3D11_BUFFER_DESC vbd = {};
            vbd.Usage = D3D11_USAGE_IMMUTABLE;
            vbd.ByteWidth = sizeof(Vertex) * _countof( vertices );
            vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA vinitData;
            vinitData.pSysMem = vertices;
            HR( g_pDevice->CreateBuffer( &vbd, &vinitData, &mVB) );
        }
        {
            D3D11_BUFFER_DESC ibd = {};
            ibd.Usage = D3D11_USAGE_IMMUTABLE;
            ibd.ByteWidth = sizeof(UINT) * _countof( indices );
            ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

            D3D11_SUBRESOURCE_DATA iinitData;
            iinitData.pSysMem = indices;
            HR( g_pDevice->CreateBuffer(&ibd, &iinitData, &mIB) );
        }
        {
            D3D11_BUFFER_DESC cbc = {};
            cbc.Usage = D3D11_USAGE_DEFAULT;
            cbc.ByteWidth = sizeof(XMMATRIX);
            cbc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            cbc.CPUAccessFlags = 0;

            HR( g_pDevice->CreateBuffer( &cbc, NULL, &mCB ) );
        }
        {
            // Shaders
            pVertexShader = new D3D11::VertexShader( "res/shaders/basicVS.hlsl" );
            pVertexShader->compile( "main" );
            pPixelShader = new D3D11::PixelShader( "res/shaders/basicPS.hlsl" );
            pPixelShader->compile("main" );
        }
        {
            auto blob = pVertexShader->getShaderBlob();
            HR( g_pDevice->CreateInputLayout(vertexDesc, _countof(vertexDesc), blob->GetBufferPointer(), blob->GetBufferSize(), &pInputLayout) );
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
        SAFE_RELEASE(mCB);
        SAFE_RELEASE(mVB);
        SAFE_RELEASE(mIB);
        _DeinitD3D11();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::render()
    {
        // Depth Prepass first
        // Render in offscreen framebuffer and blit result to the swapchain
        m_pSwapchain->clear( m_clearColor, 1.0f, 0 );

        auto renderTargetView = m_pSwapchain->getRenderTargetView();
        auto depthStencilView = m_pSwapchain->getDepthStencilView();
        g_pImmediateContext->OMSetRenderTargets( 1, &renderTargetView, depthStencilView);

        D3D11_VIEWPORT vp = {};
        vp.Width    = static_cast<float>( m_window->getSize().x );
        vp.Height   = static_cast<float>( m_window->getSize().y );
        vp.MaxDepth = 1.0f;

        // Set Pipeline States
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        g_pImmediateContext->IASetInputLayout(pInputLayout);
        g_pImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
        g_pImmediateContext->IASetIndexBuffer( mIB, DXGI_FORMAT_R32_UINT, 0 );
        g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        g_pImmediateContext->VSSetConstantBuffers(0, 1, &mCB );
        pVertexShader->bind();

        g_pImmediateContext->OMSetDepthStencilState(pDepthStencilState, 0);
        pPixelShader->bind();

        g_pImmediateContext->RSSetState(pRSState);
        g_pImmediateContext->RSSetViewports( 1, &vp );

        // Set constants
        static float angle = 0.0f;
        F32 delta = Locator::getProfiler().getUpdateDelta().value;
        angle += delta * 0.01f;
        XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);
        XMMATRIX world = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));

        static float xPos = 0.0f;
        static float zPos = 0.0f;
        zPos += AXIS_MAPPER.getAxisValue("Vertical") * delta * 0.01f;
        xPos += AXIS_MAPPER.getAxisValue("Horizontal") * delta * -0.01f;
        XMVECTOR eyePosition = XMVectorSet(xPos, 0, -10 + zPos, 1);
        XMMATRIX view = XMMatrixLookAtLH(eyePosition, { 0,0,0 }, {0,1,0});
        //XMMATRIX view = XMMatrixLookToLH(eyePosition, {0,0,1}, {0,1,0});
        XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), vp.Width / vp.Height, 0.1f, 100.0f);

        XMMATRIX worldViewProj = world*view*proj;
        g_pImmediateContext->UpdateSubresource( mCB, 0, nullptr, &worldViewProj, 0, 0 );

        g_pImmediateContext->DrawIndexed( _countof(indices), 0, 0 );


        m_pSwapchain->present( m_vsync );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::OnWindowSizeChanged( U16 w, U16 h )
    {
        // Window was minimized
        if ( w == 0 || h == 0 )
            return;

        m_pSwapchain->recreate( w, h );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::setMultiSampleCount( U32 numSamples )
    {
        if( not m_pSwapchain->numMSAASamplesSupported( numSamples ) )
            return;

        // Recreate Swapchain
        SAFE_DELETE( m_pSwapchain );
        _CreateSwapchain( numSamples );
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

        UINT createDeviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
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
        HR( g_pDevice->QueryInterface( __uuidof( ID3D11Debug ), reinterpret_cast<void**>(&pDebugDevice) ) );
        HR( pDebugDevice->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL ) );
        SAFE_RELEASE( pDebugDevice );
    }

} } // End namespaces