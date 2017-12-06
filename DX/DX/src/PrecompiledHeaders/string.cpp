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
       => YES, in order to dump StringIDs as string to disk.
**********************************************************************/

#include <codecvt>
#include <locale>


// Table which maps [HASH <-> STRING]
HashMap<U32, const char*> gStringIdTable;


//----------------------------------------------------------------------
// Forward Declarations
//----------------------------------------------------------------------

U32         internString(const char* str, bool addToTable);
const char* externString(StringID sid);
U32         hash(const char* str);

//----------------------------------------------------------------------
StringID::StringID( const char* s, bool addToTable )
    : id( internString( s, addToTable ) )
{
    if( addToTable )
        str = c_str();
}

//----------------------------------------------------------------------
const char* StringID::c_str() const
{
    return externString( *this );
}

//----------------------------------------------------------------------
String StringID::toString() const
{
    return String( c_str() );
}

//----------------------------------------------------------------------
U32 internString( const char* str, bool addToTable )
{
    U32 sid = hash( str );

    if (addToTable)
    {
        auto it = gStringIdTable.find( sid );
        if (it == gStringIdTable.end())
        {
            // This string has not yet been added to the table. Add it, being sure to copy it 
            // in case the original was dynamically allocated and might later be freed. 
            // Normally you have to free this memory manually later, but because it is
            // global anyway, it's OK to let it deleted from the OS when the program terminates.
            gStringIdTable[sid] = _strdup( str );
        }
    }

    return sid;
}

//----------------------------------------------------------------------
const char* externString( StringID sid )
{
    auto it = gStringIdTable.find( sid.id );
    if ( it != gStringIdTable.end() )
    {
        return gStringIdTable[sid.id];
    }
    ASSERT( false && "Given StringID does not exist.");
    return "";
}

//----------------------------------------------------------------------
U32 hash(const char* str)
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
WString ConvertToWString( const String& s )
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    WString wide = converter.from_bytes(s);
    return wide;
}

//----------------------------------------------------------------------
String ConvertToString( const WString& s )
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    String narrow = converter.to_bytes(s);
    return narrow;
}

