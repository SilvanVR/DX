#pragma once

/**********************************************************************
    class: None (pch.hpp)

    author: S. Hau
    date: February 27, 2018

    This file should be included in every project, so Data-Types are
    everywhere the same and important macros are exposed to everyone.
    At best you include this file in the precompiler header file and
    configure under <Properties> to include that file everywhere.
**********************************************************************/

#include "Common/data_types.hpp"
#include "Common/macros.hpp"
#include "Common/string.h"
#include "Logging/logging.h"
#include "Math/dxmath_wrapper.h"

#ifdef  _WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef ERROR
#endif //  _WIN32
