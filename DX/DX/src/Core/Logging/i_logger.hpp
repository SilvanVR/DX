#pragma once

/**********************************************************************
    class: ILogger (i_logger.hpp)

    author: S. Hau
    date: October 11, 2017

    Interface for a logger subsystem.
**********************************************************************/


#include "Core/i_subsystem.hpp"
#include "Core/Misc/color.h"
#include "Core/Misc/layer_mask.hpp"

namespace Core { namespace Logging {

    //----------------------------------------------------------------------
    #define LOGTYPE_COLOR_DEFAULT       Color::WHITE
    #define LOGTYPE_COLOR_WARNING       Color::YELLOW
    #define LOGTYPE_COLOR_ERROR         Color::RED
    #define LOGTYPE_COLOR_channel        Color::VIOLET

    //----------------------------------------------------------------------
    enum LOGLEVEL
    {
        LOG_LEVEL_VERY_IMPORTANT    = 0x00,
        LOG_LEVEL_IMPORTANT         = 0x01,
        LOG_LEVEL_NOT_SO_IMPORTANT  = 0x02,
        LOG_LEVEL_NOT_IMPORTANT     = 0x04,
        LOG_LEVEL_ALL               = LOG_LEVEL_NOT_IMPORTANT
    };

    //----------------------------------------------------------------------
    enum LOGCHANNEL
    {
        LOG_CHANNEL_DEFAULT      = 0x01,
        LOG_CHANNEL_MEMORY       = 0x02,
        LOG_CHANNEL_RENDERING    = 0x04,
        LOG_CHANNEL_PHYSICS      = 0x08,
        LOG_CHANNEL_ALL          = ~0
    };

    //**********************************************************************
    // Interface-Class for a Logging-Subsystem
    //**********************************************************************
    class ILogger : public ISubSystem
    {
        Color       m_defaultColor      = LOGTYPE_COLOR_DEFAULT;
        LOGLEVEL    m_logLevel          = LOG_LEVEL_ALL;
        LayerMask   m_channelMask       = LayerMask( LOG_CHANNEL_ALL );

    public:
        Color       getDefaultColor() const { return m_defaultColor; }
        LOGLEVEL    getCurrentLogLevel() const { return m_logLevel; }

        //----------------------------------------------------------------------
        // Set the default color for the text when printed out.
        //----------------------------------------------------------------------
        void setDefaultColor(Color color) { m_defaultColor = color; }

        //----------------------------------------------------------------------
        // Filter the given channels. They will be ignored in future.
        // Add channels together via logical OR e.g. renderMask | physicsMask
        //----------------------------------------------------------------------
        void filterChannels(U32 channels) { m_channelMask.unsetBits( channels ); }

        //----------------------------------------------------------------------
        // Unfilter the given channels. Does nothing if channel was not filtered.
        //----------------------------------------------------------------------
        void unfilterChannels(U32 channels) { m_channelMask.setBits( channels ); }

        //----------------------------------------------------------------------
        // Change the Log-Level. Only messages with Log-Level <= Current-Level
        // will be displayed (and stored).
        //----------------------------------------------------------------------
        void setLogLevel(LOGLEVEL newLogLevel) { m_logLevel = newLogLevel; }

    private:
        //----------------------------------------------------------------------
        // Functions to override. Because templates does not work well with the
        // Locator Pattern, i have to template specialize all different types
        // supported in this class and call the virtual function from them.
        // If you know a better approach let me know ;)
        //----------------------------------------------------------------------
        virtual void _Log(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel, Color color) const = 0;
        virtual void _Log(LOGCHANNEL channel, const char* msg, Color color) const = 0;

        virtual void _Warn(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel) const = 0;
        virtual void _Error(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel) const = 0;

    public:
        //----------------------------------------------------------------------
        // Log different stuff.
        // @Params:
        // "msg/num":   The message to log.
        // "logLevel":  How important the message is. Determines if the message
        //              gets logged or not.
        // "channel":    Where the message is coming from.
        // "color":     Color of the message.
        //----------------------------------------------------------------------
        template <class T>
        void log(LOGCHANNEL channel,
                 T num,
                 LOGLEVEL logLevel,
                 Color color) const
        { 
            _Log( channel, TS( num ).c_str(), logLevel, color ); 
        }

        //----------------------------------------------------------------------
        template <class T>
        void log(LOGCHANNEL channel, T num, LOGLEVEL logLevel = LOG_LEVEL_VERY_IMPORTANT) const
        {
            _Log( channel, TS( num ).c_str(), logLevel, m_defaultColor );
        }

        //----------------------------------------------------------------------
        template <class T>
        void log(LOGCHANNEL channel, T num, Color color) const
        { 
            _Log( channel, TS( num ).c_str(), LOG_LEVEL_VERY_IMPORTANT, LOG_channel_DEFAULT, color );
        };

