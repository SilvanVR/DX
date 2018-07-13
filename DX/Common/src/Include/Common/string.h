#pragma once
/**********************************************************************
    class: String (string.h)

    author: S. Hau
    date: October 3, 2017

    Encapsulates the STL string so it can theoretically be replaced
    by a custom implementation.
    Adds additionally the support for Hashed Strings.
    Note:
     - When comparing hashed strings in eg. a function,
       make the compared string static, so it gets interned only once,
       when the function is called for the first time.
**********************************************************************/

#pragma warning( disable : 4307) // '+': integral constant overflow. Occurs often with constexpr stringids

#include "data_types.hpp"
#include <string>

#define SID(str)        StringID( str, true )
#define SID_NO_ADD(str) StringID( str, false )

using String    = std::string;
using WString   = std::wstring;

WString ConvertToWString(const String& s);
String ConvertToString(const WString& s);

constexpr U32 StringHash(const char* str)
{
    // Jenkins's one-at-a-time. Implementation from https://en.wikipedia.org/wiki/Jenkins_hash_function
    U32 hash = 0;
    while (U32 c = *str++)
    {
        hash += c;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

//----------------------------------------------------------------------
// Represents a string as a number.
//----------------------------------------------------------------------
struct StringID
{
    const char* str = nullptr;
    U32 id;

    StringID() 
        : id(0), str(nullptr) {}

    //----------------------------------------------------------------------
    // Converts a string to an unsigned integer using a hash function and
    // stores the String<->ID to reverse it if desired.
    //----------------------------------------------------------------------
    explicit StringID(const char* str, bool addToTable);

    //----------------------------------------------------------------------
    // Constructor usable for compile time evaluation of string ids
    //----------------------------------------------------------------------
    constexpr explicit StringID(const char* str)
        : id(StringHash(str))
    {}

    //----------------------------------------------------------------------
    bool operator <  (const StringID& other) const { return id < other.id; }
    bool operator >  (const StringID& other) const { return id > other.id; }
    bool operator <= (const StringID& other) const { return id <= other.id; }
    bool operator >= (const StringID& other) const { return id >= other.id; }
    bool operator == (const StringID& other) const { return id == other.id; }
    bool operator != (const StringID& other) const { return id != other.id; }

    //----------------------------------------------------------------------
    // Returns the corresponding c-style array for this string
    //----------------------------------------------------------------------
    const char* c_str() const;

    //----------------------------------------------------------------------
    // Converts a StringID back to the corresponding string.
    //----------------------------------------------------------------------
    String toString() const;
};
