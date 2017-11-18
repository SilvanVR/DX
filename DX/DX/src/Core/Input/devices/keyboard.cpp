#include "keyboard.h"
/**********************************************************************
    class: Keyboard (keyboard.cpp)

    author: S. Hau
    date: November 18, 2017
**********************************************************************/

#include "locator.h"
#include "../listener/input_listener.h"

namespace Core { namespace Input {

        //----------------------------------------------------------------------
        #define LISTENER_START_CAPACITY 4

        //----------------------------------------------------------------------
        static Keyboard* s_keyboard = nullptr;

        //----------------------------------------------------------------------
        void KeyCallback( Key key, KeyAction action, KeyMod mod )   { s_keyboard->_KeyCallback( key, action, mod ); }
        void CharCallback( char c )                                 { s_keyboard->_CharCallback( c ); }

        //----------------------------------------------------------------------
        Keyboard::Keyboard( OS::Window* window )
            : m_window( window )
        {
            ASSERT(s_keyboard == nullptr);
            s_keyboard = this;

            // Subscribe to all window events
            m_window->setCallbackKey( KeyCallback );
            m_window->setCallbackChar( CharCallback );

            // Zero out arrays
            memset( m_keyPressed, 0, MAX_KEYS * sizeof( bool ) );
            memset( m_keyReleased, 0, MAX_KEYS * sizeof( bool ) );
            memset( m_keyPressedThisTick, 0, MAX_KEYS * sizeof( bool ) );
            memset( m_keyPressedLastTick, 0, MAX_KEYS * sizeof( bool ) );

            // Preallocate mem for stl stuff
            m_keyListener.reserve( LISTENER_START_CAPACITY );
        }

        //----------------------------------------------------------------------
        Keyboard::~Keyboard()
        {
            // Unregister from window events
            m_window->setCallbackKey( nullptr );
            m_window->setCallbackChar( nullptr );
        }

        //----------------------------------------------------------------------
        void Keyboard::_UpdateInternalState()
        {
            _UpdateKeyStates();
        }

        //----------------------------------------------------------------------
        bool Keyboard::isKeyDown( Key key ) const
        {
            return m_keyPressedThisTick[ (I32)key ];
        }

        //----------------------------------------------------------------------
        bool Keyboard::wasKeyPressed( Key key ) const
        {
            I32 keyIndex = (I32)key;
            return ( m_keyPressedThisTick[ keyIndex ] && not m_keyPressedLastTick[ keyIndex ] );
        }

        //----------------------------------------------------------------------
        bool Keyboard::wasKeyReleased( Key key ) const
        {
            I32 keyIndex = (I32)key;
            return ( not m_keyPressedThisTick[ keyIndex ] && m_keyPressedLastTick[ keyIndex ] );
        }

        //**********************************************************************
        // PRIVATE
        //**********************************************************************

        //----------------------------------------------------------------------
        void Keyboard::_KeyCallback( Key key, KeyAction action, KeyMod mod )
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
        void Keyboard::_CharCallback( char c )
        {
            _NotifyOnChar( c );
        }

        //**********************************************************************
        void Keyboard::_UpdateKeyStates()
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

        //**********************************************************************
        void Keyboard::_NotifyKeyPressed( Key key, KeyMod mod ) const
        {
            for (auto& listener : m_keyListener)
                listener->OnKeyPressed( key, mod);
        }

        //----------------------------------------------------------------------
        void Keyboard::_NotifyKeyReleased( Key key, KeyMod mod ) const
        {
            for (auto& listener : m_keyListener)
                listener->OnKeyReleased( key, mod);
        }

        //----------------------------------------------------------------------
        void Keyboard::_NotifyOnChar(char c) const
        {
            for (auto& listener : m_keyListener)
                listener->OnChar( c );
        }

    }
} // end namespaces