#pragma once
/**********************************************************************
    class: Window (window.h)

    author: S. Hau
    date: November 3, 2017

    Responsibilites:
      - Create a window in which the engine can draw.
      - Process OS messages
        > Subscribe to the window via callbacks functions, which will
          be called when a corresponding event has happened
**********************************************************************/

#include "../FileSystem/path.h"
#include "keycodes.hpp"

namespace Core { namespace OS {

    typedef void(*WindowCursorMoveFunc)(I16, I16);
    typedef void(*WindowMouseWheelFunc)(I16);
    typedef void(*WindowMouseButtonFunc)(KeyCode, KeyAction, KeyMod);
    typedef void(*WindowSizeChangedFunc)(U16, U16);

    //**********************************************************************
    class Window
    {
    public:
        Window() = default;
        ~Window();

        //----------------------------------------------------------------------
        bool shouldBeClosed() const { return m_shouldBeClosed; }
        U32 getWidth() const { return m_width; }
        U32 getHeight() const { return m_height; }

        //----------------------------------------------------------------------
        void setTitle(const char* newTitle) const;
        void center() const;

        //----------------------------------------------------------------------
        // Creates the window.
        // @Params:
        //  "title": The title of the window.
        //  "witdh / height": Width / Height of the window in pixels.
        //----------------------------------------------------------------------
        void create(const char* title, U32 width, U32 height);

        //----------------------------------------------------------------------
        // Tells the OS to destroy the window. Done automatically in the destructor.
        //----------------------------------------------------------------------
        void destroy();

        //----------------------------------------------------------------------
        // Processes event messages from the OS. 
        //----------------------------------------------------------------------
        void processOSMessages();

        //----------------------------------------------------------------------
        // Enables / Disables the mouse cursor
        //----------------------------------------------------------------------
        void showCursor(bool b) const;

        //----------------------------------------------------------------------
        // Changes the image from the cursor. Extension must be ".cur"
        //----------------------------------------------------------------------
        void setCursor(const Path& path) const;

        //----------------------------------------------------------------------
        // Changes the window icon. Extension must be ".ico"
        //----------------------------------------------------------------------
        void setIcon(const Path& path) const;

        //----------------------------------------------------------------------
        // Directly manipulate the mouse position. (0,0) is Top-Left corner.
        //----------------------------------------------------------------------
        void setCursorPosition(U16 x, U16 y) const;
        void centerCursor() const;

        //----------------------------------------------------------------------
        // Enables / Disables borderless fullscreen for this window.
        // @Params:
        //  "enabled": True when the window should be maximized and 
        //             become borderless.
        //----------------------------------------------------------------------
        void setBorderlessFullscreen(bool enabled);

        //----------------------------------------------------------------------
        // Given function will be called whenever the mouse gets a new position.
        // @Function-Params:
        //   |I16|: X-Coordinate of the new position
        //   |I16|: Y-Coordinate of the new position
        //----------------------------------------------------------------------
        void setCallbackCursorMove(WindowCursorMoveFunc func) { m_callbackHelper.m_cursorMoveCallback = func; }

        //----------------------------------------------------------------------
        // Given function will be called whenever the mouse-wheel was used.
        // @Function-Params:
        //   |I16|: +1 if FORWARD, -1 if BACKWARDS
        //---------------------------------------------------------------------
        void setCallbackMouseWheel(WindowMouseWheelFunc func) { m_callbackHelper.m_mouseWheelFunc = func; }

        //----------------------------------------------------------------------
        // Given function will be called whenever a mouse button was pressed.
        // @Function-Params:
        //  |KeyCode|:    KeyCode of the corresponding button
        //  |KeyAction|:  Action of the button, e.g. pressed or released
        //  |KeyMod|:     Additional modifiers, e.g. control/shift was down
        //----------------------------------------------------------------------
        void setCallbackMouseButtons(WindowMouseButtonFunc func) { m_callbackHelper.m_mouseButtonFunc = func; }

        //----------------------------------------------------------------------
        // Given function will be called whenever the window size has changed.
        // @Function-Params:
        //   |U16|: New width of the window.
        //   |U16|: New height of the window.
        //---------------------------------------------------------------------
        void setCallbackSizeChanged(WindowSizeChangedFunc func) { m_callbackHelper.m_sizeChangedFunc = func; }

    private:
        U16             m_width             = 0;
        U16             m_height            = 0;
        bool            m_created           = false;
        bool            m_shouldBeClosed    = false;

        //**********************************************************************
        // Small helper class which stores all the different window callbacks.
        // The WindowProc function access those via a static instance of this
        // object. If a callback is not set, it it simply ignored.
        //**********************************************************************
        class WindowCallbackHelper
        {
        public:
            void callCursorCallback(I16 x, I16 y) const;
            void callMouseWheelCallback(I16 delta) const;
            void callMouseButtonCallback(KeyCode, KeyAction, KeyMod) const;
            void callSizeChangedCallback(U16 width, U16 height) const;

        private:
            friend class Window;
            WindowCursorMoveFunc    m_cursorMoveCallback    = nullptr;
            WindowMouseWheelFunc    m_mouseWheelFunc        = nullptr;
            WindowMouseButtonFunc   m_mouseButtonFunc       = nullptr;
            WindowSizeChangedFunc   m_sizeChangedFunc       = nullptr;
        };

        static WindowCallbackHelper m_callbackHelper;

    public:
        //----------------------------------------------------------------------
        static Window::WindowCallbackHelper _GetCallbackHelper() { return m_callbackHelper; }

    private:
        Window(const Window& other)                 = delete;
        Window& operator = (const Window& other)    = delete;
        Window(Window&& other)                      = delete;
        Window& operator = (Window&& other)         = delete;
    };


} } // end namespaces