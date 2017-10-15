#include "console_logger.h"

/**********************************************************************
    class: Logger (logger.cpp)

    author: S. Hau
    date: October 10, 2017
**********************************************************************/

#include "Utils/utils.h"
#include "Core/OS/PlatformTimer/platform_timer.h"

#include "Core/OS/FileSystem/file.h"
#include <fstream>

namespace Core { namespace Logging {

    //----------------------------------------------------------------------
    static String s_logFilePath;

    //----------------------------------------------------------------------
    void ConsoleLogger::init()
    {
        // @ TODO: Virtual-Path Resolve
        s_logFilePath = "res/logs/" + OS::PlatformTimer::getCurrentTime().toString() + ".log";

        // Windows does not allow ":" characters in a filename
        std::replace( s_logFilePath.begin(), s_logFilePath.end(), ':', '_' );

        m_messageBuffer.reserve( MESSAGE_BUFFER_CAPACITY );
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::shutdown()
    {
        // Write logged stuff to file if desired
        if (m_dumpToDisk)
        {
            _DumpToDisk();
        }
        m_messageBuffer.clear();
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
    void ConsoleLogger::_DumpToDisk()
    {
        //@TODO Virtual Path!!
        //OS::File logFile( s_logFilePath.c_str(), true );
        /*for (auto& message : m_messageBuffer)
        {
            const char* preface = _GetchannelAsString( message.channel );
            if (preface != "")
                logFile.write( preface );

            logFile.write( message.message.c_str() );
            logFile.write( "\n" );
        }*/

        std::ofstream logFile( s_logFilePath, std::ios::out | std::ios::app );
        for (auto& message : m_messageBuffer)
        {
            const char* preface = _GetchannelAsString(message.channel);
            if (preface != "")
                logFile << preface;

            logFile << message.message;
            logFile << "\n";
        }

        m_messageBuffer.clear();
    }

    //----------------------------------------------------------------------
    void ConsoleLogger::_StoreLogMessage(ELogChannel channel, const char* msg, ELogLevel logLevel)
    {
        _LOGMESSAGE logMessage;
        logMessage.message = String( msg );
        logMessage.channel = channel;

        // Store message in a buffer.
        m_messageBuffer.push_back( logMessage );

        // Flush to disk if buffer is full.
        if ( m_messageBuffer.size() == MESSAGE_BUFFER_CAPACITY )
        {
            _DumpToDisk();
        }
    }

} }