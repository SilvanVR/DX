#pragma once
/**********************************************************************
    class: Window (window.h)

    author: S. Hau
    date: November 3, 2017

    Responsibilites:
      - Create a window in which the engine can draw.
      - Peek and Dispatch OS messages
**********************************************************************/

namespace Core { namespace OS {


    class Window
    {
        static Window* s_instance;

    public:
        Window(const char* title, U32 width, U32 height);
        ~Window();

        void create();
        void destroy();

        bool pollEvents();
        //bool shouldClose() const { return m_shouldBeClosed; }

    private:
        U32         m_width;
        U32         m_height;
        String      m_title;
        bool        m_created = false;
       // bool        m_shouldBeClosed = false;
    };


} } // end namespaces