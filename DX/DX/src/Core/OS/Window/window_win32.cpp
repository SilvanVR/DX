#include "window.h"
/**********************************************************************
    class: Window (window_win32.cpp)

    author: S. Hau
    date: November 3, 2017

    Windows implementation for creating a window.
**********************************************************************/

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef ERROR

#include "locator.h"

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    HINSTANCE hInstance;
    HWND hwnd;

    //----------------------------------------------------------------------
    bool RegisterClassWindow( HINSTANCE hInstance, LPCSTR className );


    //----------------------------------------------------------------------
    LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        switch ( uMsg )
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        // TODO: Handle Input


        default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
        }
    }

    //----------------------------------------------------------------------
    bool Window::pollEvents()
    {
        MSG msg;

        while ( PeekMessage( &msg, NULL, NULL, NULL, PM_REMOVE ) )
        {
            if ( msg.message == WM_QUIT )
                return false;

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

        return true;
    }


    //----------------------------------------------------------------------
    void Window::create()
    {
        LPCSTR className = "DXWNDClassName";

        if ( not RegisterClassWindow( hInstance, className ) )
        {
            ERROR( "Window[Win32]::create(): Failed to register a window class." );
        }

        UINT screenResX = GetSystemMetrics( SM_CXSCREEN );
        UINT screenResY = GetSystemMetrics( SM_CYSCREEN );

        UINT x = (UINT) ( screenResX * 0.5f - (m_width * 0.5f) );
        UINT y = (UINT) ( screenResY * 0.5f - (m_height * 0.5f) );

        hwnd = CreateWindow( className, m_title.c_str(), 
                             WS_OVERLAPPEDWINDOW, 
                             x, y, 
                             m_width, m_height, 
                             NULL, NULL, hInstance, NULL );

        if ( not hwnd )
        {
            ERROR( "Window[Win32]::create(): Failed to create a window. " );
        }

        ShowWindow( hwnd, SW_SHOW );
        m_created = true;
    }

    //----------------------------------------------------------------------
    void Window::destroy()
    {
        if ( not DestroyWindow( hwnd ) )
        {
            ERROR( "Window[Win32]::destroy(): Failed to destroy the window. " );
        }
        m_created = false;
    }

    //----------------------------------------------------------------------
    bool RegisterClassWindow( HINSTANCE hInstance, LPCSTR className )
    {
        WNDCLASSEX lpWndClass;
        lpWndClass.cbSize = sizeof( WNDCLASSEX );
        lpWndClass.style = ( CS_HREDRAW | CS_VREDRAW );
        lpWndClass.lpfnWndProc = WindowProc;
        lpWndClass.cbClsExtra = 0;
        lpWndClass.cbWndExtra = 0;
        lpWndClass.hInstance = hInstance;
        lpWndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
        lpWndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
        lpWndClass.hbrBackground = NULL;
        lpWndClass.lpszMenuName = NULL;
        lpWndClass.lpszClassName = className;
        lpWndClass.hIconSm = LoadIcon( NULL, IDI_APPLICATION );

        return RegisterClassEx( &lpWndClass );
    }

} } // end namespaces


#endif