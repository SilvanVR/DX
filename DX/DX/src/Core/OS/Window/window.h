#pragma once
/**********************************************************************
    class: Window (window.h)

    author: S. Hau
    date: November 3, 2017

    Responsibilites:
      - Create a window in which the engine can draw.
      - Process OS messages
        > Subscribe to the window via callbacks functions, which will
          be called when a corresponding event has happened.
          Use either a plain old c-function or if you want to call a
          function on an object "std::bind(...)". Macros are available
          in the way: BIND_THIS_FUNC_[NUM_ARGS]_ARGS(FUNCTION) e.g.
          "setCallbackChar( BIND_THIS_FUNC_1_ARGS( &Object::Func ) );"
**********************************************************************/

#include "../FileSystem/path.h"
#include "keycodes.h"

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    struct Point2D
    {
        I16 x = 0;
        I16 y = 0;

        Point2D operator - (const Point2D& other) { return Point2D{ x - other.x, y - other.y }; }
        Point2D operator + (const Point2D& other) { return Point2D{ x + other.x, y + other.y }; }
    };

    //----------------------------------------------------------------------
    typedef std::function<void(I16, I16)>                       WindowCursorMoveFunc;
    typedef std::function<void(I16)>                            WindowMouseWheelFunc;
    typedef std::function<void(MouseKey, KeyAction, KeyMod)>    WindowMouseButtonFunc;
    typedef std::function<void(U16, U16)>                       WindowSizeChangedFunc;
    typedef std::function<void(Key, KeyAction, KeyMod)>         WindowKeyButtonFunc;
    typedef std::function<void()>                               WindowLooseFocusFunc;
    typedef std::function<void()>                               WindowGainFocusFunc;
    typedef std::function<void(char)>                           WindowCharFunc;

    //**********************************************************************
    class Window
    {
    public:
        Window() = default;
        ~Window();

        //----------------------------------------------------------------------
        bool        shouldBeClosed() const { return m_shouldBeClosed; }
        U16         getWidth() const { return m_width; }
        U16         getHeight() const { return m_height; }
        Point2D     getSize() const { return Point2D{ (I16)m_width, (I16)m_height }; }

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
        void        setCursorPosition(I16 x, I16 y) const;
        Point2D     getCursorPosition() const;
        void        centerCursor() const;

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
        void setCallbackMouseWheel(WindowMouseWheelFunc func) { m_callbackHelper.m_mouseWheelCallback = func; }

        //----------------------------------------------------------------------
        // Given function will be called whenever a mouse button was pressed.
        // @Function-Params:
        //  |MouseKey|:   KeyCode of the corresponding mouse button
        //  |KeyAction|:  Action of the button, e.g. pressed or released
        //  |KeyMod|:     Additional modifiers, e.g. control/shift was down
        //----------------------------------------------------------------------
        void setCallbackMouseButtons(WindowMouseButtonFunc func) { m_callbackHelper.m_mouseButtonCallback = func; }

        //----------------------------------------------------------------------
        // Given function will be called whenever the window size has changed.
        // @Function-Params:
        //   |U16|: New width of the window.
        //   |U16|: New height of the window.
        //---------------------------------------------------------------------
        void setCallbackSizeChanged(WindowSizeChangedFunc func) { m_callbackHelper.m_sizeChangedCallback = func; }

        //----------------------------------------------------------------------
        // Given function will be called whenever a key is pressed or released.
        // @Function-Params:
        //  |Key|:        Keycode of the corresponding button
        //  |KeyAction|:  Action of the button, e.g. pressed or released
        //  |KeyMod|:     Additional modifiers, e.g. control/shift was down
        //---------------------------------------------------------------------
        void setCallbackKey(WindowKeyButtonFunc func) { m_callbackHelper.m_keyCallback = func; }

        //----------------------------------------------------------------------
        // Given function will be called whenever the window looses focus.
        //---------------------------------------------------------------------
        void setCallbackLooseFocus(WindowLooseFocusFunc func) { m_callbackHelper.m_looseFocusCallback = func; }

        //----------------------------------------------------------------------
        // Given function will be called whenever the window gains focus.
        //---------------------------------------------------------------------
        void setCallbackGainFocus(WindowGainFocusFunc func) { m_callbackHelper.m_gainFocusCallback = func; }

        //----------------------------------------------------------------------
        // Given function will be called whenever a character button was pressed.
        // The difference to the "KeyCallback" is that this function returns
        // the correct encoded character. E.g. ("Shift + a" = A) wherever only ("a" = a)
        // @Function-Params:
        //  |char|: Character which was pressed.
        //---------------------------------------------------------------------
        void setCallbackChar(WindowCharFunc func) { m_callbackHelper.m_charCallback = func; }

    private:
        U16             m_width             = 0;
        U16             m_height            = 0;
        bool            m_created           = false;
        bool            m_shouldBeClosed    = false;

        //**********************************************************************
        // Small helper class which stores all the different window callbacks.
        // The WindowProc function access those via a static instance of this
        // object. If a callback is not set, it it simply ignored, when called.
        //**********************************************************************
        class WindowCallbackHelper
        {
        public:
            void callCursorCallback(I16 x, I16 y) const;
            void callMouseWheelCallback(I16 delta) const;
            void callMouseButtonCallback(MouseKey, KeyAction, KeyMod) const;
            void callSizeChangedCallback(U16 width, U16 height) const;
            void callKeyCallback(Key, KeyAction, KeyMod) const;
            void callLooseFocusCallback() const;
            void callGainFocusCallback() const;
            void callCharCallback(char) const;

        private:
            friend class Window;
            WindowCursorMoveFunc    m_cursorMoveCallback    = nullptr;
            WindowMouseWheelFunc    m_mouseWheelCallback    = nullptr;
            WindowMouseButtonFunc   m_mouseButtonCallback   = nullptr;
            WindowSizeChangedFunc   m_sizeChangedCallback   = nullptr;
            WindowKeyButtonFunc     m_keyCallback           = nullptr;
            WindowLooseFocusFunc    m_looseFocusCallback    = nullptr;
            WindowGainFocusFunc     m_gainFocusCallback     = nullptr;
            WindowCharFunc          m_charCallback          = nullptr;
        } static m_callbackHelper;

    public:
        //----------------------------------------------------------------------
        static Window::WindowCallbackHelper _GetCallbackHelper() { return m_callbackHelper; }
        void _SetSize(U16 width, U16 height) { m_width = width; m_height = height; }

    private:
        Window(const Window& other)                 = delete;
        Window& operator = (const Window& other)    = delete;
        Window(Window&& other)                      = delete;
        Window& operator = (Window&& other)         = delete;
    };


} } // end namespaces