#pragma once
/**********************************************************************
    class: Mouse (mouse.h)

    author: S. Hau
    date: November 18, 2017

    Responsibilites:
      - Encapsulation of an Mouse device. Retrieves the mouse events
        from the underlying OS and makes it suitable for anyone else.
**********************************************************************/

#include "OS/Window/window.h" /* Point2D, Keycodes */
#include "Core/Input/input_enums.hpp"
#include "../channel_user.hpp"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    class IMouseListener;

    //**********************************************************************
    class Mouse : public IChannelUser
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
        I16     getWheelDelta() const;

        //----------------------------------------------------------------------
        // True: Mouse is hidden and cursor is always centered.
        //----------------------------------------------------------------------
        void    setFirstPersonMode(bool enabled);

        //----------------------------------------------------------------------
        // @Return: Delta position from the mouse between last and current tick.
        //----------------------------------------------------------------------
        OS::Point2D getMouseDelta() const;

        //----------------------------------------------------------------------
        OS::Point2D getMousePos() const { return m_cursorThisTick; }
        bool isInFirstPersonMode() const { return m_firstPersonMode; }

    private:
        OS::Window*     m_window            = nullptr;
        bool            m_firstPersonMode   = false;

        // These will be updated whenever the OS messages will be processed (faster than tick-rate)
        bool    m_mouseKeyPressed[MAX_MOUSE_KEYS];
        bool    m_mouseKeyReleased[MAX_MOUSE_KEYS];

        // These contains the state of the current and previous tick
        bool    m_mouseKeyPressedThisTick[MAX_MOUSE_KEYS];
        bool    m_mouseKeyPressedLastTick[MAX_MOUSE_KEYS];

        // 1 if Forward, -1 if Backwards (only for one tick)
        I16     m_wheelDelta = 0;

        OS::Point2D m_cursor, m_cursorThisTick, m_cursorLastTick, m_cursorDelta;

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

        //----------------------------------------------------------------------
        friend class InputManager;
        void _UpdateInternalState();

        // Callbacks for windows-messages
        void _MouseCallback(MouseKey key, KeyAction action, KeyMod mod);
        void _MouseWheelCallback(I16 param);
        void _CursorMovedCallback(I16 x, I16 y);

        NULL_COPY_AND_ASSIGN(Mouse)
    };



} } // end namespaces