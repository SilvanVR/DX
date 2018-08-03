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
    #define NUM_BACKBUFFERS         1

    //----------------------------------------------------------------------
    Swapchain::Swapchain( HWND hwnd, U16 width, U16 height, U8 numMSAASamples )
    {
        if ( not Utility::MSAASamplesSupported( FORMAT, numMSAASamples) )
        {
            LOG_WARN_RENDERING( "D3D11: #" + TS(numMSAASamples) + " samples are not supported for this swapchain." );
            numMSAASamples = 1;
        }

        // Pick highest quality level
        UINT msaaQualityLevels;
        HR( g_pDevice->CheckMultisampleQualityLevels( FORMAT, numMSAASamples, &msaaQualityLevels ) );
        UINT highestQualityLevel = (msaaQualityLevels - 1);

        // Create backbuffers and depth-buffer 
        _CreateD3D11Swapchain( hwnd, width, height, numMSAASamples, highestQualityLevel );
        _CreateRenderTargetView();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************    

    //----------------------------------------------------------------------
    void Swapchain::recreate( U16 width, U16 height )
    {
        m_pRenderTargetView.release();
        HR( m_pSwapChain->ResizeBuffers( 1 + NUM_BACKBUFFERS, width, height, FORMAT, 0 ) );
        _CreateRenderTargetView();
    }

    //----------------------------------------------------------------------
    void Swapchain::bindForRendering()
    {
        g_pImmediateContext->OMSetRenderTargets( 1, &m_pRenderTargetView.get(), NULL );
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
    void Swapchain::_CreateD3D11Swapchain( HWND hwnd, U16 width, U16 height, U8 msaaCount, U8 msaaQualityLevel )
    {
        ASSERT( NUM_BACKBUFFERS < DXGI_MAX_SWAP_CHAIN_BUFFERS );

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width        = width;
        sd.Height       = height;
        sd.Format       = FORMAT;
        sd.Stereo       = FALSE;
        sd.SampleDesc   = { msaaCount, msaaQualityLevel };
        sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount  = NUM_BACKBUFFERS;
        sd.Scaling      = DXGI_SCALING_STRETCH;
        sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
        sd.AlphaMode    = DXGI_ALPHA_MODE_IGNORE;
        sd.Flags        = 0;

        ComPtr<IDXGIDevice2> pDXGIDevice;
        HR( g_pDevice->QueryInterface( __uuidof( IDXGIDevice2 ), (void **)&pDXGIDevice.get() ) ) ;

        ComPtr<IDXGIAdapter> pDXGIAdapter;
        HR( pDXGIDevice->GetAdapter( &pDXGIAdapter.get() ) );

        ComPtr<IDXGIFactory2> pIDXGIFactory;
        HR( pDXGIAdapter->GetParent( __uuidof( IDXGIFactory2 ), (void **)&pIDXGIFactory.get()) );

        HR( pIDXGIFactory->CreateSwapChainForHwnd( g_pDevice, hwnd, &sd, NULL, NULL, &m_pSwapChain.releaseAndGet() ) );
    }

    //----------------------------------------------------------------------
    void Swapchain::_CreateRenderTargetView()
    {
        ID3D11Texture2D* backBuffer;
        HR( m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBuffer ) ) );
        HR( g_pDevice->CreateRenderTargetView( backBuffer, NULL, &m_pRenderTargetView.releaseAndGet() ) );
        SAFE_RELEASE( backBuffer );
    }

} } // End namespaces