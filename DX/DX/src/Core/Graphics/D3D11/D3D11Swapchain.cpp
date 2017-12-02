#include "D3D11Swapchain.h"
/**********************************************************************
    class: Swapchain (D3D11Swapchain.cpp)

    author: S. Hau
    date: December 2, 2017
**********************************************************************/

namespace Core { namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    #define BACKBUFFER_FORMAT       DXGI_FORMAT_R8G8B8A8_UNORM
    #define DEPTH_STENCIL_FORMAT    DXGI_FORMAT_D24_UNORM_S8_UINT
    #define NUM_BACKBUFFERS         1

    //----------------------------------------------------------------------
    Swapchain::Swapchain( HWND hwnd, U16 width, U16 height, U8 numMSAASamples )
        : m_msaaCount( numMSAASamples )
    {
        if ( not numMSAASamplesSupported( m_msaaCount ) )
            m_msaaCount = 1;

        // Pick highest quality level
        UINT msaaQualityLevels;
        HR( g_pDevice->CheckMultisampleQualityLevels( BACKBUFFER_FORMAT, m_msaaCount, &msaaQualityLevels ) );
        m_msaaQualityLevel = (msaaQualityLevels - 1);

        // Create backbuffers and depth-buffer 
        _CreateD3D11Swapchain( hwnd, width, height );
        _CreateRenderTargetView();
        _CreateDepthBuffer( width, height );
    }

    //----------------------------------------------------------------------
    Swapchain::~Swapchain()
    {
        SAFE_RELEASE( m_pDepthStencilBuffer );
        SAFE_RELEASE( m_pDepthStencilView );
        SAFE_RELEASE( m_pRenderTargetView );
        SAFE_RELEASE( m_pSwapChain );
    }

    //----------------------------------------------------------------------
    void Swapchain::recreate( U16 width, U16 height )
    {
        SAFE_RELEASE( m_pDepthStencilBuffer );
        SAFE_RELEASE( m_pDepthStencilView );
        SAFE_RELEASE( m_pRenderTargetView );
        HR( m_pSwapChain->ResizeBuffers( 1 + NUM_BACKBUFFERS, width, height, BACKBUFFER_FORMAT, 0 ) );
        _CreateRenderTargetView();
        _CreateDepthBuffer( width, height );
    }

    //----------------------------------------------------------------------
    void Swapchain::clear( Color color, F32 depth, U8 stencil )
    {
        g_pImmediateContext->ClearRenderTargetView( m_pRenderTargetView, color.normalized().data() );
        g_pImmediateContext->ClearDepthStencilView( m_pDepthStencilView, (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL), 1.0f, 0 );
    }

    //----------------------------------------------------------------------
    void Swapchain::present( bool vsync )
    {
        HR( m_pSwapChain->Present( vsync ? 1 : 0, NULL ) );
    }

    //----------------------------------------------------------------------
    void Swapchain::_CreateD3D11Swapchain( HWND hwnd, U16 width, U16 height )
    {
        ASSERT( ( 1 + NUM_BACKBUFFERS ) <= DXGI_MAX_SWAP_CHAIN_BUFFERS );

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width        = width;
        sd.Height       = height;
        sd.Format       = BACKBUFFER_FORMAT;
        sd.Stereo       = FALSE;
        sd.SampleDesc   = { m_msaaCount, m_msaaQualityLevel };
        sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount  = NUM_BACKBUFFERS;
        sd.Scaling      = DXGI_SCALING_STRETCH;
        sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
        sd.AlphaMode    = DXGI_ALPHA_MODE_IGNORE;
        sd.Flags        = 0;

        IDXGIDevice2* pDXGIDevice;
        HR( g_pDevice->QueryInterface( __uuidof( IDXGIDevice2 ), (void **)&pDXGIDevice ) ) ;

        IDXGIAdapter* pDXGIAdapter;
        HR( pDXGIDevice->GetAdapter( &pDXGIAdapter ) );

        IDXGIFactory2* pIDXGIFactory;
        HR( pDXGIAdapter->GetParent( __uuidof( IDXGIFactory2 ), (void **)&pIDXGIFactory ) );

        HR( pIDXGIFactory->CreateSwapChainForHwnd( g_pDevice, hwnd, &sd, NULL, NULL, &m_pSwapChain ) );
        SAFE_RELEASE( pDXGIDevice );
        SAFE_RELEASE( pDXGIAdapter );
        SAFE_RELEASE( pIDXGIFactory );
    }

    //----------------------------------------------------------------------
    void Swapchain::_CreateRenderTargetView()
    {
        ID3D11Texture2D* backBuffer;
        HR( m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBuffer ) ) );
        HR( g_pDevice->CreateRenderTargetView( backBuffer, NULL, &m_pRenderTargetView ) );
        SAFE_RELEASE( backBuffer );
    }

    //----------------------------------------------------------------------
    void Swapchain::_CreateDepthBuffer( U16 width, U16 height )
    {
        D3D11_TEXTURE2D_DESC depthStencilDesc = {};
        depthStencilDesc.Width          = width;
        depthStencilDesc.Height         = height;
        depthStencilDesc.MipLevels      = 1;
        depthStencilDesc.ArraySize      = 1;
        depthStencilDesc.Format         = DEPTH_STENCIL_FORMAT;
        depthStencilDesc.SampleDesc     = { m_msaaCount, m_msaaQualityLevel };
        depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags      = 0;

        HR( g_pDevice->CreateTexture2D( &depthStencilDesc, NULL, &m_pDepthStencilBuffer) );
        HR( g_pDevice->CreateDepthStencilView( m_pDepthStencilBuffer, NULL, &m_pDepthStencilView ) );
    }

    //----------------------------------------------------------------------
    bool Swapchain::numMSAASamplesSupported( U8 numSamples )
    {
        if (numSamples > D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT)
        {
            WARN_RENDERING( "D3D11: #" + TS( numSamples ) + " samples are too high. "
                            "Max is: " + TS( D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT ) );
            return false;
        }

        UINT msaaQualityLevels;
        HR( g_pDevice->CheckMultisampleQualityLevels( BACKBUFFER_FORMAT, numSamples, &msaaQualityLevels ) );
        if (msaaQualityLevels == 0)
        {
            WARN_RENDERING( "D3D11: #" + TS( numSamples ) + " samples are not supported "
                            "with the current swapchain format." );
            return false;
        }

        return true;
    }

} } } // End namespaces