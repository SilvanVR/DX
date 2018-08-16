#pragma once
/**********************************************************************
    class: None

    author: S. Hau
    date: August 15, 2018

    Extents the standard library for cleaner code or more functions.
**********************************************************************/

#include <algorithm>

namespace estl {

    template<typename T, typename T2>
    bool find(const T& cont, const T2& val) { return std::find(cont.begin(), cont.end(), val) != cont.end(); }

    template<typename T, typename T2>
    bool find_if(const T& cont, const T2& pred) { return std::find_if(cont.begin(), cont.end(), pred) != cont.end(); }

} // End namespaces