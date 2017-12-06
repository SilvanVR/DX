#pragma once
/**********************************************************************
    class: None (D3D11.hpp)

    author: S. Hau
    date: December 2, 2017

    Include for D3D11 and some common stuff to D3D11 applications. 
    The reason not to include D3D11 in the PCH is, that it increases 
    build time about ~0.5 - 1sec and i don't want to have all the 
    D3D11 stuff in the global namespace.
**********************************************************************/

#include <d3d11_4.h>
#include "locator.h" /* Logging... */

//----------------------------------------------------------------------
extern ID3D11Device*           g_pDevice;
extern ID3D11DeviceContext*    g_pImmediateContext;


//----------------------------------------------------------------------
#define SAFE_RELEASE(com) if( com ) { com->Release(); com = nullptr; }


#ifdef _DEBUG
    #include <comdef.h> /* _com_error */
    #define HR(x) \
        if ( FAILED( x ) ) { \
            _com_error err( x );\
            LPCTSTR errMsg = err.ErrorMessage();\
            ERROR_RENDERING( String( "D3D11Renderer: @" ) + __FILE__ + ", line " + TS(__LINE__) + ". "\
                             "Function: " + #x + ". Reason: " + errMsg );\
        }
#else
    #define HR(x) (x)
#endif
