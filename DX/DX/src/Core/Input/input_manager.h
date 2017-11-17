#pragma once
/**********************************************************************
    class: InputManager (input_manager.h)

    author: S. Hau
    date: November 4, 2017

    - ActionNames e.g. "MoveForward" -> 
       should trigger when e.g. "W" is pressed OR controller "Forward"
       -> Map actions to names, so the action can be triggered regardless of input device
    - Virtual Mappings
      - just another level of indirection. 
        Store a map of [Key <-> Key] and check which key corresponds to the "virtual key"

    - Axis Mapping

    Responsibilites:
      - Process the input window callbacks and saves the current state
        of input devices to be queried by anyone.

    Features:
      - Ask the InputManager about the current state e.g. mouse-pos etc.
      - Subscribe indirectly via inheritation from a listener class:
         IKeyListener: Override methods to get notified by key events.
         IMouseListener: Override methods to get notified by mouse events.
        Listener callbacks will be called whenever they occur in the
        update loop (possibly faster than tick rate)
**********************************************************************/

#include "Core/i_subsystem.hpp"
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
        //Vec2 getMouseDelta() const { return Vec2( (m_cursorThisTickX - m_cursorLastTickX), (m_cursorThisTickY - m_cursorLastTickY) ); }
        void getMouseDelta(I16& x, I16& y) const { x = m_cursorDeltaX; y = m_cursorDeltaY; }

        //----------------------------------------------------------------------
        // Enable/Disable the first person mode.
        // True: Mouse is hidden and cursor is always centered.
        //----------------------------------------------------------------------
        void enableFirstPersonMode(bool enabled);

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

        I16     m_wheelDelta = 0;
        I16     m_cursorX = 0;
        I16     m_cursorY = 0;
        I16     m_cursorThisTickX = 0;
        I16     m_cursorThisTickY = 0;
        I16     m_cursorLastTickX = 0;
        I16     m_cursorLastTickY = 0;
        I16     m_cursorDeltaX = 0;
        I16     m_cursorDeltaY = 0;

        // <---------- MISC ----------->
        bool    m_firstPersonMode = false;

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

        void _UpdateCursorDelta();

        //----------------------------------------------------------------------
        InputManager(const InputManager& other)                 = delete;
        InputManager& operator = (const InputManager& other)    = delete;
        InputManager(InputManager&& other)                      = delete;
        InputManager& operator = (InputManager&& other)         = delete;
    };


} }
