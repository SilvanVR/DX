#include "window.h"
/**********************************************************************
    class: Window (window.cpp)

    author: S. Hau
    date: November 3, 2017
**********************************************************************/


namespace Core { namespace OS {


    //----------------------------------------------------------------------
    Window::WindowCallbackHelper Window::m_callbackHelper;


    //----------------------------------------------------------------------
    Window::~Window()
    {
        if (m_created)
            destroy();
    }

    //**********************************************************************
    // WindowCallbackHelper
    //**********************************************************************

    //----------------------------------------------------------------------
    void Window::WindowCallbackHelper::callCursorCallback( I16 x, I16 y ) const 
    { 
        if (m_cursorMoveCallback) 
            m_cursorMoveCallback( x, y ); 
    }

    //----------------------------------------------------------------------
    void Window::WindowCallbackHelper::callMouseWheelCallback( I16 delta ) const 
    { 
        if (m_mouseWheelFunc) 
            m_mouseWheelFunc( delta );
    }

    //----------------------------------------------------------------------
    void Window::WindowCallbackHelper::callMouseButtonCallback( KeyCode code, KeyAction action, KeyMod mod ) const
    {
        if (m_mouseButtonFunc)
            m_mouseButtonFunc( code, action, mod );
    }

    //----------------------------------------------------------------------
    void Window::WindowCallbackHelper::callSizeChangedCallback(U16 width, U16 height) const
    {
        if (m_sizeChangedFunc)
            m_sizeChangedFunc( width, height );
    }

} } // end namespaces