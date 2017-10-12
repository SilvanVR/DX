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
#include "Core/Misc/layer_mask.hpp"

namespace Core { namespace Logging {

    //----------------------------------------------------------------------
    #define LOGTYPE_COLOR_DEFAULT       Color::WHITE
    #define LOGTYPE_COLOR_WARNING       Color::YELLOW
    #define LOGTYPE_COLOR_ERROR         Color::RED
    #define LOGTYPE_COLOR_CHANNEL       Color::VIOLET

    //----------------------------------------------------------------------
    enum ELogLevel
    {
        LOG_LEVEL_VERY_IMPORTANT    = 0x00,
        LOG_LEVEL_IMPORTANT         = 0x01,
        LOG_LEVEL_NOT_SO_IMPORTANT  = 0x02,
        LOG_LEVEL_NOT_IMPORTANT     = 0x04,
        LOG_LEVEL_ALL
    };

    //----------------------------------------------------------------------
    enum ELogChannel
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
        LayerMask   m_channelMask       = LayerMask( LOG_CHANNEL_ALL );

    protected:
        Color       m_defaultColor      = LOGTYPE_COLOR_DEFAULT;
        ELogLevel   m_logLevel          = LOG_LEVEL_ALL;
        bool        m_dumpToDisk        = true;

    public:
        //----------------------------------------------------------------------
        // Set whether log messages should be saved and dumped to disk
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
        void setELogLevel(ELogLevel newLogLevel) { m_logLevel = newLogLevel; }

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
        // "channel":    Where the message is coming from.
        // "color":     Color of the message.
        //----------------------------------------------------------------------
        template <class T>
        void log(ELogChannel channel, T num, ELogLevel ELogLevel, Color color)
        { 
            _Log( channel, TS( num ).c_str(), ELogLevel, color ); 
        }

        //----------------------------------------------------------------------
        template <class T>
        void log(ELogChannel channel, T num, ELogLevel ELogLevel = LOG_LEVEL_VERY_IMPORTANT) 
        {
            _Log( channel, TS( num ).c_str(), ELogLevel, m_defaultColor );
        }

        //----------------------------------------------------------------------
        template <class T>
        void log(ELogChannel channel, T num, Color color)
        { 
            _Log( channel, TS( num ).c_str(), LOG_LEVEL_VERY_IMPORTANT, LOG_channel_DEFAULT, color );
        };

        //----------------------------------------------------------------------
        template <class T>
        void warn(ELogChannel channel, T num, ELogLevel ELogLevel = LOG_LEVEL_VERY_IMPORTANT)
        {
            _Warn( channel, TS( num ).c_str(), ELogLevel );
        }

        //----------------------------------------------------------------------
        template <class T>
        void error(ELogChannel channel, T num, ELogLevel ELogLevel = LOG_LEVEL_VERY_IMPORTANT)
        {
            _Error( channel, TS( num ).c_str(), ELogLevel );
        }

        //----------------------------------------------------------------------
        // TEMPLATE SPECIALIZATIONS
        //----------------------------------------------------------------------
        template <>
        void log<const char*>(ELogChannel channel, const char* msg, ELogLevel ELogLevel, Color color) {
            _Log( channel, msg, ELogLevel, color );
        }
        template <>
        void log<StringID>(ELogChannel channel, StringID msg, ELogLevel ELogLevel, Color color) {
            _Log( channel, msg.str, ELogLevel, color );
        }
        template <>
        void log<String>(ELogChannel channel, String msg, ELogLevel ELogLevel, Color color) {
            _Log( channel, msg.c_str(), ELogLevel, color );
        }

        //----------------------------------------------------------------------
        template <>
        void log<const char*>(ELogChannel channel, const char* msg, ELogLevel ELogLevel) {
            _Log( channel, msg, ELogLevel, m_defaultColor );
        }
        template <>
        void log<StringID>(ELogChannel channel, StringID msg, ELogLevel ELogLevel) {
            _Log( channel, msg.str, ELogLevel, m_defaultColor );
        }
        template <>
        void log<String>(ELogChannel channel, String msg, ELogLevel ELogLevel) {
            _Log( channel, msg.c_str(), ELogLevel, m_defaultColor );
        }

        //----------------------------------------------------------------------
        template <>
        void log<const char*>(ELogChannel channel, const char* msg, Color color) {
            _Log( channel, msg, LOG_LEVEL_VERY_IMPORTANT, color );
        }
        template <>
        void log<StringID>(ELogChannel channel, StringID msg, Color color) {
            _Log( channel, msg.str, LOG_LEVEL_VERY_IMPORTANT, color );
        }
        template <>
        void log<String>(ELogChannel channel, String msg, Color color) {
            _Log( channel, msg.c_str(), LOG_LEVEL_VERY_IMPORTANT, color );
        }

        //----------------------------------------------------------------------
        template <>
        void warn<const char*>(ELogChannel channel, const char* msg, ELogLevel ELogLevel) {
            _Warn( channel, msg, ELogLevel );
        }
        template <>
        void warn<StringID>(ELogChannel channel, StringID msg, ELogLevel ELogLevel) {
            _Warn( channel, msg.str, ELogLevel );
        }
        template <>
        void warn<String>(ELogChannel channel, String msg, ELogLevel ELogLevel) {
            _Warn( channel, msg.c_str(), ELogLevel );
        }

        //----------------------------------------------------------------------
        template <>
        void error<const char*>(ELogChannel channel, const char* msg, ELogLevel ELogLevel) {
            _Error( channel, msg, ELogLevel );
        }
        template <>
        void error<StringID>(ELogChannel channel, StringID msg, ELogLevel ELogLevel) {
            _Error( channel, msg.str, ELogLevel );
        }
        template <>
        void error<String>(ELogChannel channel, String msg, ELogLevel ELogLevel) {
            _Error( channel, msg.c_str(), ELogLevel );
        }

    protected:
        struct _LOGMESSAGE
        {
            StringID        message;
            ELogChannel     channel;
            ELogLevel       level;
        };

        const char* _GetchannelAsString(ELogChannel channel) const
        {
            switch (channel)
            {
            case LOG_CHANNEL_MEMORY:     return "[Memory] ";
            case LOG_CHANNEL_RENDERING:  return "[Rendering] ";
            case LOG_CHANNEL_PHYSICS:    return "[Physics] ";
            default:
                return "";
            }
        }

        //----------------------------------------------------------------------
        // Check the given log-level.
        // @Return:
        //  True, if message should be ignored.
        //----------------------------------------------------------------------
        bool _CheckLogLevel(ELogLevel ELogLevel) const
        {
            return ELogLevel > m_logLevel;
        }

        //----------------------------------------------------------------------
        // Check the given channel if it's contained in the filter mask.
        // @Return:
        //  True, when filtered (bit is not present).
        //----------------------------------------------------------------------
        bool _Filterchannel(ELogChannel channel) const
        {
            return ( not m_channelMask.isBitSet( channel ) );
        }

    };


} } // end namespaces