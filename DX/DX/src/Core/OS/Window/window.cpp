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
        if (m_mouseWheelCallback) 
            m_mouseWheelCallback( delta );
    }

    //----------------------------------------------------------------------
    void Window::WindowCallbackHelper::callMouseButtonCallback( MouseKey code, KeyAction action, KeyMod mod ) const
    {
        if (m_mouseButtonCallback)
            m_mouseButtonCallback( code, action, mod );
    }

    //----------------------------------------------------------------------
    void Window::WindowCallbackHelper::callSizeChangedCallback( U16 width, U16 height ) const
    {
        if (m_sizeChangedCallback)
            m_sizeChangedCallback( width, height );
    }

    //----------------------------------------------------------------------
    void Window::WindowCallbackHelper::callKeyCallback( Key key, KeyAction action, KeyMod mod ) const
    {
        if (m_keyCallback)
            m_keyCallback( key, action, mod );
    }

    //----------------------------------------------------------------------
    void Window::WindowCallbackHelper::callLooseFocusCallback() const
    {
        if (m_looseFocusCallback)
            m_looseFocusCallback();
    }

    //----------------------------------------------------------------------
    void Window::WindowCallbackHelper::callGainFocusCallback() const
    {
        if (m_gainFocusCallback)
            m_gainFocusCallback();
    }

    //----------------------------------------------------------------------
    void Window::WindowCallbackHelper::callCharCallback( char c ) const
    {
        if (m_charCallback)
            m_charCallback( c );
    }


} } // end namespaces