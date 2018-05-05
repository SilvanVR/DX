#pragma once
/**********************************************************************
    class: None (stdafx.h)

    author: S. Hau
    date: February 27, 2018

    Use of precompiled header files to speed up compilation.
    Include here all header files which rarely change AND are often
    used to gain a huge performance boost in compilation.
**********************************************************************/

#include <memory>
#include <queue>

#include "Common/data_types.hpp"
#include "Common/macros.hpp"
#include "Common/string.h"
#include "Common/color.h"
#include "Math/dxmath_wrapper.h"

#ifdef _WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef ERROR
    #include <DirectXMath.h>

    #pragma comment( lib, "D3D11" )
    #pragma comment( lib, "d3dcompiler" )
    #pragma comment( lib, "dxguid" )
#endif
