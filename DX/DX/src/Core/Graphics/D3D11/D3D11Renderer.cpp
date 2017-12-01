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

#undef ERROR
#include <d3d11_4.h>


namespace Core { namespace Graphics {

    //----------------------------------------------------------------------
    #define BACKBUFFER_FORMAT       DXGI_FORMAT_R8G8B8A8_UNORM
    #define INITIAL_MULTI_SAMPLES   4

    //----------------------------------------------------------------------
    static ID3D11Device*            pDevice = nullptr;
    static ID3D11DeviceContext*     pImmediateContext = nullptr;

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
        // Check if numSamples are supported
        UINT msaaQuality;
        HRESULT hr = pDevice->CheckMultisampleQualityLevels( BACKBUFFER_FORMAT, numSamples, &msaaQuality );
        if ( msaaQuality == 0 )
        {
            WARN_RENDERING( "D3D11Renderer::setMultiSampleCount(): #" + TS( numSamples ) + " samples are not supported." );
            return;
        }

        m_multiSampleCount = numSamples;

        // Recreate Swapchain
        // @TODO

    }


    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::_InitD3D11()
    {
        _CreateDeviceAndContext();
        setMultiSampleCount( INITIAL_MULTI_SAMPLES );
        _CreateSwapchain();


        LOG_RENDERING( "Done initializing D3D11..." );
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
        HRESULT hr = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
                                        featureLevels, _countof( featureLevels ), D3D11_SDK_VERSION,
                                        &pDevice, &featureLevel, &pImmediateContext );
        if ( FAILED( hr ) )
            ERROR_RENDERING( "D3D11Renderer: Faled to create a device and device-context." );

        if ( featureLevel != featureLevels[0] )
            WARN_RENDERING( "D3D11Renderer: Latest feature level not supported. Fallback to a later version." );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_CreateSwapchain()
    {
        OS::Point2D size = m_window->getSize();

        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = size.x;
        sd.BufferDesc.Height = size.y;
        sd.BufferDesc.Format = BACKBUFFER_FORMAT;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        //sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::_DeinitD3D11()
    {
        pDevice->Release();
        pImmediateContext->Release();
    }




} } // End namespaces