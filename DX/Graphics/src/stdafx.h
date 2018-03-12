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
#include "pch.hpp"
#include "Common/color.h"
#include "Include/Graphics/enums.hpp"

#ifdef _WIN32
    #include <DirectXMath.h>
#endif

#pragma comment( lib, "D3D11" )
#pragma comment( lib, "d3dcompiler" )
#pragma comment( lib, "dxguid" )