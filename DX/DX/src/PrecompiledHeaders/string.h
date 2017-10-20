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

#include <string>

using String = std::string;

#define SID(str) StringID( str )

//----------------------------------------------------------------------
// Returns the string-tables which stores the connection of id <-> string
//----------------------------------------------------------------------
std::map<U32, const char*>& getStringTable();

//----------------------------------------------------------------------
// Represents a string as a number.
//----------------------------------------------------------------------
struct StringID
{
    const char* str;
    U32 id;

    StringID() 
        : id(0), str(nullptr) {}

    //----------------------------------------------------------------------
    // Converts a string to an unsigned integer using a hash function and
    // stores the String<->ID to reverse it if desired.
    //----------------------------------------------------------------------
    explicit StringID(const char* str);

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