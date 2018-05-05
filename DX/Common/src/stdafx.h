#pragma once
/**********************************************************************
    class: None (stdafx.h)

    author: S. Hau
    date: February 27, 2018

    Use of precompiled header files to speed up compilation.
    Include here all header files which rarely change AND are often
    used to gain a huge performance boost in compilation.
**********************************************************************/

#include <functional>
#include <algorithm>
#include <array>
#include <mutex>
#include <queue>

#include "Common/data_types.hpp"
#include "Common/macros.hpp"
#include "Common/string.h"

#ifdef  _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef ERROR
#endif //  _WIN32
