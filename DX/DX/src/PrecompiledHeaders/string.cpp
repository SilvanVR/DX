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
    - Optimize for release builds. Do i need the c-style string?
**********************************************************************/

//#ifdef _DEBUG
    //static std::map<U32, const char*> gStringIdTable;
//#endif

//----------------------------------------------------------------------
// Forward Declarations
//----------------------------------------------------------------------

//U32         internString(const char* str);
//const char* externString(StringID sid);
U32 hash(const char* str);

//----------------------------------------------------------------------
StringID::StringID( const char* str )
    : id( hash( str ) ), str( str )
{}

//----------------------------------------------------------------------
U32 hash( const char* str )
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
//const char* StringID::toString()
//{
//    return externString( *this );
//}

//----------------------------------------------------------------------
//U32 internString( const char* str )
//{
//    U32 sid = hash( str );
//
////#ifdef _DEBUG
//    auto it = gStringIdTable.find( sid );
//
//    if (it == gStringIdTable.end())
//    {
//        // This string has not yet been added to the
//        // table. Add it, being sure to copy it in case
//        // the original was dynamically allocated and
//        // might later be freed. 
//        // Normally you have to free this memory manually later, but because it is
//        // global anyway, it's OK to let it deleted from the OS when the program terminates.
//        gStringIdTable[sid] = _strdup(str);
//    }
////#endif
//
//    return sid;
//}
//
////----------------------------------------------------------------------
//const char* externString( StringID sid )
//{
////#ifdef _DEBUG
//    auto it = gStringIdTable.find( sid.id );
//    if (it != gStringIdTable.end())
//    {
//        return gStringIdTable[ sid.id ];
//    }
//    ASSERT( false && "Given StringID does not exist.");
//    return "";
////#else
//    // This should actually never be called, so return a very weird string
//    //static const char* releaseString = "W31RD";
//    //return releaseString;
////#endif
//}


