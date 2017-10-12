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
    void ConsoleLogger::_Log( ELogChannel channel, const char* msg, ELogLevel logLevel, Color color )
    {
        if ( _CheckLogLevel( logLevel ) || _Filterchannel( channel ) )
            return;

        if ( m_dumpToDisk )
            _StoreLogMessage( channel, msg, logLevel );

        m_console.setColor( LOGTYPE_COLOR_CHANNEL );

        const char* preface = _GetchannelAsString( channel );
        if( preface != "" )
            m_console.write( preface );

        m_console.setColor( color );
        m_console.writeln( msg );
        m_console.setColor( m_defaultColor );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Log( ELogChannel channel, const char* msg, Color color )
    {
        _Log( Logging::LOG_CHANNEL_DEFAULT, msg, Logging::LOG_LEVEL_VERY_IMPORTANT, color );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Warn( ELogChannel channel, const char* msg, ELogLevel logLevel)
    {
        _Log( channel, msg, logLevel, LOGTYPE_COLOR_WARNING );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Error( ELogChannel channel, const char* msg, ELogLevel logLevel )
    {
        _Log( channel, msg, logLevel, LOGTYPE_COLOR_ERROR );

        #ifdef _WIN32
            __debugbreak();
        #elif
            ASSERT( false );
        #endif
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_WriteToFile(const char* fileName, bool append)
    {
        // @TODO
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_StoreLogMessage(ELogChannel channel, const char* msg, ELogLevel logLevel)
    {
        _LOGMESSAGE logMessage;
        logMessage.message  = StringID(msg);
        logMessage.level    = logLevel;
        logMessage.channel  = channel;

        // Store message in a ringbuffer.


        // Flush to disk if buffer is full and enabled.
        bool isFull = false;
        if (isFull)
        {
            _WriteToFile("", true);
        }
    }

} }