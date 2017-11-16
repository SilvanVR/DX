#include "input_manager.h"
/**********************************************************************
    class: InputManager (input_manager.cpp)

    author: S. Hau
    date: November 4, 2017

    @Considerations:
     - Theorethically it can happen that a button pressed/release
       will be missed, because the tick-rate occurs in a slower rate
       than the os-loop TODO
**********************************************************************/

#include "locator.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    static InputManager* s_instance = nullptr;

    //----------------------------------------------------------------------
    void KeyCallback( Key key, KeyAction action, KeyMod mod )        { s_instance->_KeyCallback( key, action, mod ); }
    void CharCallback( char c )                                      { s_instance->_CharCallback( c ); }
    void MouseCallback( MouseKey key, KeyAction action, KeyMod mod ) { s_instance->_MouseCallback( key, action, mod ); }
    void MouseWheelCallback( I16 param )                             { s_instance->_MouseWheelCallback( param ); }
    void CursorMovedCallback( I16 x, I16 y )                         { s_instance->_CursorMovedCallback( x, y ); }

    //----------------------------------------------------------------------
    void InputManager::init()
    {
        ASSERT( s_instance == nullptr );
        s_instance = this;

        // Subscribe to OnTick() event
        Locator::getCoreEngine().subscribe( this );

        // Subscribe to all window events
        OS::Window& window = Locator::getWindow();
        window.setCallbackKey( KeyCallback );
        window.setCallbackChar( CharCallback );
        window.setCallbackMouseButtons( MouseCallback );
        window.setCallbackMouseWheel( MouseWheelCallback );
        window.setCallbackCursorMove( CursorMovedCallback );

        // Zero out arrays
        memset( m_keyPressed, 0, MAX_KEYS * sizeof( bool ) );
        memset( m_keyReleased, 0, MAX_KEYS * sizeof( bool ) );
        memset( m_keyPressedThisTick, 0, MAX_KEYS * sizeof( bool ) );
        memset( m_keyPressedLastTick, 0, MAX_KEYS * sizeof( bool ) );
    }

    //----------------------------------------------------------------------
    void InputManager::OnTick( Time::Seconds delta )
    {
        memcpy( m_keyPressedLastTick, m_keyPressedThisTick, MAX_KEYS * sizeof( bool ) );

        // Update the "keyPressedThisTick" array.
        // This loop decouples the slower tick-rate from the window-callback rate.
        // Because the window-callbacks will be called every frame it can theoretically
        // happen that a key is pressed and released at the same time in a tick.
        // With the "else if" part, no key events will ever be missed.
        for ( I32 i = 0; i < MAX_KEYS; i++ )
        {
            if ( m_keyPressed[i] )
            {
                m_keyPressedThisTick[i] = true;
                m_keyPressed[i] = false;
            }
            else if ( m_keyReleased[i] )
            {
                m_keyPressedThisTick[i] = false;
                m_keyReleased[i] = false;
            }
        }

        // Notify subscribers
        for (I32 i = 0; i < MAX_KEYS; i++)
        {
            if ( wasKeyPressed( (Key)i ) )
            {
                _NotifyKeyPressed( (Key)i );
            }

            if ( wasKeyReleased( (Key)i ) )
            {
                _NotifyKeyReleased( (Key)i );
            }
        }

        // ---- MOUSE ----
        memcpy( m_mouseKeyPressedLastTick, m_mouseKeyPressedThisTick, MAX_MOUSE_KEYS * sizeof( bool ) );
        for (I32 i = 0; i < MAX_MOUSE_KEYS; i++)
        {
            if (m_mouseKeyPressed[i])
            {
                m_mouseKeyPressedThisTick[i] = true;
                m_mouseKeyPressed[i] = false;
            }
            else if (m_mouseKeyReleased[i])
            {
                m_mouseKeyPressedThisTick[i] = false;
                m_mouseKeyReleased[i] = false;
            }
        }

        for (I32 i = 0; i < MAX_MOUSE_KEYS; i++)
        {
            if ( wasMouseKeyPressed( (MouseKey)i ) )
            {
                _NotifyMouseKeyPressed( (MouseKey)i );
            }

            if ( wasMouseKeyReleased( (MouseKey)i ) )
            {
                _NotifyMouseKeyReleased( (MouseKey)i );
            }
        }

        if (m_mouseMoved)
        {
            m_mouseMoved = false;
            _NotifyMouseMoved();
        }

        if (m_scrolledWheel)
        {
            m_scrolledWheel = false;
        }
        else
        {
            m_wheelDelta = 0;
        }
    }

    //----------------------------------------------------------------------
    void InputManager::shutdown()
    {
        // Unsubscribe to all events (just for safety)
        OS::Window& window = Locator::getWindow();
        window.setCallbackKey( nullptr );
        window.setCallbackChar( nullptr );
        window.setCallbackMouseButtons( nullptr );
        window.setCallbackMouseWheel( nullptr );
        window.setCallbackCursorMove( nullptr );
    }

    //----------------------------------------------------------------------
    bool InputManager::isKeyDown( Key key ) const
    {
        return m_keyPressedThisTick[(I32)key];
    }

    //----------------------------------------------------------------------
    bool InputManager::wasKeyPressed( Key key ) const
    {
        I32 keyAsNum = (I32)key;
        return m_keyPressedThisTick[ keyAsNum ] && not m_keyPressedLastTick[ keyAsNum ];
    }

    //----------------------------------------------------------------------
    bool InputManager::wasKeyReleased( Key key ) const
    {
        I32 keyAsNum = (I32)key;
        return not m_keyPressedThisTick[ keyAsNum ] && m_keyPressedLastTick[ keyAsNum ];
    }

    //----------------------------------------------------------------------
    bool InputManager::isMouseKeyDown(MouseKey key) const
    {
        return m_mouseKeyPressedThisTick[ (I32)key ];
    }

    //----------------------------------------------------------------------
    bool InputManager::wasMouseKeyPressed( MouseKey key ) const
    {
        I32 keyAsNum = (I32)key;
        return m_mouseKeyPressedThisTick[ keyAsNum ] && not m_mouseKeyPressedLastTick[ keyAsNum ];
    }

    //----------------------------------------------------------------------
    bool InputManager::wasMouseKeyReleased( MouseKey key ) const
    {
        I32 keyAsNum = (I32)key;
        return not m_mouseKeyPressedThisTick[ keyAsNum ] && m_mouseKeyPressedLastTick[ keyAsNum ];
    }

    //----------------------------------------------------------------------
    void InputManager::_MouseCallback( MouseKey key, KeyAction action, KeyMod mod )
    {
        switch (action)
        {
        case KeyAction::DOWN:
            m_mouseKeyPressed[ (I32)key ] = true;
            break;
        case KeyAction::UP:
            m_mouseKeyReleased[ (I32)key ] = true;
            break;
        }
    }

    //----------------------------------------------------------------------
    void InputManager::_CursorMovedCallback( I16 x, I16 y )
    {
        m_cursorX = x;
        m_cursorY = y;
        m_mouseMoved = true;
    }

    //----------------------------------------------------------------------
    void InputManager::_MouseWheelCallback( I16 delta )
    {
        //LOG( "Mouse Wheel: " + TS(param), Color::RED );
        m_wheelDelta = delta;
        m_scrolledWheel = true;
    }

    //----------------------------------------------------------------------
    void InputManager::_KeyCallback( Key key, KeyAction action, KeyMod mod )
    {
        ASSERT( (I32)key < MAX_KEYS );

        switch (action)
        {
        case KeyAction::DOWN:
            m_keyPressed[ (I32)key ] = true;
            break;
        case KeyAction::UP:
            m_keyReleased[ (I32)key ] = true;
            break;
        }
    }

    //----------------------------------------------------------------------
    void InputManager::_CharCallback( char c )
    {
        //static String buffer;
        //if (c == '\b')
        //    buffer = buffer.substr(0, buffer.size() - 1);
        //else
        //    buffer += c;

        //if (buffer.size() > 0)
        //    LOG(buffer);
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyKeyPressed( Key key ) const
    {
        for (auto& listener : m_keyListener)
            listener->OnKeyPressed( key );
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyKeyReleased( Key key ) const
    {
        for (auto& listener : m_keyListener)
            listener->OnKeyReleased( key );
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyMouseMoved() const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMouseMoved( m_cursorX, m_cursorY );
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyMouseKeyPressed( MouseKey key ) const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMousePressed( key );
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyMouseKeyReleased( MouseKey key ) const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMouseReleased( key );
    }

} }
