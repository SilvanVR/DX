#pragma once

/**********************************************************************
    class: None (data_types.hpp)

    author: S. Hau
    date: September 28, 2017

    Common data-type definitions used throughout the whole program.
**********************************************************************/

#include <map>
#include <vector>

using U8 = unsigned __int8;
using I8 = __int8;
using Byte = U8;
using Char = char;
using WChar = wchar_t;

typedef const char* CString;

using U16 = unsigned __int16;
using I16 = __int16;

using U32 = unsigned __int32;
using I32 = __int32;

using U64 = unsigned __int64;
using I64 = __int64;

using F32 = float;
using F64 = double;

using CallbackID = U64;

using Size = std::size_t;

using Hash = Size;

template <typename T, typename T2>
using HashMap = std::map<T, T2>;

template <typename T>
using ArrayList = std::vector<T>;

enum class DataType
{
    Unknown = -1,
    Boolean,
    Char,
    Float,
    Int,
    Double,
    Vec2,
    Vec3,
    Vec4,
    Matrix,
    Color,
    String,
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCubemap,
    Struct
};
