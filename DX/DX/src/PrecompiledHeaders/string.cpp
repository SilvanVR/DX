#include "string.h"

/**********************************************************************
    class: String (string.cpp)

    author: S. Hau
    date: October 3, 2017

    Implementation of the hashing algorithm for a string.
    @Considerations:
    - Two approaches possible for converting IDs back to a string:
        1.) HashTable for storing string <-> ids or
        2.) StringID class which stores pointer to c-style array
    - Optimize for release builds. Do i need the table?
**********************************************************************/

//#ifdef _DEBUG
    static std::map<StringID, const char*> gStringIdTable;
//#endif

StringID hash(const char* str);

//----------------------------------------------------------------------
StringID internString(const char* str)
{
    StringID sid = hash( str );

//#ifdef _DEBUG
    auto it = gStringIdTable.find( sid );

    if (it == gStringIdTable.end())
    {
        // This string has not yet been added to the
        // table. Add it, being sure to copy it in case
        // the original was dynamically allocated and
        // might later be freed. 
        // Normally you have to free this memory manually later, but because it is
        // global anyway, it's OK to let it deleted from the OS when the program terminates.
        gStringIdTable[sid] = _strdup(str);
    }
//#endif

    return sid;
}

//----------------------------------------------------------------------
String externString(StringID sid)
{
//#ifdef _DEBUG
    auto it = gStringIdTable.find( sid );
    if (it != gStringIdTable.end())
    {
        return gStringIdTable[sid];
    }
    return "";
//#else
    // This should actually never be called, so return a very weird string
    //static const char* releaseString = "W31RD";
    //return releaseString;
//#endif
}

//----------------------------------------------------------------------
StringID hash(const char* str)
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