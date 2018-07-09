#include "string_utils.h"
/**********************************************************************
    class: None (string_utils.cpp)

    author: S. Hau
    date: October 20, 2017
**********************************************************************/

namespace StringUtils {

    //----------------------------------------------------------------------
    String toLower(const String& s)
    {
        String res = s;
        std::transform( res.begin(), res.end(), res.begin(), ::tolower );
        return res;
    }

    //----------------------------------------------------------------------
    String toUpper(const String& s)
    {
        String res = s;
        std::transform( res.begin(), res.end(), res.begin(), ::toupper );
        return res;
    }

    //----------------------------------------------------------------------
    void ltrim( String& s ) {
        s.erase( s.begin(), std::find_if( s.begin(), s.end(), []( int ch ) {
            return !isspace( ch );
        } ) );
    }

    //----------------------------------------------------------------------
    void rtrim( String& s ) {
        s.erase( std::find_if( s.rbegin(), s.rend(), []( int ch ) {
            return !isspace(ch);
        }).base(), s.end());
    }

    //----------------------------------------------------------------------
    void trim( String &s ) {
        ltrim(s);
        rtrim(s);
    }

    //----------------------------------------------------------------------
    String substringBetween( const String& str, const char first, const char second )
    {
        Size firstOccurrence = str.find_first_of( first );

        if (firstOccurrence != String::npos)
        {
            Size secondOccurence = str.find_first_of( second, firstOccurrence + 1 );

            if (secondOccurence != String::npos)
            {
                Size numChars = ( secondOccurence - firstOccurrence ) - 1;
                return str.substr(firstOccurrence + 1, numChars);
            }
        }

        return "";
    }

    //----------------------------------------------------------------------
    String substringBetweenWithBounds( const String& str, const char first, const char second )
    {
        Size firstOccurrence = str.find_first_of( first );

        if (firstOccurrence != String::npos)
        {
            Size secondOccurence = str.find_first_of( second, firstOccurrence + 1 );

            if (secondOccurence != String::npos)
            {
                Size numChars = (secondOccurence - firstOccurrence) + 1;
                return str.substr( firstOccurrence, numChars );
            }
        }

        return "";
    }

    //----------------------------------------------------------------------
    std::vector<String> splitString( const String& str, const char delim )
    {
        String s = str;
        for (Size i = 0; i < str.length(); i++)
        {
            if (s[i] == delim)
                s[i] = ' ';
        }

        std::vector<String> array;
        IStringStream ss( s );

        String temp;
        while (ss >> temp)
            array.push_back( temp );

        return array;
    }

    //----------------------------------------------------------------------
    String format( CString str, ... )
    {
        char buffer[256]{};
        va_list args;
        va_start( args, str );
        vsnprintf( buffer, 256, str, args );
        va_end( args );

        return String( buffer );
    }

    //**********************************************************************
    // IStringStream
    //**********************************************************************
    IStringStream::IStringStream(const String& str)
        : m_str(str), m_readPos(0), m_failed(false)
    {}

    //----------------------------------------------------------------------
    String IStringStream::_NextWord()
    {
        Size nextNonSpace = m_str.find_first_not_of( " \n\r\t", m_readPos );

        if (nextNonSpace != String::npos)
        {
            Size end = m_str.find_first_of( " \n\r\t", nextNonSpace );
            m_readPos = end;

            if (end == String::npos)
                return m_str.substr( nextNonSpace, (m_str.size() - nextNonSpace) );

            String nextWord = m_str.substr( nextNonSpace, (end - nextNonSpace) );

            return nextWord;
        }

        m_readPos = nextNonSpace;
        return "";
    }

    //----------------------------------------------------------------------
    char IStringStream::peekNextChar()
    {
        Size nextCharPos = m_str.find_first_not_of( " \n\r\t", m_readPos );

        if (nextCharPos == String::npos)
            return ' ';

        return m_str[nextCharPos];
    }

    //----------------------------------------------------------------------
    String IStringStream::nextLine()
    {
        Size nextNewLine = m_str.find_first_of( "\n", m_readPos );

        if (nextNewLine != String::npos)
        {
            String line = m_str.substr( m_readPos, (nextNewLine - m_readPos) );

            m_readPos = nextNewLine + 1;
            nextNewLine = m_str.find_first_of( "\n", m_readPos );
            if (nextNewLine == String::npos)
                m_readPos = String::npos;

            return line;
        }

        return "";
    }

    //----------------------------------------------------------------------
    bool IStringStream::_ParseAsFloat( const char* str, F64* val )
    {
        char* endPtr;
        (*val) = strtod( str, &endPtr );

        return (endPtr != str);
    }

    //----------------------------------------------------------------------
    bool IStringStream::_ParseAsIntegral( const char* str, I64* val )
    {
        char* endPtr;
        (*val) = strtoll( str, &endPtr, 0 );

        return (endPtr != str);
    }

}