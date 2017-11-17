#include "input_manager.h"
/**********************************************************************
    class: InputManager (input_manager.cpp)

    author: S. Hau
    date: November 4, 2017

    @Considerations:
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
        memset( m_mouseKeyPressed, 0, MAX_MOUSE_KEYS * sizeof( bool ) );
        memset( m_mouseKeyReleased, 0, MAX_MOUSE_KEYS * sizeof( bool ) );
        memset( m_mouseKeyPressedThisTick, 0, MAX_MOUSE_KEYS * sizeof( bool ) );
        memset( m_mouseKeyPressedLastTick, 0, MAX_MOUSE_KEYS * sizeof( bool ) );

        // Preallocate mem
        m_keyListener.reserve( 4 );
        m_mouseListener.reserve( 4 );
    }

    //----------------------------------------------------------------------
    void InputManager::OnTick( Time::Seconds delta )
    {
        _UpdateKeyStates();
        _UpdateMouseStates();
        _UpdateCursorDelta();
        _UpdateAxes( delta );
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
        return m_keyPressedThisTick[ (I32)key ];
    }

    //----------------------------------------------------------------------
    bool InputManager::wasKeyPressed( Key key ) const
    {
        I32 keyIndex = (I32)key;
        return m_keyPressedThisTick[ keyIndex ] && not m_keyPressedLastTick[ keyIndex ];
    }

    //----------------------------------------------------------------------
    bool InputManager::wasKeyReleased( Key key ) const
    {
        I32 keyIndex = (I32)key;
        return not m_keyPressedThisTick[ keyIndex ] && m_keyPressedLastTick[ keyIndex ];
    }

    //----------------------------------------------------------------------
    bool InputManager::isMouseKeyDown( MouseKey key ) const
    {
        return m_mouseKeyPressedThisTick[ (I32)key ];
    }

    //----------------------------------------------------------------------
    bool InputManager::wasMouseKeyPressed( MouseKey key ) const
    {
        I32 keyIndex = (I32)key;
        return m_mouseKeyPressedThisTick[ keyIndex ] && not m_mouseKeyPressedLastTick[ keyIndex ];
    }

    //----------------------------------------------------------------------
    bool InputManager::wasMouseKeyReleased( MouseKey key ) const
    {
        I32 keyIndex = (I32)key;
        return not m_mouseKeyPressedThisTick[ keyIndex ] && m_mouseKeyPressedLastTick[ keyIndex ];
    }

    //----------------------------------------------------------------------
    void InputManager::setFirstPersonMode( bool enabled )
    {
        if ( ( enabled && m_firstPersonMode ) || ( not enabled && not m_firstPersonMode ) )
            return; // Was already enabled or disabled

        m_firstPersonMode = enabled;
        OS::Window& window = Locator::getWindow();
        window.showCursor( not m_firstPersonMode );

        if (m_firstPersonMode)
        {
            window.centerCursor();
            window.getCursorPosition( &m_cursorLastTick.x, &m_cursorLastTick.y );
        }
    }

    //----------------------------------------------------------------------
    F32 InputManager::getAxis(const char* name) const
    {
        StringID axis = SID( name );
        if ( m_axisMap.count( axis ) == 0 )
        {
            WARN( "InputManager::getAxis(): Axis name '" + axis.toString() + "' does not exist." );
            return 0.0f;
        }
        return (F32) m_axisMap.at( axis );
    }

    //----------------------------------------------------------------------
    void InputManager::registerAxis(const char* name, Key key0, Key key1, F64 acc)
    {
        StringID axis = SID( name );
        if ( m_axisMap.count( axis ) != 0 )
        {
            WARN( "InputManager::registerAxis(): Axis '" + axis.toString() + "' already exists. Consider a different name." );
            return;
        }

        m_axisInfos.push_back( { axis, key0, key1, acc } );
        m_axisMap[ axis ] = 0.0;
    }

    //----------------------------------------------------------------------
    void InputManager::unregisterAxis( const char* name )
    {
        StringID axis = SID( name );
        for ( auto it = m_axisInfos.begin(); it != m_axisInfos.end(); it++ )
        {
            if ( axis == it->name )
            {
                m_axisInfos.erase( it );
                break;
            }
        }
        m_axisMap.erase( axis );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************
    void InputManager::_UpdateKeyStates()
    {
        // Save last state
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
    }

    //----------------------------------------------------------------------
    void InputManager::_UpdateMouseStates()
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
    }

    //----------------------------------------------------------------------
    void InputManager::_UpdateCursorDelta()
    {
        if (m_firstPersonMode)
        {
            Locator::getWindow().centerCursor();
            // m_cursorLastTick is always fixed (center of screen)
        }
        else
        {
            m_cursorLastTick = m_cursorThisTick;
        }

        m_cursorThisTick = m_cursor;
        m_cursorDelta = (m_cursorThisTick - m_cursorLastTick);
    }

    //----------------------------------------------------------------------
    void InputManager::_UpdateAxes( Time::Seconds delta )
    {
        for (auto& axis : m_axisInfos)
        {
            bool key0Down   = isKeyDown( axis.key0 );
            bool key1Down   = isKeyDown( axis.key1 );
            F64 step        = (axis.acc * delta.value);

            F64& val = m_axisMap[ axis.name ];
            if (key0Down)
            {
                val += step;
                if (val > AXIS_MAX) val = AXIS_MAX;
            }

            if (key1Down)
            {
                val -= step;
                if (val < AXIS_MIN) val = AXIS_MIN;
            }

            if ( not key0Down && not key1Down )
            {
                if (val > step)
                    val -= step;
                else if (val < -step)
                    val += step;
                else
                    val = 0;
            }
        }
    }

    //----------------------------------------------------------------------
    void InputManager::_MouseCallback( MouseKey key, KeyAction action, KeyMod mod )
    {
        switch (action)
        {
        case KeyAction::DOWN:
            m_mouseKeyPressed[ (I32)key ] = true;
            _NotifyMouseKeyPressed( key, mod);
            break;
        case KeyAction::UP:
            m_mouseKeyReleased[ (I32)key ] = true;
            _NotifyMouseKeyReleased( key, mod);
            break;
        }
    }

    //----------------------------------------------------------------------
    void InputManager::_CursorMovedCallback( I16 x, I16 y )
    {
        m_cursor.x = x;
        m_cursor.y = y;
        _NotifyMouseMoved( m_cursor.x, m_cursor.y );
    }

    //----------------------------------------------------------------------
    void InputManager::_MouseWheelCallback( I16 delta )
    {
        m_wheelDelta = delta;
        _NotifyMouseWheel( m_wheelDelta );
    }

    //----------------------------------------------------------------------
    void InputManager::_KeyCallback( Key key, KeyAction action, KeyMod mod )
    {
        I32 keyIndex = (I32) key;
        ASSERT( keyIndex < MAX_KEYS );

        switch (action)
        {
        case KeyAction::DOWN:
            m_keyPressed[ keyIndex ] = true;
            _NotifyKeyPressed( key, mod );
            break;
        case KeyAction::UP:
            m_keyReleased[ keyIndex ] = true;
            _NotifyKeyReleased( key, mod);
            break;
        }
    }

    //----------------------------------------------------------------------
    void InputManager::_CharCallback( char c )
    {
        _NotifyOnChar( c );
    }

    //**********************************************************************

    //----------------------------------------------------------------------
    void InputManager::_NotifyKeyPressed( Key key, KeyMod mod ) const
    {
        for (auto& listener : m_keyListener)
            listener->OnKeyPressed( key, mod);
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyKeyReleased( Key key, KeyMod mod ) const
    {
        for (auto& listener : m_keyListener)
            listener->OnKeyReleased( key, mod);
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyOnChar(char c) const
    {
        for (auto& listener : m_keyListener)
            listener->OnChar( c );
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyMouseMoved( I16 x, I16 y ) const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMouseMoved( x, y );
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyMouseKeyPressed( MouseKey key, KeyMod mod ) const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMousePressed( key, mod);
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyMouseKeyReleased( MouseKey key, KeyMod mod ) const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMouseReleased( key, mod );
    }

    //----------------------------------------------------------------------
    void InputManager::_NotifyMouseWheel( I16 delta ) const
    {
        for (auto& listener : m_mouseListener)
            listener->OnMouseWheel( delta );
    }

} }
