#include "console_logger.h"

/**********************************************************************
    class: Logger (logger.cpp)

    author: S. Hau
    date: October 10, 2017

    @Considerations:
      - Class which holds the message buffer and writes automatically
        to file when buffer is full.
**********************************************************************/

#include "Core/OS/PlatformTimer/platform_timer.h"
#include "Core/OS/FileSystem/file.h"

namespace Core { namespace Logging {

    //----------------------------------------------------------------------
    void ConsoleLogger::init()
    {
#ifdef _DEBUG
        const char* configuration = "_debug";
#else
        const char* configuration = "";
#endif
        // Guaranteed unique filename per run
        m_logFilePath = "/logs/" + OS::PlatformTimer::getCurrentTime().toString() + configuration + ".log";

        // Replace ":" characters (Windows does not allow those in a filename)
        std::replace( m_logFilePath.begin(), m_logFilePath.end(), ':', '_' );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::shutdown()
    {
        // Write logged stuff to file if desired
        if (m_dumpToDisk)
        {
            _DumpToDisk();

            String msg = " >> Written log to file '" + m_logFilePath + "'";
            _LOG( ELogType::INFO, LOG_CHANNEL_DEFAULT, msg.c_str(), ELogLevel::VERY_IMPORTANT, Color::GREEN );
        }
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_LOG( ELogType logType, ELogChannel channel, const char* msg, ELogLevel logLevel, Color color )
    {
        if ( _CheckLogLevel( logLevel ) || _Filterchannel( channel ) )
            return;

        if ( m_dumpToDisk )
            _StoreLogMessage( logType, channel, msg, logLevel );

        m_console.setColor( color );
        {
            const char* type = _GetLogTypeAsString( logType );
            if (type != "")
                m_console.write( type );

            const char* preface = _GetChannelAsString( channel );
            if (preface != "")
                m_console.write( preface );

            m_console.writeln( msg );
        }
        m_console.setColor( m_defaultColor );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Log( ELogChannel channel, const char* msg, ELogLevel logLevel, Color color )
    {
        _LOG( ELogType::INFO, channel, msg, logLevel, color );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Log( ELogChannel channel, const char* msg, Color color )
    {
        _LOG( ELogType::INFO, LOG_CHANNEL_DEFAULT, msg, ELogLevel::VERY_IMPORTANT, color );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Warn( ELogChannel channel, const char* msg, ELogLevel logLevel)
    {
        _LOG( ELogType::WARNING, channel, msg, logLevel, LOGTYPE_COLOR_WARNING );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_Error( ELogChannel channel, const char* msg, ELogLevel logLevel )
    {
        _LOG( ELogType::ERROR, channel, msg, logLevel, LOGTYPE_COLOR_ERROR );

        if (m_dumpToDisk)
            _DumpToDisk();

        #ifdef _WIN32
            __debugbreak();
        #elif
            ASSERT( false );
        #endif
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_StoreLogMessage( ELogType logType, ELogChannel channel, const char* msg, ELogLevel logLevel )
    {
        // Write stuff to the message-buffer. Always flush before if necessary.
        const char* type = _GetLogTypeAsString( logType );
        if ( type != "" )
            _WriteToBuffer( type );

        const char* preface = _GetChannelAsString( channel );
        if ( preface != "" )
            _WriteToBuffer( preface );

        _WriteToBuffer( msg );

        // And finally the newline character
        if ( not m_messageBuffer.hasEnoughPlace( 1 ) )
            _DumpToDisk();
        m_messageBuffer.write( '\n' );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_WriteToBuffer( const char* msg )
    {
        Size len = strlen( msg );
        if ( not m_messageBuffer.hasEnoughPlace( len ) )
            _DumpToDisk();

        m_messageBuffer.write( msg );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_DumpToDisk()
    {
        static OS::TextFile logFile( m_logFilePath.c_str() );

        logFile.write( m_messageBuffer.data(), m_messageBuffer.size() );
        logFile.flush();

        m_messageBuffer.clear();
    }

} }