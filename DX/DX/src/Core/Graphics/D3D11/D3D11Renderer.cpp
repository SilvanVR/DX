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

#include <d3d11_4.h>
#include <comdef.h> /* _com_error */

namespace Core { namespace Graphics {

    //----------------------------------------------------------------------
    #define BACKBUFFER_FORMAT       DXGI_FORMAT_R8G8B8A8_UNORM
    #define NUM_BACKBUFFERS         1

    #define HR(x) \
    if ( FAILED( x ) ) { \
        _com_error err( x );\
        LPCTSTR errMsg = err.ErrorMessage();\
        ERROR_RENDERING( String("D3D11Renderer: @") + __FILE__ + ", line " + TS(__LINE__) + ". Reason: " + errMsg );\
    }

    //----------------------------------------------------------------------
    static ID3D11Device*            pDevice = nullptr;
    static ID3D11DeviceContext*     pImmediateContext = nullptr;
    static IDXGISwapChain1*         pSwapChain = nullptr;

    //**********************************************************************
    // INIT STUFF
    //**********************************************************************

    //----------------------------------------------------------------------
    D3D11Renderer::D3D11Renderer( OS::Window* window )
        : IRenderer( window )
    {
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::init()
    {
        _InitD3D11();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::shutdown()
    {
        _DeinitD3D11();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::render()
    {

    }

    //----------------------------------------------------------------------
    void D3D11Renderer::OnWindowSizeChanged( U16 w, U16 h )
    {
        LOG( "New Window-Size: " + TS(w) + "," + TS(h) );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::setVSync( bool enabled )
    {

    }

    //----------------------------------------------------------------------
    void D3D11Renderer::setMultiSampleCount( U32 numSamples )
    {
        if( not _CheckMSAASupport( numSamples ) )
            return;

        // Recreate Swapchain
        SAFE_RELEASE( pSwapChain );
        _CreateSwapchain();
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::_InitD3D11()
    {
        _CreateDeviceAndContext();
        if ( not _CheckMSAASupport( m_msaaCount ) )
            m_msaaCount = 1;
        _CreateSwapchain();

        LOG_RENDERING( "Done initializing D3D11..." );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_DeinitD3D11()
    {
        SAFE_RELEASE( pSwapChain );
        SAFE_RELEASE( pImmediateContext );
        SAFE_RELEASE( pDevice );
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
                               &pDevice, &featureLevel, &pImmediateContext ) );

        if ( featureLevel != featureLevels[0] )
            WARN_RENDERING( "D3D11Renderer: Latest feature level not supported. Fallback to a later version." );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_CreateSwapchain()
    {
        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width        = m_window->getSize().x;
        sd.Height       = m_window->getSize().y;
        sd.Format       = BACKBUFFER_FORMAT;
        sd.Stereo       = FALSE;
        sd.SampleDesc   = { m_msaaCount, m_msaaQualityLevel };
        sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount  = NUM_BACKBUFFERS;
        sd.Scaling      = DXGI_SCALING_STRETCH;
        sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
        sd.AlphaMode    = DXGI_ALPHA_MODE_IGNORE;

        IDXGIDevice2* pDXGIDevice;
        HR( pDevice->QueryInterface( __uuidof( IDXGIDevice2 ), (void **)&pDXGIDevice ) ) ;

        IDXGIAdapter* pDXGIAdapter = nullptr;
        HR( pDXGIDevice->GetAdapter( &pDXGIAdapter ) );

        IDXGIFactory2* pIDXGIFactory = nullptr;
        HR( pDXGIAdapter->GetParent( __uuidof( IDXGIFactory2 ), (void **)&pIDXGIFactory ) );

        HR( pIDXGIFactory->CreateSwapChainForHwnd( pDevice, m_window->getHWND(), &sd, NULL, NULL, &pSwapChain ) );
        SAFE_RELEASE( pDXGIDevice );
        SAFE_RELEASE( pDXGIAdapter );
        SAFE_RELEASE( pIDXGIFactory );
    }

    //----------------------------------------------------------------------
    bool D3D11Renderer::_CheckMSAASupport( U32 numSamples )
    {
        if (numSamples > D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT)
        {
            WARN_RENDERING( "D3D11Renderer::setMultiSampleCount(): #" + TS( numSamples ) + " samples are not supported." );
            return false;
        }

        UINT msaaQualityLevels;
        HR( pDevice->CheckMultisampleQualityLevels( BACKBUFFER_FORMAT, numSamples, &msaaQualityLevels ) );
        if (msaaQualityLevels == 0)
        {
            WARN_RENDERING( "D3D11Renderer::setMultiSampleCount(): #" + TS( numSamples ) + " samples are not supported "
                            "with the current backbuffer format." );
            return false;
        }

        m_msaaCount         = numSamples;
        m_msaaQualityLevel  = (msaaQualityLevels - 1);
        return true;
    }


} } // End namespaces