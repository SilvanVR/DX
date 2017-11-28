#include "i_renderer.h"
/**********************************************************************
    class: IRenderer (i_renderer.cpp)

    author: S. Hau
    date: November 28, 2017

**********************************************************************/

#include "Core/OS/Window/window.h"

namespace Core { namespace Graphics {

    static IRenderer* s_instance = nullptr;

    //----------------------------------------------------------------------
    void OnWindowSizeChangedHelper(U16 w, U16 h){ s_instance->OnWindowSizeChanged( w, h ); }

    //----------------------------------------------------------------------
    IRenderer::IRenderer(OS::Window* window) 
        : m_window( window )
    {
        s_instance = this;
        m_window->setCallbackSizeChanged( OnWindowSizeChangedHelper );
    }




} } // End namespaces