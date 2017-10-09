#pragma once

/**********************************************************************
    class: None (macros.hpp)

    author: S. Hau
    date: October 3, 2017

    Common macros used throughout whole program.
**********************************************************************/

#define TS(s)                   std::to_string(s)
#define not                     !

#ifdef _DEBUG
    #define NEW                 new(__FILE__, __LINE__)
    #define ASSERT(exp)         assert(exp)
#else
    #define NEW                 new
    #define ASSERT(exp)         assert(exp) // TODO: Replace by nothing if everything works fine :-)
#endif