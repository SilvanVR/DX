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
       // Nothing to do yet!
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::shutdown()
    {
        // @TODO:
        // Write logged stuff to file if desired
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Log( LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel, Color color ) const
    {
        if ( _CheckLogLevel( logLevel ) || _Filterchannel( channel ) )
            return;

        m_console.setColor( LOGTYPE_COLOR_channel );

        String preface = _GetchannelAsString( channel ) + " ";
        if( not preface.empty() )
            m_console.write( preface.c_str() );

        m_console.setColor( color );
        m_console.writeln( msg );
        m_console.setColor( getDefaultColor() );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Log( LOGCHANNEL channel, const char* msg, Color color ) const
    {
        _Log( Logging::LOG_CHANNEL_DEFAULT, msg, Logging::LOG_LEVEL_VERY_IMPORTANT, color );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Warn( LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel ) const
    {
        _Log( channel, msg, logLevel, LOGTYPE_COLOR_WARNING );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Error( LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel ) const
    {
        _Log( channel, msg, logLevel, LOGTYPE_COLOR_ERROR );
    }


} }