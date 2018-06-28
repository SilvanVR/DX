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
        IChannelUser(InputChannels channels) : m_channelMask(channels) {}
        virtual ~IChannelUser() = default;

        //----------------------------------------------------------------------
        // Modifies the channel mask for this device.
        //----------------------------------------------------------------------
        inline void setChannels(InputChannels channelMask) { m_channelMask; m_channelMask = channelMask; }

        //----------------------------------------------------------------------
        // Enables the given channel.
        //----------------------------------------------------------------------
        inline void setChannel(EInputChannel channel) { m_channelMask |= channel; }

        //----------------------------------------------------------------------
        // Disables the given channel.
        //----------------------------------------------------------------------
        inline void unsetChannel(EInputChannel channel) { m_channelMask &= ~((InputChannels)channel); }

        //----------------------------------------------------------------------
        // Sets the given channel exclusively.
        //----------------------------------------------------------------------
        inline void claimChannel(EInputChannel channel) { m_channelMask = (InputChannels)channel; }

        //----------------------------------------------------------------------
        // Sets the given channel exclusively.
        //----------------------------------------------------------------------
        inline void restoreDefault() { m_channelMask = (InputChannels)EInputChannel::Default; }

        //----------------------------------------------------------------------
        // @Return: Current channel mask.
        //----------------------------------------------------------------------
        inline InputChannels getChannelMask() const { return m_channelMask; }

    protected:
        //----------------------------------------------------------------------
        bool _IsMasterChannelSet() const { return m_channelMask & EInputChannel::Master; }

    private:
        InputChannels m_channelMask;
    };

} } // end namespaces