        //----------------------------------------------------------------------
        template <class T>
        void warn(LOGCHANNEL channel, T num, LOGLEVEL logLevel = LOG_LEVEL_VERY_IMPORTANT) const
        {
            _Warn( channel, TS( num ).c_str(), logLevel );
        }

        //----------------------------------------------------------------------
        template <class T>
        void error(LOGCHANNEL channel, T num, LOGLEVEL logLevel = LOG_LEVEL_VERY_IMPORTANT) const
        {
            _Error( channel, TS( num ).c_str(), logLevel );
        }

        //----------------------------------------------------------------------
        // TEMPLATE SPECIALIZATIONS
        //----------------------------------------------------------------------
        template <>
        void log<const char*>(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel, Color color) const {
            _Log( channel, msg, logLevel, color );
        }
        template <>
        void log<StringID>(LOGCHANNEL channel, StringID msg, LOGLEVEL logLevel, Color color) const {
            _Log( channel, msg.str, logLevel, color );
        }
        template <>
        void log<String>(LOGCHANNEL channel, String msg, LOGLEVEL logLevel, Color color) const {
            _Log( channel, msg.c_str(), logLevel, color );
        }

        //----------------------------------------------------------------------
        template <>
        void log<const char*>(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel) const {
            _Log( channel, msg, logLevel, m_defaultColor );
        }
        template <>
        void log<StringID>(LOGCHANNEL channel, StringID msg, LOGLEVEL logLevel) const {
            _Log( channel, msg.str, logLevel, m_defaultColor );
        }
        template <>
        void log<String>(LOGCHANNEL channel, String msg, LOGLEVEL logLevel) const {
            _Log( channel, msg.c_str(), logLevel, m_defaultColor );
        }

        //----------------------------------------------------------------------
        template <>
        void log<const char*>(LOGCHANNEL channel, const char* msg, Color color) const {
            _Log( channel, msg, LOG_LEVEL_VERY_IMPORTANT, color );
        }
        template <>
        void log<StringID>(LOGCHANNEL channel, StringID msg, Color color) const {
            _Log( channel, msg.str, LOG_LEVEL_VERY_IMPORTANT, color );
        }
        template <>
        void log<String>(LOGCHANNEL channel, String msg, Color color) const {
            _Log( channel, msg.c_str(), LOG_LEVEL_VERY_IMPORTANT, color );
        }

        //----------------------------------------------------------------------
        template <>
        void warn<const char*>(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel) const {
            _Log( channel, msg, logLevel, LOGTYPE_COLOR_WARNING );
        }
        template <>
        void warn<StringID>(LOGCHANNEL channel, StringID msg, LOGLEVEL logLevel) const {
            _Log( channel, msg.str, logLevel, LOGTYPE_COLOR_WARNING );
        }
        template <>
        void warn<String>(LOGCHANNEL channel, String msg, LOGLEVEL logLevel) const {
            _Log( channel, msg.c_str(), logLevel, LOGTYPE_COLOR_WARNING );
        }

        //----------------------------------------------------------------------
        template <>
        void error<const char*>(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel) const {
            _Log( channel, msg, logLevel, LOGTYPE_COLOR_ERROR );
        }
        template <>
        void error<StringID>(LOGCHANNEL channel, StringID msg, LOGLEVEL logLevel) const {
            _Log( channel, msg.str, logLevel, LOGTYPE_COLOR_ERROR );
        }
        template <>
        void error<String>(LOGCHANNEL channel, String msg, LOGLEVEL logLevel) const {
            _Log( channel, msg.c_str(), logLevel, LOGTYPE_COLOR_ERROR );
        }

    protected:
        String _GetchannelAsString(LOGCHANNEL channel) const
        {
            switch (channel)
            {
            case LOG_CHANNEL_MEMORY:     return "[Memory]";
            case LOG_CHANNEL_RENDERING:  return "[Rendering]";
            case LOG_CHANNEL_PHYSICS:    return "[Physics]";
            default:
                return "";
            }
        }

        //----------------------------------------------------------------------
        // Check the given log-level.
        // @Return:
        //  True, if message should be ignored.
        //----------------------------------------------------------------------
        bool _CheckLogLevel(LOGLEVEL logLevel) const
        {
            return logLevel > m_logLevel;
        }

        //----------------------------------------------------------------------
        // Check the given channel if it's contained in the filter mask.
        // @Return:
        //  True, when filtered (bit is not present).
        //----------------------------------------------------------------------
        bool _Filterchannel(LOGCHANNEL channel) const
        {
            bool containsBit = m_channelMask.isBitSet( channel ) ;

            return (not containsBit);
        }

    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************


} } // end namespaces
