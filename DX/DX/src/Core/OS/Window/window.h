#pragma once
/**********************************************************************
    class: Window (window.h)

    author: S. Hau
    date: November 3, 2017

    Responsibilites:
      - Create a window in which the engine can draw.
      - Peek and Dispatch OS messages
**********************************************************************/

#include "../FileSystem/path.h"

namespace Core { namespace OS {

    //**********************************************************************
    class Window
    {
        static Window* s_instance;

    public:
        Window();
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
        void pollEvents();

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
        void setCursorPosition(U32 x, U32 y) const;
        void centerCursor() const;

        //----------------------------------------------------------------------
        // Enables / Disables borderless fullscreen for this window.
        // @Params:
        //  "enabled": True when the window should be maximized/borderless.
        //----------------------------------------------------------------------
        void setBorderlessFullscreen(bool enabled);

    private:
        U32         m_width             = 0;
        U32         m_height            = 0;
        bool        m_created           = false;
        bool        m_shouldBeClosed    = false;
    };


} } // end namespaces