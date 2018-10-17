#pragma once
/**********************************************************************
    class: ChannelUser (channel_user.hpp)

    author: S. Hau
    date: June 17, 2018

    Superclass for devices and listener to support channel based functionality.
**********************************************************************/

#include "input_enums.hpp"

namespace Core { namespace Input {

    //**********************************************************************
    class IChannelUser
    {
    public:
        //----------------------------------------------------------------------
        IChannelUser(EInputChannels channels) : m_channelMask(channels), m_lastChannelMask(channels) {}
        virtual ~IChannelUser() = default;

        //----------------------------------------------------------------------
        // Modifies the channel mask for this device.
        //----------------------------------------------------------------------
        inline void setChannels(EInputChannels channelMask) { m_lastChannelMask = m_channelMask; m_channelMask = channelMask; }

        //----------------------------------------------------------------------
        // Enables the given channel.
        //----------------------------------------------------------------------
        inline void setChannel(EInputChannels channel) { m_lastChannelMask = m_channelMask; m_channelMask |= channel; }

        //----------------------------------------------------------------------
        // Disables the given channel.
        //----------------------------------------------------------------------
        inline void unsetChannel(EInputChannels channel) { m_lastChannelMask = m_channelMask; m_channelMask &= ~channel; }

        //----------------------------------------------------------------------
        // Sets the given channel exclusively.
        //----------------------------------------------------------------------
        inline void claimChannel(EInputChannels channel) { m_lastChannelMask = m_channelMask; m_channelMask = channel; }

        //----------------------------------------------------------------------
        // Sets the given channel exclusively.
        //----------------------------------------------------------------------
        inline void restoreDefault() { m_lastChannelMask = m_channelMask; m_channelMask = EInputChannels::Default; }

        //----------------------------------------------------------------------
        // @Return: Current channel mask.
        //----------------------------------------------------------------------
        inline EInputChannels getChannelMask() const { return m_channelMask; }

        //----------------------------------------------------------------------
        // Restores the last channel mask
        //----------------------------------------------------------------------
        inline void restoreLastChannelMask() { auto curChannel = m_channelMask; m_channelMask = m_lastChannelMask; m_lastChannelMask = curChannel; }

    protected:
        //----------------------------------------------------------------------
        bool _IsMasterChannelSet() const { return (m_channelMask & EInputChannels::Master) != EInputChannels::None; }

    private:
        EInputChannels m_channelMask;
        EInputChannels m_lastChannelMask;
    };

} } // end namespaces