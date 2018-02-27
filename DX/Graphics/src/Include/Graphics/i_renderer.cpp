#include "i_renderer.h"
/**********************************************************************
    class: IRenderer (i_renderer.cpp)

    author: S. Hau
    date: November 28, 2017

**********************************************************************/



namespace Core { namespace Graphics {


    //----------------------------------------------------------------------
    IRenderer::IRenderer(OS::Window* window) 
        : m_window( window )
    { 
        ASSERT( window != nullptr );
        m_window->setCallbackSizeChanged( BIND_THIS_FUNC_2_ARGS( &IRenderer::OnWindowSizeChanged ) );
    }




} } // End namespaces