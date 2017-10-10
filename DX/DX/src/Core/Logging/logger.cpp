#include "logger.h"

/**********************************************************************
    class: Logger (logger.cpp)

    author: S. Hau
    date: October 10, 2017
**********************************************************************/

#include "Utils/utils.h"

namespace Core { namespace Logging {

    void Logger::log( const char* msg )
    {
        console.setColor(Color::RED);
        console.writeln( msg );
    }

    void Logger::log( void* addr )
    {
        log( Utils::memoryAddressToString( addr ) );
    }

    void Logger::log( StringID id )
    {
        log( id.str );
    }

    void Logger::log( String msg )
    {
        log( msg.c_str() );
    }

    void Logger::log( I64 num )
    {
        log( TS(num) );
    }


} }