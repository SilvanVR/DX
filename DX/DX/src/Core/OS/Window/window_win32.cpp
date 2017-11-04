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
    HANDLE loadImageFromFile(const char* path, UINT type);

    //----------------------------------------------------------------------
    LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        switch ( uMsg )
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        // TODO: Handle Input
        // TODO: Handle Resize

        case WM_SETCURSOR:
            return 0; // Default WindowProc uses default cursor, so just return here.

        default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
        }
    }

    //----------------------------------------------------------------------
    void Window::pollEvents()
    {
        MSG msg;
        while ( PeekMessage( &msg, NULL, NULL, NULL, PM_REMOVE ) )
        {
            if ( msg.message == WM_QUIT )
                m_shouldBeClosed = true;

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }


    //----------------------------------------------------------------------
    void Window::create( const char* title, U32 width, U32 height )
    {
        ASSERT( m_created == false );
        LPCSTR className = "DXWNDClassName";

        if ( not RegisterClassWindow( hInstance, className ) )
        {
            ERROR( "Window[Win32]::create(): Failed to register a window class." );
        }

        UINT screenResX = GetSystemMetrics( SM_CXSCREEN );
        UINT screenResY = GetSystemMetrics( SM_CYSCREEN );

        UINT x = (UINT) ( screenResX * 0.5f - (width * 0.5f) );
        UINT y = (UINT) ( screenResY * 0.5f - (height * 0.5f) );

        hwnd = CreateWindow( className, title,
                             WS_OVERLAPPEDWINDOW, 
                             x, y, 
                             width, height, 
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

    //----------------------------------------------------------------------
    void Window::showCursor(bool b) const
    {
        ShowCursor( b );
    }

    //----------------------------------------------------------------------
    void Window::setCursor(const Path& path) const
    {
        HCURSOR cursor = (HCURSOR) loadImageFromFile( path, IMAGE_CURSOR );
        if (cursor == NULL)
        {
           WARN( "Window[Win32]::setCursor(): Could not load cursor '" + path.toString() + "'. Wrong Extension? (.cur)." );
           return;
        }

        SetCursor( cursor );
    }

    //----------------------------------------------------------------------
    void Window::setIcon(const Path& path) const
    {
        HICON icon = (HICON) loadImageFromFile( path, IMAGE_ICON );
        if (icon == NULL)
        {
           WARN( "Window[Win32]::setIcon(): Could not load icon '" + path.toString() + "'. Wrong Extension? (.ico)." );
           return;
        }

        SendMessage( hwnd, (UINT) WM_SETICON, ICON_BIG, (LPARAM) icon );
    }

    //----------------------------------------------------------------------
    void PrintLastErrorAsString()
    {
        //Get the error message, if any.
        DWORD errorMessageID = GetLastError();
        LPSTR messageBuffer = nullptr;
        Size size = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                    NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL );

        printf( "Last Win32 Error: %s", messageBuffer );

        //Free the buffer.
        LocalFree(messageBuffer);
    }

    //----------------------------------------------------------------------
    HANDLE loadImageFromFile( const char* path, UINT type )
    {
        HANDLE img = LoadImage( NULL, path, type, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE );
        return img;
    }


} } // end namespaces


#endif