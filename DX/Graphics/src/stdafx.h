#pragma once
/**********************************************************************
    class: None (stdafx.h)

    author: S. Hau
    date: February 27, 2018

    Use of precompiled header files to speed up compilation.
    Include here all header files which rarely change AND are often
    used to gain a huge performance boost in compilation.
**********************************************************************/

#ifdef _WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef ERROR
    #include <DirectXMath.h>
#endif

#include "common.hpp"
#include "Misc/color.h"

#pragma comment( lib, "D3D11" )
#pragma comment( lib, "d3dcompiler" )