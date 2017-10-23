#pragma once

/**********************************************************************
    class: ILogger (i_logger.hpp)

    author: S. Hau
    date: October 11, 2017

    Interface for a logger subsystem. 
    Features:
     - Several Channels which can be toggled on and off. 
     - Different importance levels. Unimportant messages can be toggled off.
     - Colored Output!
    @Considerations:
     - Set color for a channel.

**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "Core/Misc/color.h"
#include "Core/Misc/bit_mask.hpp"

namespace Core { namespace Logging {

    //----------------------------------------------------------------------
    #define LOGTYPE_COLOR_DEFAULT       Color::WHITE
    #define LOGTYPE_COLOR_WARNING       Color::YELLOW
    #define LOGTYPE_COLOR_ERROR         Color::RED

    //----------------------------------------------------------------------
    enum class ELogLevel
    {
        VERY_IMPORTANT    = 0,
        IMPORTANT         = 1,
        NOT_SO_IMPORTANT  = 2,
        NOT_IMPORTANT     = 3,
        ALL
    };

    //----------------------------------------------------------------------
    enum ELogChannel
    {
        LOG_CHANNEL_DEFAULT      = 1 << 0,
        LOG_CHANNEL_MEMORY       = 1 << 1,
        LOG_CHANNEL_RENDERING    = 1 << 2,
        LOG_CHANNEL_PHYSICS      = 1 << 3,
        LOG_CHANNEL_TEST         = 1 << 4,
        LOG_CHANNEL_ALL          = ~0
    };

    //----------------------------------------------------------------------
    enum class ELogType
    {
        NONE,
        INFO,
        WARNING,
        ERROR
    };

    //**********************************************************************
    // Interface-Class for a Logging-Subsystem
    //**********************************************************************
    class ILogger : public ISubSystem
    {
        BitMask     m_channelMask       = BitMask( static_cast<U32>( LOG_CHANNEL_ALL ) );

    protected:
        Color       m_defaultColor      = LOGTYPE_COLOR_DEFAULT;
        ELogLevel   m_logLevel          = ELogLevel::ALL;
        bool        m_dumpToDisk        = true;

    public:
        virtual ~ILogger() {}

        //----------------------------------------------------------------------
        // Set whether log messages should be saved to disk
        //----------------------------------------------------------------------
        void setSaveToDisk(bool saveToDisk){ m_dumpToDisk = saveToDisk; }

        //----------------------------------------------------------------------
        // Set the default color for the text when printed out.
        //----------------------------------------------------------------------
        void setDefaultColor(Color color) { m_defaultColor = color; }

        //----------------------------------------------------------------------
        // Set all given channels as active. Any other channel will be filtered.
        //----------------------------------------------------------------------
        void setChannels(U32 channels) { m_channelMask.unsetAnyBit(); m_channelMask.setBits( channels ); }

        //----------------------------------------------------------------------
        // Filter the given channels. They will be ignored in future.
        // Add channels together via logical OR e.g. renderMask | physicsMask
        //----------------------------------------------------------------------
        void filterChannels(U32 channels) { m_channelMask.unsetBits( channels ); }

        //----------------------------------------------------------------------
        // Unfilter the given channels. Does nothing if channels were not filtered.
        //----------------------------------------------------------------------
        void unfilterChannels(U32 channels) { m_channelMask.setBits( channels ); }

        //----------------------------------------------------------------------
        // Change the Log-Level. Only messages with Log-Level <= Current-Level
        // will be displayed (and stored).
        //----------------------------------------------------------------------
        void setLogLevel(ELogLevel newLogLevel) { m_logLevel = newLogLevel; }

    private:
        //----------------------------------------------------------------------
        // Functions to override. Because templates does not work well with the
        // Locator Pattern, i have to template specialize all different types
        // supported in this class and call the virtual function from them.
        // If you know a better approach let me know ;)
        //----------------------------------------------------------------------
        virtual void _Log(ELogChannel channel, const char* msg, ELogLevel ELogLevel, Color color) = 0;
        virtual void _Log(ELogChannel channel, const char* msg, Color color) = 0;

        virtual void _Warn(ELogChannel channel, const char* msg, ELogLevel ELogLevel) = 0;
        virtual void _Error(ELogChannel channel, const char* msg, ELogLevel ELogLevel) = 0;

    public:
        //----------------------------------------------------------------------
        // Log different stuff.
        // @Params:
        // "msg/num":   The message to log.
        // "ELogLevel":  How important the message is. Determines if the message
        //              gets logged or not.
        // "channel":   Where the message is coming from.
        // "color":     Color of the message.
        //----------------------------------------------------------------------
        template <class T>
        void log(ELogChannel channel, T num, ELogLevel logLevel, Color color)
        { 
            _Log( channel, TS( num ).c_str(), logLevel, color );
        }

        //----------------------------------------------------------------------
        template <class T>
        void log(ELogChannel channel, T num, ELogLevel logLevel = ELogLevel::VERY_IMPORTANT)
        {
            _Log( channel, TS( num ).c_str(), logLevel, m_defaultColor );
        }

        //----------------------------------------------------------------------
        template <class T>
        void log(ELogChannel channel, T num, Color color)
        { 
            _Log( channel, TS( num ).c_str(), ELogLevel::VERY_IMPORTANT, LOG_channel_DEFAULT, color );
        };

        //----------------------------------------------------------------------
        template <class T>
        void warn(ELogChannel channel, T num, ELogLevel logLevel = ELogLevel::VERY_IMPORTANT)
        {
            _Warn( channel, TS( num ).c_str(), logLevel );
        }

        //----------------------------------------------------------------------
        template <class T>
        void error(ELogChannel channel, T num, ELogLevel logLevel = ELogLevel::VERY_IMPORTANT)
        {
            _Error( channel, TS( num ).c_str(), logLevel );
        }

        //----------------------------------------------------------------------
        // TEMPLATE SPECIALIZATIONS
        //----------------------------------------------------------------------
        template <>
        void log<const char*>(ELogChannel channel, const char* msg, ELogLevel logLevel, Color color) {
            _Log( channel, msg, logLevel, color );
        }
        template <>
        void log<StringID>(ELogChannel channel, StringID msg, ELogLevel logLevel, Color color) {
            _Log( channel, msg.c_str(), logLevel, color );
        }
        template <>
        void log<String>(ELogChannel channel, String msg, ELogLevel logLevel, Color color) {
            _Log( channel, msg.c_str(), logLevel, color );
        }

        //----------------------------------------------------------------------
        template <>
        void log<const char*>(ELogChannel channel, const char* msg, ELogLevel logLevel) {
            _Log( channel, msg, logLevel, m_defaultColor );
        }
        template <>
        void log<StringID>(ELogChannel channel, StringID msg, ELogLevel logLevel) {
            _Log( channel, msg.c_str(), logLevel, m_defaultColor );
        }
        template <>
        void log<String>(ELogChannel channel, String msg, ELogLevel logLevel) {
            _Log( channel, msg.c_str(), logLevel, m_defaultColor );
        }

        //----------------------------------------------------------------------
        template <>
        void log<const char*>(ELogChannel channel, const char* msg, Color color) {
            _Log( channel, msg, ELogLevel::VERY_IMPORTANT, color );
        }
        template <>
        void log<StringID>(ELogChannel channel, StringID msg, Color color) {
            _Log( channel, msg.c_str(), ELogLevel::VERY_IMPORTANT, color );
        }
        template <>
        void log<String>(ELogChannel channel, String msg, Color color) {
            _Log( channel, msg.c_str(), ELogLevel::VERY_IMPORTANT, color );
        }

        //----------------------------------------------------------------------
        template <>
        void warn<const char*>(ELogChannel channel, const char* msg, ELogLevel logLevel) {
            _Warn( channel, msg, logLevel );
        }
        template <>
        void warn<StringID>(ELogChannel channel, StringID msg, ELogLevel logLevel) {
            _Warn( channel, msg.c_str(), logLevel );
        }
        template <>
        void warn<String>(ELogChannel channel, String msg, ELogLevel logLevel) {
            _Warn( channel, msg.c_str(), logLevel );
        }

        //----------------------------------------------------------------------
        template <>
        void error<const char*>(ELogChannel channel, const char* msg, ELogLevel logLevel) {
            _Error( channel, msg, logLevel );
        }
        template <>
        void error<StringID>(ELogChannel channel, StringID msg, ELogLevel logLevel) {
            _Error( channel, msg.c_str(), logLevel );
        }
        template <>
        void error<String>(ELogChannel channel, String msg, ELogLevel logLevel) {
            _Error( channel, msg.c_str(), logLevel );
        }

    protected:
        const char* _GetChannelAsString(ELogChannel channel) const
        {
            switch (channel)
            {
            case LOG_CHANNEL_MEMORY:     return "[Memory] ";
            case LOG_CHANNEL_RENDERING:  return "[Rendering] ";
            case LOG_CHANNEL_PHYSICS:    return "[Physics] ";
            case LOG_CHANNEL_TEST:       return "[TEST] ";
            default:  
                return "";
            }
        }

        const char* _GetLogTypeAsString(ELogType type) const
        {
            switch (type)
            {
            case ELogType::INFO:     return "[INFO] ";
            case ELogType::WARNING:  return "[WARNING] ";
            case ELogType::ERROR:    return "[ERROR] ";
            default:
                return "";
            }
        }

        //----------------------------------------------------------------------
        // Check the given log-level.
        // @Return:
        //  True, if message should be ignored.
        //----------------------------------------------------------------------
        bool _CheckLogLevel(ELogLevel logLevel) const
        {
            return (logLevel > m_logLevel);
        }

        //----------------------------------------------------------------------
        // Check the given channel if it's contained in the filter mask.
        // @Return:
        //  True, when filtered (bit is not present).
        //----------------------------------------------------------------------
        bool _Filterchannel(ELogChannel channel) const
        {
            return ( not m_channelMask.isBitSet( static_cast<U32>( channel ) ) );
        }

    };


} } // end namespaces
