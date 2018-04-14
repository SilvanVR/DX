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
#include <vector>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>
#include <map>

#include "pch.hpp"

#ifdef _WIN32
    #include <DirectXMath.h>
    #include <xaudio2.h>
    #include <x3daudio.h>
    
    #pragma comment(lib,"xaudio2.lib")
#endif
