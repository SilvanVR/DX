#pragma once

/**********************************************************************
    class: ILogger (i_logger.hpp)

    author: S. Hau
    date: October 11, 2017

    Interface for a logger subsystem.
**********************************************************************/


#include "Core/i_subsystem.hpp"
#include "Core/Misc/color.h"


namespace Core { namespace Logging {

    //----------------------------------------------------------------------
    #define LOGTYPE_COLOR_DEFAULT       Color::WHITE
    #define LOGTYPE_COLOR_WARNING       Color::YELLOW
    #define LOGTYPE_COLOR_ERROR         Color::RED

    //----------------------------------------------------------------------
    enum LOGLEVEL
    {
        LOG_LEVEL_VERY_IMPORTANT    = 0,
        LOG_LEVEL_IMPORTANT         = 1,
        LOG_LEVEL_NOT_SO_IMPORTANT  = 2,
        LOG_LEVEL_NOT_IMPORTANT     = 3
    };

    //----------------------------------------------------------------------
    enum LOGSOURCE
    {
        LOG_SOURCE_DEFAULT          = 0,
        LOG_SOURCE_MEMORY           = 1,
        LOG_SOURCE_RENDERING        = 2,
        LOG_SOURCE_PHYSICS          = 3
    };

    //**********************************************************************
    // Interface-Class for a Logging-Subsystem
    //**********************************************************************
    class ILogger : public ISubSystem
    {

    private:
        virtual void _Log(LOGSOURCE source, const char* msg, LOGLEVEL logLevel, Color color) const = 0;
        virtual void _Log(LOGSOURCE source, const char* msg, Color color) const = 0;

        virtual void _Warn(LOGSOURCE source, const char* msg, LOGLEVEL logLevel) const = 0;
        virtual void _Error(LOGSOURCE source, const char* msg, LOGLEVEL logLevel) const = 0;

    public:
        //----------------------------------------------------------------------
        // Log different stuff.
        // @Params:
        // "msg/num":   The message to log.
        // "logLevel":  How important the message is. Determines if the message
        //              gets logged or not.
        // "source":    Where the message is coming from.
        // "color":     Color of the message.
        //----------------------------------------------------------------------
        template <class T>
        void log(LOGSOURCE source,
                 T num,
                 LOGLEVEL logLevel = LOG_LEVEL_VERY_IMPORTANT,
                 Color color = Color::WHITE) const
        { 
            _Log( source, TS( num ).c_str(), logLevel, color ); 
        }

        //----------------------------------------------------------------------
        template <class T>
        void log(LOGSOURCE source, T num, Color color) const
        { 
            _Log( source, TS( num ).c_str(), LOG_LEVEL_VERY_IMPORTANT, LOG_SOURCE_DEFAULT, color );
        };

        //----------------------------------------------------------------------
        template <class T>
        void warn(LOGSOURCE source, T num, LOGLEVEL logLevel = LOG_LEVEL_VERY_IMPORTANT) const
        {
            _Warn( source, TS( num ).c_str(), logLevel );
        }

        //----------------------------------------------------------------------
        template <class T>
        void error(LOGSOURCE source, T num, LOGLEVEL logLevel = LOG_LEVEL_VERY_IMPORTANT) const
        {
            _Error( source, TS( num ).c_str(), logLevel );
        }

        //----------------------------------------------------------------------
        // TEMPLATE SPECIALIZATIONS
        //----------------------------------------------------------------------
        template <>
        void log<const char*>(LOGSOURCE source, const char* msg, LOGLEVEL logLevel, Color color) const {
            _Log( source, msg, logLevel, color );
        }
        template <>
        void log<StringID>(LOGSOURCE source, StringID msg, LOGLEVEL logLevel, Color color) const {
            _Log( source, msg.str, logLevel, color );
        }
        template <>
        void log<String>(LOGSOURCE source, String msg, LOGLEVEL logLevel, Color color) const {
            _Log( source, msg.c_str(), logLevel, color );
        }

        //----------------------------------------------------------------------
        template <>
        void log<const char*>(LOGSOURCE source, const char* msg, Color color) const {
            _Log( source, msg, LOG_LEVEL_VERY_IMPORTANT, color );
        }
        template <>
        void log<StringID>(LOGSOURCE source, StringID msg, Color color) const {
            _Log( source, msg.str, LOG_LEVEL_VERY_IMPORTANT, color );
        }
        template <>
        void log<String>(LOGSOURCE source, String msg, Color color) const {
            _Log( source, msg.c_str(), LOG_LEVEL_VERY_IMPORTANT, color );
        }

        //----------------------------------------------------------------------
        template <>
        void warn<const char*>(LOGSOURCE source, const char* msg, LOGLEVEL logLevel) const {
            _Log( source, msg, logLevel, LOGTYPE_COLOR_WARNING );
        }
        template <>
        void warn<StringID>(LOGSOURCE source, StringID msg, LOGLEVEL logLevel) const {
            _Log( source, msg.str, logLevel, LOGTYPE_COLOR_WARNING );
        }
        template <>
        void warn<String>(LOGSOURCE source, String msg, LOGLEVEL logLevel) const {
            _Log( source, msg.c_str(), logLevel, LOGTYPE_COLOR_WARNING );
        }

        //----------------------------------------------------------------------
        template <>
        void error<const char*>(LOGSOURCE source, const char* msg, LOGLEVEL logLevel) const {
            _Log( source, msg, logLevel, LOGTYPE_COLOR_ERROR );
        }
        template <>
        void error<StringID>(LOGSOURCE source, StringID msg, LOGLEVEL logLevel) const {
            _Log( source, msg.str, logLevel, LOGTYPE_COLOR_ERROR );
        }
        template <>
        void error<String>(LOGSOURCE source, String msg, LOGLEVEL logLevel) const {
            _Log( source, msg.c_str(), logLevel, LOGTYPE_COLOR_ERROR );
        }

    };


} } // end namespaces
