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
#include <windowsx.h> /* GET_X_LPARAM, GET_Y_LPARAM */
#undef ERROR

#include "locator.h"

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    HINSTANCE   hInstance;
    HWND        hwnd;

    //----------------------------------------------------------------------
    bool    RegisterWindowClass( HINSTANCE hInstance, LPCSTR className );
    HANDLE  LoadImageFromFile( const char* path, UINT type );
    KeyMod  GetKeyMod( WPARAM wParam );

    //----------------------------------------------------------------------
    LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        switch ( uMsg )
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        //----------------------------------------------------------------------
        // Handle resize
        case WM_SIZE:
            Window::_GetCallbackHelper().callSizeChangedCallback( LOWORD( lParam ), HIWORD( lParam ) );
            return 0;

        //----------------------------------------------------------------------
        // Handle Mouse Input
        case WM_LBUTTONDOWN:
            SetCapture( hwnd );
            Window::_GetCallbackHelper().callMouseButtonCallback( KeyCode::LMOUSEBUTTON, KeyAction::DOWN, GetKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_LBUTTONUP:
            ReleaseCapture();
            Window::_GetCallbackHelper().callMouseButtonCallback( KeyCode::LMOUSEBUTTON, KeyAction::UP, GetKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_MBUTTONDOWN:
            SetCapture( hwnd );
            Window::_GetCallbackHelper().callMouseButtonCallback( KeyCode::MMOUSEBUTTON, KeyAction::DOWN, GetKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_MBUTTONUP:
            ReleaseCapture();
            Window::_GetCallbackHelper().callMouseButtonCallback( KeyCode::MMOUSEBUTTON, KeyAction::UP, GetKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_RBUTTONDOWN:
            SetCapture( hwnd );
            Window::_GetCallbackHelper().callMouseButtonCallback( KeyCode::RMOUSEBUTTON, KeyAction::DOWN, GetKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_RBUTTONUP:
            ReleaseCapture();
            Window::_GetCallbackHelper().callMouseButtonCallback( KeyCode::RMOUSEBUTTON, KeyAction::UP, GetKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_MOUSEWHEEL:
            Window::_GetCallbackHelper().callMouseWheelCallback( GET_WHEEL_DELTA_WPARAM( wParam ) > 0 ? 1 : -1 );
            return 0;
        //----------------------------------------------------------------------
        case WM_MOUSEMOVE:
            Window::_GetCallbackHelper().callCursorCallback( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
            return 0;

        //----------------------------------------------------------------------
        // Handle Keyboard Input
        //  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)

        //----------------------------------------------------------------------
        default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
        }
    }

    //----------------------------------------------------------------------
    void Window::processOSMessages()
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
        m_width  = width;
        m_height = height;

        LPCSTR className = "DXWNDClassName";
        if ( not RegisterWindowClass( hInstance, className ) )
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
    void Window::setBorderlessFullscreen(bool enabled)
    {
        static U32 oldResolutionX = 800;
        static U32 oldResolutionY = 600;

        LONG lStyle = GetWindowLong( hwnd, GWL_STYLE );
        if (enabled)
        {
            // Change style to borderless
            lStyle &= ~( WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU );

            // Save old position in case of reverting
            oldResolutionX = m_width;
            oldResolutionY = m_height;

            // Maximize window
            m_width  = GetSystemMetrics( SM_CXSCREEN );
            m_height = GetSystemMetrics( SM_CYSCREEN );
        }
        else
        {
            // Revert style
            lStyle |= (WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU );

            // Change size of the window
            m_width  = oldResolutionX;
            m_height = oldResolutionY;
        }

        SetWindowLongPtr( hwnd, GWL_STYLE, lStyle );
        SetWindowPos( hwnd, NULL, 0, 0, m_width, m_height, SWP_NOZORDER | SWP_NOOWNERZORDER );
    }

    //----------------------------------------------------------------------
    void Window::center() const
    {
        UINT screenResX = GetSystemMetrics( SM_CXSCREEN );
        UINT screenResY = GetSystemMetrics( SM_CYSCREEN );

        UINT x = (UINT) ( ( screenResX * 0.5f ) - ( m_width  * 0.5f ) );
        UINT y = (UINT) ( ( screenResY * 0.5f ) - ( m_height * 0.5f ) );

        SetWindowPos( hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER );
    }

    //----------------------------------------------------------------------
    void Window::setTitle( const char* newTitle ) const
    {
        SetWindowText( hwnd, newTitle );
    }

    //----------------------------------------------------------------------
    void Window::setCursorPosition( U16 x, U16 y ) const
    {
        POINT point { (LONG) x, (LONG) y };
        ClientToScreen( hwnd, &point );
        SetCursorPos( point.x, point.y );
    }

    //----------------------------------------------------------------------
    void Window::centerCursor() const
    {
        I32 width  = GetSystemMetrics( SM_CXCURSOR );
        I32 height = GetSystemMetrics( SM_CYCURSOR );

        I32 centerX = (I32) ( ( m_width  * 0.5f ) - ( width  * 0.5f ) );
        I32 centerY = (I32) ( ( m_height * 0.5f ) - ( height * 0.5f ) );

        setCursorPosition( centerX, centerY );
    }

    //----------------------------------------------------------------------
    void Window::showCursor( bool b ) const
    {
        ShowCursor( b );
    }

    //----------------------------------------------------------------------
    void Window::setCursor( const Path& path ) const
    {
        HCURSOR cursor = (HCURSOR) LoadImageFromFile( path, IMAGE_CURSOR );
        if (cursor == NULL)
        {
           WARN( "Window[Win32]::setCursor(): Could not load cursor '" + path.toString() + "'. Wrong Extension? (.cur)." );
           return;
        }

        // Set the windows class cursor (otherwise WM_SETCURSOR must be properly handled)
        SetClassLongPtr( hwnd, GCLP_HCURSOR, (LONG_PTR) cursor );
    }

    //----------------------------------------------------------------------
    void Window::setIcon( const Path& path ) const
    {
        HICON icon = (HICON) LoadImageFromFile( path, IMAGE_ICON );
        if (icon == NULL)
        {
           WARN( "Window[Win32]::setIcon(): Could not load icon '" + path.toString() + "'. Wrong Extension? (.ico)." );
           return;
        }

        SetClassLongPtr( hwnd, GCLP_HICON, (LONG_PTR) icon );
        SetClassLongPtr( hwnd, GCLP_HICONSM, (LONG_PTR) icon );
    }

    //----------------------------------------------------------------------
    void PrintLastErrorAsString()
    {
        //Get the error message, if any
        DWORD errorMessageID = GetLastError();
        LPSTR messageBuffer = nullptr;
        Size size = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                    NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL );

        printf( "Last Win32 Error: %s", messageBuffer );

        // Free buffer
        LocalFree( messageBuffer );
    }

    //----------------------------------------------------------------------
    HANDLE LoadImageFromFile( const char* path, UINT type )
    {
        HANDLE img = LoadImage( NULL, path, type, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE );
        return img;
    }

    //----------------------------------------------------------------------
    bool RegisterWindowClass( HINSTANCE hInstance, LPCSTR className )
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
    KeyMod GetKeyMod( WPARAM wParam )
    {
        KeyMod mod = KeyModBits::NONE;
        if (wParam & MK_CONTROL)
            mod |= KeyModBits::CONTROL;
        if (wParam & MK_SHIFT)
            mod |= KeyModBits::SHIFT;
        return mod;
    }

} } // end namespaces


#endif