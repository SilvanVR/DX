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

namespace Core { namespace Graphics {

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
        _DeinitD3D11();
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::render()
    {
        auto renderTargetView = m_pSwapchain->getRenderTargetView();
        auto depthStencilView = m_pSwapchain->getDepthStencilView();
        //g_pImmediateContext->OMSetRenderTargets( 1, &renderTargetView, depthStencilView);

        D3D11_VIEWPORT vp;
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;
        vp.Width    = static_cast<float>( m_window->getSize().x );
        vp.Height   = static_cast<float>( m_window->getSize().y );
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        //g_pImmediateContext->RSSetViewports( 1, &vp );
        
        m_pSwapchain->clear( m_clearColor, 1.0f, 0 );

        // TODO: Render

        m_pSwapchain->present( m_vsync );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::OnWindowSizeChanged( U16 w, U16 h )
    {
        // Window was minimized
        if ( w == 0 || h == 0)
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
        _CreateSwapchain();
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::_InitD3D11()
    {
        _CreateDeviceAndContext();
        _CreateSwapchain();

        LOG_RENDERING( "Done initializing D3D11..." );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_DeinitD3D11()
    {
        SAFE_DELETE( m_pSwapchain );
        SAFE_RELEASE( g_pImmediateContext );
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
    void D3D11Renderer::_CreateSwapchain()
    {
        auto windowSize = m_window->getSize();
        m_pSwapchain = new D3D11::Swapchain( m_window->getHWND(), windowSize.x, windowSize.y, INITIAL_MSAA_SAMPLES );
    }


} } // End namespaces