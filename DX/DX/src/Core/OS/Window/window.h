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

    private:
        U32         m_width             = 0;
        U32         m_height            = 0;
        bool        m_created           = false;
        bool        m_shouldBeClosed    = false;
    };


} } // end namespaces