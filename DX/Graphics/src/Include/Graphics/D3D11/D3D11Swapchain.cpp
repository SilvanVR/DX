#include "D3D11Swapchain.h"
/**********************************************************************
    class: Swapchain (D3D11Swapchain.cpp)

    author: S. Hau
    date: December 2, 2017
**********************************************************************/

#include "D3D11Utility.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    #define FORMAT                  DXGI_FORMAT_R8G8B8A8_UNORM
    #define DEPTH_STENCIL_FORMAT    DXGI_FORMAT_D24_UNORM_S8_UINT
    #define NUM_BACKBUFFERS         1

    //----------------------------------------------------------------------
    Swapchain::Swapchain( HWND hwnd, U16 width, U16 height, U8 numMSAASamples )
        : m_msaaCount( numMSAASamples )
    {
        if ( not Utility::MSAASamplesSupported( FORMAT, m_msaaCount ) )
        {
            LOG_WARN_RENDERING( "D3D11: #" + TS(numMSAASamples) + " samples are not supported for this swapchain." );
            m_msaaCount = 1;
        }

        // Pick highest quality level
        UINT msaaQualityLevels;
        HR( g_pDevice->CheckMultisampleQualityLevels( FORMAT, m_msaaCount, &msaaQualityLevels ) );
        m_msaaQualityLevel = (msaaQualityLevels - 1);

        // Create backbuffers and depth-buffer 
        _CreateD3D11Swapchain( hwnd, width, height );
        _CreateRenderTargetView();
    }

    //----------------------------------------------------------------------
    Swapchain::~Swapchain()
    {
        SAFE_RELEASE( m_pRenderTargetView );
        SAFE_RELEASE( m_pSwapChain );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************    

    //----------------------------------------------------------------------
    void Swapchain::recreate( U16 width, U16 height )
    {
        SAFE_RELEASE( m_pRenderTargetView );
        HR( m_pSwapChain->ResizeBuffers( 1 + NUM_BACKBUFFERS, width, height, FORMAT, 0 ) );
        _CreateRenderTargetView();
    }

    //----------------------------------------------------------------------
    void Swapchain::bindForRendering()
    {
        g_pImmediateContext->OMSetRenderTargets( 1, &m_pRenderTargetView, NULL );
    }

    //----------------------------------------------------------------------
    void Swapchain::clear( Color color )
    {
        g_pImmediateContext->ClearRenderTargetView( m_pRenderTargetView, color.normalized().data() );
    }

    //----------------------------------------------------------------------
    void Swapchain::present( bool vsync )
    {
        HR( m_pSwapChain->Present( vsync ? 1 : 0, NULL ) );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Swapchain::_CreateD3D11Swapchain( HWND hwnd, U16 width, U16 height )
    {
        ASSERT( NUM_BACKBUFFERS < DXGI_MAX_SWAP_CHAIN_BUFFERS );

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width        = width;
        sd.Height       = height;
        sd.Format       = FORMAT;
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

} } // End namespaces