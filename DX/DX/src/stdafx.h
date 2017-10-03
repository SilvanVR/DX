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
#include <vector>
#include <list>
#include <map>
#include <set>

#include "PrecompiledHeaders/data_types.hpp"
#include "PrecompiledHeaders/macros.hpp"

#ifdef _WIN32
    #include <Windows.h>
#elif
    #include <chrono>
#endif
