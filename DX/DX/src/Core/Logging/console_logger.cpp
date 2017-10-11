#include "console_logger.h"

/**********************************************************************
    class: Logger (logger.cpp)

    author: S. Hau
    date: October 10, 2017
**********************************************************************/

#include "Utils/utils.h"

namespace Core { namespace Logging {

    //----------------------------------------------------------------------
    void ConsoleLogger::init()
    {
        m_console.setColor( Color::RED );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::shutdown()
    {
        // @TODO:
        // Write logged stuff to file
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::log( const char* msg ) const
    {
        m_console.writeln( msg );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::log( void* addr ) const
    {
        log( Utils::memoryAddressToString( addr ) );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::log( StringID id ) const
    {
        log( id.str );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::log( String msg ) const
    {
        log( msg.c_str() );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::log( I64 num ) const
    {
        log( TS(num) );
    }


} }