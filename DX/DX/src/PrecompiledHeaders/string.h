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

using String    = std::string;
using StringID  = U32;

#define SID(str) internString(str)
#define IDS(str) externString(str)

//----------------------------------------------------------------------
// Converts a string to an unsigned integer using a hash function and
// stores the String<->ID in a table to reverse it if necessary.
// @Return: 
//      Hashed ID of given string.
//----------------------------------------------------------------------
StringID internString(const char* str);

//----------------------------------------------------------------------
// Converts a StringID back to the corresponding string.
// @Return: 
//      String which was given to internString() when calculated SID.
//----------------------------------------------------------------------
const char* externString(StringID sid);