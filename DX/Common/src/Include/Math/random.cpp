#include "random.h"
/**********************************************************************
    class: Random (random.cpp)

    author: S. Hau
    date: March 11, 2018
**********************************************************************/

namespace Math
{

    //---------------------------------------------------------------------------
    std::default_random_engine Random::engine{ std::random_device{}() };

}
