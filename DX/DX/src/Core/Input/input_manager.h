#pragma once
/**********************************************************************
    class: InputManager (input_manager.h)

    author: S. Hau
    date: November 4, 2017

    - Virtual Mappings
      - just another level of indirection. 
        Store a map of [Key <-> Key] and check which key corresponds to the "virtual key"

    - Fixed mouse pos
    - Mouse delta

    - Ask if key is pressed CONTINOUSLY
    - Ask if key is pressed THIS FRAME

    {
        - Subscribe to events? 
           - e.g. Component A is interested in OnKeyDown(...);
             > Locator::getInputManager().subscribeMouseMoved( this );
                -> OnMouseMove(x, y) is called on this object then.
    }

    - ActionNames e.g. "MoveForward" -> 
       should trigger when e.g. "W" is pressed OR controller "Forward"
       -> Map actions to names, so the action can be triggered regardless of input device


    Responsibilites:
      - TODO
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "Core/OS/Window/keycodes.h"
#include "listener/input_listener.h"

namespace Core { namespace Input {


    //**********************************************************************
    class InputManager : public ISubSystem
    {
        static const U32 MAX_KEYS = 255;
        static const U32 MAX_MOUSE_KEYS = 16;

    public:
        InputManager() = default;
        ~InputManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void OnTick(Time::Seconds delta) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // @Return: True when the given key is down.
        //----------------------------------------------------------------------
        bool isKeyDown( Key key ) const;

        //----------------------------------------------------------------------
        // @Return: True, the frame the key was pressed.
        //----------------------------------------------------------------------
        bool wasKeyPressed( Key key ) const;

        //----------------------------------------------------------------------
        // @Return: True, the frame the key was released.
        //----------------------------------------------------------------------
        bool wasKeyReleased( Key key ) const;

        //----------------------------------------------------------------------
        // @Return: True when the given mousekey is down.
        //----------------------------------------------------------------------
        bool isMouseKeyDown(MouseKey key) const;

        //----------------------------------------------------------------------
        // @Return: True, the frame the mousekey was pressed.
        //----------------------------------------------------------------------
        bool wasMouseKeyPressed(MouseKey key) const;

        //----------------------------------------------------------------------
        // @Return: True, the frame the mousekey was released.
        //----------------------------------------------------------------------
        bool wasMouseKeyReleased(MouseKey key) const;

        //----------------------------------------------------------------------
        // @Return: Forward: 1, Backwards: -1 [For one frame]. Otherwise 0.
        //----------------------------------------------------------------------
        I16 getWheelDelta() const { return m_wheelDelta; }

        //----------------------------------------------------------------------
        //Vec2 getMousePos() const { return Vec2( m_cursorX, m_cursorY ); }
        //Vec2 getMouseDelta() const {}

        //----------------------------------------------------------------------
        void _KeyCallback(Key key, KeyAction action, KeyMod mod);
        void _CharCallback(char c);
        void _MouseCallback(MouseKey key, KeyAction action, KeyMod mod);
        void _MouseWheelCallback(I16 param);
        void _CursorMovedCallback(I16 x, I16 y);

    private:
        // <---------- KEYBOARD ----------->
        bool    m_keyPressed[MAX_KEYS];
        bool    m_keyReleased[MAX_KEYS];

        bool    m_keyPressedThisTick[MAX_KEYS];
        bool    m_keyPressedLastTick[MAX_KEYS];

        // <---------- MOUSE ----------->
        bool    m_mouseKeyPressed[MAX_MOUSE_KEYS];
        bool    m_mouseKeyReleased[MAX_MOUSE_KEYS];

        bool    m_mouseKeyPressedThisTick[MAX_MOUSE_KEYS];
        bool    m_mouseKeyPressedLastTick[MAX_MOUSE_KEYS];

        I16     m_cursorX = 0;
        I16     m_cursorY = 0;
        I16     m_wheelDelta = 0;

        // <---------- LISTENER ----------->
        ArrayList<IKeyListener*>        m_keyListener;
        ArrayList<IMouseListener*>      m_mouseListener;
        HashMap<Key, Key>               m_virtualKeys;

        //----------------------------------------------------------------------
        friend class IKeyListener;
        void _Subscribe(IKeyListener* listener) { m_keyListener.push_back( listener ); }
        void _Unsubscribe(IKeyListener* listener) { m_keyListener.erase( std::remove( m_keyListener.begin(), m_keyListener.end(), listener ) ); }

        friend class IMouseListener;
        void _Subscribe(IMouseListener* listener) { m_mouseListener.push_back( listener ); }
        void _Unsubscribe(IMouseListener* listener) { m_mouseListener.erase( std::remove( m_mouseListener.begin(), m_mouseListener.end(), listener ) ); }

        //----------------------------------------------------------------------
        void _UpdateKeyStates();
        void _NotifyKeyPressed(Key key, KeyMod mod) const;
        void _NotifyKeyReleased(Key key, KeyMod mod) const;
        void _NotifyOnChar(char c) const;

        void _UpdateMouseStates();
        void _NotifyMouseMoved() const;
        void _NotifyMouseKeyPressed(MouseKey key, KeyMod mod) const;
        void _NotifyMouseKeyReleased(MouseKey key, KeyMod mod) const;
        void _NotifyMouseWheel() const;

        //----------------------------------------------------------------------
        InputManager(const InputManager& other)                 = delete;
        InputManager& operator = (const InputManager& other)    = delete;
        InputManager(InputManager&& other)                      = delete;
        InputManager& operator = (InputManager&& other)         = delete;
    };


} }
