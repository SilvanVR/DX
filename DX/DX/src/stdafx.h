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
#include <functional>
#include <algorithm>
#include <assert.h>
#include <cstddef>
#include <vector>
#include <memory>
#include <mutex>
#include <queue>
#include <array>
#include <map>

#include "PrecompiledHeaders/forward_declarations.hpp"
#include "PrecompiledHeaders/data_types.hpp"
#include "PrecompiledHeaders/macros.hpp"
