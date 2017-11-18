#pragma once
/**********************************************************************
    class: Mouse (mouse.h)

    author: S. Hau
    date: November 18, 2017

    @TODO: Replace Point2D by Vec2D

    Responsibilites:
      - Encapsulation of an Mouse device. Retrieves the mouse events
        from the underlying OS and makes it suitable for anyone else.
**********************************************************************/

#include "Core/OS/Window/keycodes.h"

//----------------------------------------------------------------------
namespace Core { namespace OS { class Window; } }


namespace Core { namespace Input {

    //----------------------------------------------------------------------
    class IMouseListener;

    //**********************************************************************
    class Mouse
    {
        static const U32 MAX_MOUSE_KEYS = 16;

    public:
        Mouse(OS::Window* window);
        ~Mouse();

        //----------------------------------------------------------------------
        void    centerCursor() const;
        void    showCursor(bool b) const;

        //----------------------------------------------------------------------
        // @Return: True when the given mousekey is down.
        //----------------------------------------------------------------------
        bool    isKeyDown(MouseKey key) const;

        //----------------------------------------------------------------------
        // @Return: True, the tick the mousekey was pressed.
        //----------------------------------------------------------------------
        bool    wasKeyPressed(MouseKey key) const;

        //----------------------------------------------------------------------
        // @Return: True, the tick the mousekey was released.
        //----------------------------------------------------------------------
        bool    wasKeyReleased(MouseKey key) const;

        //----------------------------------------------------------------------
        // @Return: Forward: 1, Backwards: -1 [For one frame]. Otherwise 0.
        //----------------------------------------------------------------------
        I16     getWheelDelta() const { return m_wheelDelta; }

        //----------------------------------------------------------------------
        // True: Mouse is hidden and cursor is always centered.
        //----------------------------------------------------------------------
        void    setFirstPersonMode(bool enabled);

        //----------------------------------------------------------------------
        //Vec2 getMousePos() const { return m_cursorThisTick; }
        //Vec2 getMouseDelta() const { return m_cursorDelta; }
        void    getMouseDelta(I16& x, I16& y) const { x = m_cursorDelta.x; y = m_cursorDelta.y; }

        //----------------------------------------------------------------------
        void    _UpdateInternalState();

    private:
        OS::Window* m_window            = nullptr;
        bool        m_firstPersonMode   = false;

        // These will be updated whenever the OS messages will be processed (faster than tick-rate)
        bool    m_mouseKeyPressed[MAX_MOUSE_KEYS];
        bool    m_mouseKeyReleased[MAX_MOUSE_KEYS];

        // These contains the state of the current and previous tick
        bool    m_mouseKeyPressedThisTick[MAX_MOUSE_KEYS];
        bool    m_mouseKeyPressedLastTick[MAX_MOUSE_KEYS];

        // 1 if Forward, -1 if Backwards (only for one tick)
        I16     m_wheelDelta = 0;

        struct Point2D
        {
            I16 x = 0;
            I16 y = 0;

            Point2D operator - (const Point2D& other) { return Point2D{ x - other.x, y - other.y }; }
        } m_cursor, m_cursorThisTick, m_cursorLastTick, m_cursorDelta;

        //----------------------------------------------------------------------
        void _UpdateMouseKeyStates();
        void _UpdateCursorDelta();

        // <------------ LISTENER ------------->
        ArrayList<IMouseListener*>  m_mouseListener;

        friend class IMouseListener;
        void _Subscribe(IMouseListener* listener) { m_mouseListener.push_back( listener ); }
        void _Unsubscribe(IMouseListener* listener) { m_mouseListener.erase( std::remove( m_mouseListener.begin(), m_mouseListener.end(), listener ) ); }

        void _NotifyMouseMoved(I16 x, I16 y) const;
        void _NotifyMouseKeyPressed(MouseKey key, KeyMod mod) const;
        void _NotifyMouseKeyReleased(MouseKey key, KeyMod mod) const;
        void _NotifyMouseWheel(I16 delta) const;

    public:
        // Callbacks for windows-messages
        void _MouseCallback(MouseKey key, KeyAction action, KeyMod mod);
        void _MouseWheelCallback(I16 param);
        void _CursorMovedCallback(I16 x, I16 y);

    private:
        //----------------------------------------------------------------------
        Mouse(const Mouse& other)                 = delete;
        Mouse& operator = (const Mouse& other)    = delete;
        Mouse(Mouse&& other)                      = delete;
        Mouse& operator = (Mouse&& other)         = delete;
    };



} } // end namespaces