#include "D3D11Renderer.h"
/**********************************************************************
    class: D3D11Renderer (D3D11Renderer.cpp)

    author: S. Hau
    date: November 28, 2017
**********************************************************************/

#include "locator.h"

#undef ERROR
#include <d3d11.h>

namespace Core { namespace Graphics {

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
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Renderer::_InitD3D11()
    {
        OS::Point2D size = m_window->getSize();

        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = size.x;
        sd.BufferDesc.Height = size.y;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        //sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;

        HRESULT hr = S_OK;
        D3D_FEATURE_LEVEL featureLevel;
    }


} } // End namespaces