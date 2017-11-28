#include "i_renderer.h"
/**********************************************************************
    class: IRenderer (i_renderer.cpp)

    author: S. Hau
    date: November 28, 2017

**********************************************************************/

#include "Core/OS/Window/window.h"

namespace Core { namespace Graphics {


    //----------------------------------------------------------------------
    IRenderer::IRenderer(OS::Window* window) 
        : m_window( window )
    { 
        m_window->setCallbackSizeChanged( BIND_THIS_FUNC_2_ARGS( &IRenderer::OnWindowSizeChanged ) );
    }




} } // End namespaces