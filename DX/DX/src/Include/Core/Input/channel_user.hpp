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
        inline void setChannels(InputChannels channelMask) { m_channelMask = channelMask; }

        //----------------------------------------------------------------------
        // Enables the given channel.
        //----------------------------------------------------------------------
        inline void setChannel(EInputChannel channel) { m_channelMask |= (U32)channel; }

        //----------------------------------------------------------------------
        // @Return: Current channel mask.
        //----------------------------------------------------------------------
        inline InputChannels getChannelMask() const { return m_channelMask; }

    protected:
        //----------------------------------------------------------------------
        bool _IsMasterChannelSet() const
        {
            return (m_channelMask & (U32)EInputChannel::Master) != 0;
        }

    private:
        InputChannels m_channelMask;
    };

} } // end namespaces