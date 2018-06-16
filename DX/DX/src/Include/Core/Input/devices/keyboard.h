#pragma once
/**********************************************************************
    class: Keyboard (keyboard.h)

    author: S. Hau
    date: November 18, 2017

    Responsibilites:
      - Encapsulation of an Keyboard device. Retrieves the key events
        from the underlying OS and makes it suitable for anyone else.
**********************************************************************/

#include "OS/Window/keycodes.h"

//----------------------------------------------------------------------
namespace OS { class Window; }


namespace Core { namespace Input {

    //----------------------------------------------------------------------
    class IKeyListener;

    //**********************************************************************
    class Keyboard
    {
        static const U32 MAX_KEYS = 255;

    public:
        Keyboard(OS::Window* window);
        ~Keyboard();

        //----------------------------------------------------------------------
        // @Return: True when the given key is down.
        //----------------------------------------------------------------------
        bool isKeyDown(Key key) const;

        //----------------------------------------------------------------------
        // @Return: True, the frame the key was pressed.
        //----------------------------------------------------------------------
        bool wasKeyPressed(Key key) const;

        //----------------------------------------------------------------------
        // @Return: True, the frame the key was released.
        //----------------------------------------------------------------------
        bool wasKeyReleased(Key key) const;

    private:
        OS::Window* m_window            = nullptr;

        // <---------- KEYBOARD ----------->
        // Those will be updated per update
        bool    m_keyPressed[MAX_KEYS];
        bool    m_keyReleased[MAX_KEYS];

        // Those will be updated per tick
        bool    m_keyPressedThisTick[MAX_KEYS];
        bool    m_keyPressedLastTick[MAX_KEYS];

        //----------------------------------------------------------------------
        void _UpdateKeyStates();

        // <------------ LISTENER ------------->
        ArrayList<IKeyListener*> m_keyListener;

        friend class IKeyListener;
        void _Subscribe(IKeyListener* listener) { m_keyListener.push_back( listener ); }
        void _Unsubscribe(IKeyListener* listener) { m_keyListener.erase( std::remove( m_keyListener.begin(), m_keyListener.end(), listener ) ); }

        void _NotifyKeyPressed(Key key, KeyMod mod) const;
        void _NotifyKeyReleased(Key key, KeyMod mod) const;
        void _NotifyOnChar(char c) const;

    private:
        //----------------------------------------------------------------------
        friend class InputManager;
        void _UpdateInternalState();

        // Callbacks for windows-messages
        void _KeyCallback(Key key, KeyAction action, KeyMod mod);
        void _CharCallback(char c);

        NULL_COPY_AND_ASSIGN(Keyboard)
    };



} } // end namespaces