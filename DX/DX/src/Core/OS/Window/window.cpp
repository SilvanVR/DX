#include "window.h"
/**********************************************************************
    class: Window (window.cpp)

    author: S. Hau
    date: November 3, 2017
**********************************************************************/


namespace Core { namespace OS {

    //----------------------------------------------------------------------
    Window* Window::s_instance = nullptr;

    //----------------------------------------------------------------------
    Window::Window(const char* title, U32 width, U32 height)
       : m_width( width ), m_height( height ), m_title( title )
    {
        ASSERT( s_instance == nullptr );
        s_instance = this;
    }

    //----------------------------------------------------------------------
    Window::~Window()
    {
        if (m_created)
            destroy();
    }


} } // end namespaces