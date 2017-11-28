#include "mouse.h"
/**********************************************************************
    class: Mouse (mouse.cpp)

    author: S. Hau
    date: November 18, 2017
**********************************************************************/

#include "locator.h"
#include "../listener/input_listener.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    #define LISTENER_START_CAPACITY 4

    //----------------------------------------------------------------------
    static Mouse* s_mouse = nullptr;

    //----------------------------------------------------------------------
    void MouseCallback( MouseKey key, KeyAction action, KeyMod mod ) { s_mouse->_MouseCallback( key, action, mod ); }
    void MouseWheelCallback( I16 param )                             { s_mouse->_MouseWheelCallback( param ); }
    void CursorMovedCallback( I16 x, I16 y )                         { s_mouse->_CursorMovedCallback( x, y ); }

    //----------------------------------------------------------------------
    Mouse::Mouse( OS::Window* window )
        : m_window( window )
    {
        ASSERT( s_mouse == nullptr );
        s_mouse = this;

        // Zero out arrays
        memset( m_mouseKeyPressed, 0, MAX_MOUSE_KEYS * sizeof( bool ) );
        memset( m_mouseKeyReleased, 0, MAX_MOUSE_KEYS * sizeof( bool ) );
        memset( m_mouseKeyPressedThisTick, 0, MAX_MOUSE_KEYS * sizeof( bool ) );
        memset( m_mouseKeyPressedLastTick, 0, MAX_MOUSE_KEYS * sizeof( bool ) );

        // Subscribe to window callbacks
        m_window->setCallbackMouseButtons( MouseCallback );
        m_window->setCallbackMouseWheel( MouseWheelCallback );
        m_window->setCallbackCursorMove( CursorMovedCallback );

        // Preallocate mem for listener
        m_mouseListener.reserve( LISTENER_START_CAPACITY );
    }

    //----------------------------------------------------------------------
    Mouse::~Mouse()
    {
        // Unregister from window events
        m_window->setCallbackMouseButtons( nullptr );
        m_window->setCallbackMouseWheel( nullptr );
        m_window->setCallbackCursorMove( nullptr );
    }

    //----------------------------------------------------------------------
    void Mouse::_UpdateInternalState()
    {
        _UpdateMouseKeyStates();
        _UpdateCursorDelta();
    }

    //----------------------------------------------------------------------
    void Mouse::centerCursor() const
    {
        m_window->centerCursor();
    }

    //----------------------------------------------------------------------
    void Mouse::showCursor(bool b) const
    {
        m_window->showCursor( b );
    }

    //----------------------------------------------------------------------
    bool Mouse::isKeyDown( MouseKey key ) const
    {
        return m_mouseKeyPressedThisTick[ (I32)key ];
    }

    //----------------------------------------------------------------------
    bool Mouse::wasKeyPressed( MouseKey key ) const
    {
        I32 keyIndex = (I32)key;
        return ( m_mouseKeyPressedThisTick[ keyIndex ] && not m_mouseKeyPressedLastTick[ keyIndex ] );
    }

    //----------------------------------------------------------------------
    bool Mouse::wasKeyReleased( MouseKey key ) const
    {
        I32 keyIndex = (I32)key;
        return ( not m_mouseKeyPressedThisTick[ keyIndex ] && m_mouseKeyPressedLastTick[ keyIndex ] );
    }

    //----------------------------------------------------------------------
    void Mouse::setFirstPersonMode( bool enabled )
    {
        if ( m_firstPersonMode == enabled )
            return; // Is already enabled or disabled

        m_firstPersonMode = enabled;
        showCursor( not m_firstPersonMode );

        if (m_firstPersonMode)
        {
            centerCursor();
            m_cursorLastTick = m_window->getCursorPosition();
        }
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Mouse::_MouseCallback( MouseKey key, KeyAction action, KeyMod mod )
    {
        switch (action)
        {
        case KeyAction::DOWN:
            m_mouseKeyPressed[ (I32)key ] = true;
            _NotifyMouseKeyPressed( key, mod );
            break;
        case KeyAction::UP:
            m_mouseKeyReleased[ (I32)key ] = true;
            _NotifyMouseKeyReleased( key, mod );
            break;
        }
    }

    //----------------------------------------------------------------------
    void Mouse::_CursorMovedCallback( I16 x, I16 y )
    {
        m_cursor.x = x;
        m_cursor.y = y;
        _NotifyMouseMoved( m_cursor.x, m_cursor.y );
    }

    //----------------------------------------------------------------------
    void Mouse::_MouseWheelCallback( I16 delta )
    {
        m_wheelDelta = delta;
        _NotifyMouseWheel( m_wheelDelta );
    }

    //**********************************************************************
    void Mouse::_UpdateMouseKeyStates()
    {
        // Save last state
        memcpy( m_mouseKeyPressedLastTick, m_mouseKeyPressedThisTick, MAX_MOUSE_KEYS * sizeof( bool ) );

        // Same mechanism as with the keyboard. Used to decouple slower tick rate from faster update rate.
        for (I32 i = 0; i < MAX_MOUSE_KEYS; i++)
        {
            if ( m_mouseKeyPressed[i] )
            {
                m_mouseKeyPressedThisTick[i] = true;
                m_mouseKeyPressed[i] = false;
            }
            else if ( m_mouseKeyReleased[i] )
            {
                m_mouseKeyPressedThisTick[i] = false;
                m_mouseKeyReleased[i] = false;
            }
        }

        // Reset mouse wheel delta. This ensures that the delta is saved for one tick.
        static bool usedMouseWheel = false;
        if (usedMouseWheel)
            m_wheelDelta = 0;
        usedMouseWheel = (m_wheelDelta != 0);
    }

    //----------------------------------------------------------------------
    void Mouse::_UpdateCursorDelta()
    {
        if (m_firstPersonMode)
        {
            centerCursor();
            // m_cursorLastTick is always fixed (center of screen)
        }
        else
        {
            m_cursorLastTick = m_cursorThisTick;
        }

        m_cursorThisTick = m_cursor;
        m_cursorDelta = (m_cursorThisTick - m_cursorLastTick);
    }

    //**********************************************************************
    void Mouse::_NotifyMouseMoved( I16 x, I16 y ) const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMouseMoved( x, y );
    }

    //----------------------------------------------------------------------
    void Mouse::_NotifyMouseKeyPressed( MouseKey key, KeyMod mod ) const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMousePressed( key, mod);
    }

    //----------------------------------------------------------------------
    void Mouse::_NotifyMouseKeyReleased( MouseKey key, KeyMod mod ) const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMouseReleased( key, mod );
    }

    //----------------------------------------------------------------------
    void Mouse::_NotifyMouseWheel( I16 delta ) const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMouseWheel( delta );
    }



} } // end namespaces