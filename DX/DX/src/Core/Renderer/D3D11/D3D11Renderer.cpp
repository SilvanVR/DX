#include "D3D11Renderer.h"
/**********************************************************************
    class: D3D11Renderer (D3D11Renderer.cpp)

    author: S. Hau
    date: November 28, 2017
**********************************************************************/

#include "locator.h"

namespace Core { namespace Graphics {

    D3D11Renderer::D3D11Renderer(OS::Window* window)
        : IRenderer( window )
    {

    }

    //----------------------------------------------------------------------
    void D3D11Renderer::init()
    {
        OS::Point2D size = m_window->getSize();

        LOG( TS(size.x) + " " + TS(size.y) );
    }

    //----------------------------------------------------------------------
    void D3D11Renderer::shutdown()
    {

    }

    //----------------------------------------------------------------------
    void D3D11Renderer::render()
    {

    }


} } // End namespaces