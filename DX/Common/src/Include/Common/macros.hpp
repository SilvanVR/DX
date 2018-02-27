#pragma once

/**********************************************************************
    class: None (macros.hpp)

    author: S. Hau
    date: October 3, 2017

    Common macros used throughout whole program.
**********************************************************************/

#include <assert.h>

#define INVALID_CALLBACK_ID         0
#define TS(s)                       std::to_string(s)
#define not                         !
#define and                         &&
#define or                          ||
#define SAFE_DELETE(x)              { delete x; x = nullptr; }

#ifdef _DEBUG
    #define NEW                     new(__FILE__, __LINE__)
    #define ASSERT(exp)             assert(exp)
#else
    #define NEW                     new
    #define ASSERT(exp)             (exp)
#endif

//----------------------------------------------------------------------
#define BIND_THIS_FUNC_0_ARGS(F)    std::bind( F, this )
#define BIND_THIS_FUNC_1_ARGS(F)    std::bind( F, this, std::placeholders::_1 )
#define BIND_THIS_FUNC_2_ARGS(F)    std::bind( F, this, std::placeholders::_1, std::placeholders::_2 )
#define BIND_THIS_FUNC_3_ARGS(F)    std::bind( F, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )