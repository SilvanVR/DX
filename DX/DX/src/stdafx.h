#pragma once

/**********************************************************************
    class: None (stdafx.h)

    author: S. Hau
    date: October 3, 2017

    Use of precompiled header files to speed up compilation.
    Include here all header files which rarely change AND are often 
    used to gain a huge performance boost in compilation.
**********************************************************************/

#include <unordered_map>
#include <assert.h>
#include <cstddef>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>

#include "Common/data_types.hpp"
#include "Common/macros.hpp"
#include "Common/string.h"
#include "Logging/logging.h"
#include "Math/dxmath_wrapper.h"

#ifdef _WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef ERROR
    #include <DirectXMath.h>
    #include <xaudio2.h>
    #include <x3daudio.h>

    #pragma comment(lib,"xaudio2.lib")
#endif
