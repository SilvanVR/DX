#pragma once
/**********************************************************************
    class: Several Listener (input_listener.h)

    author: S. Hau
    date: November 16, 2017

    Contains listener classes related to input.
    If somebody is interested in input, inherit from one of 
    the listener classes and override the appropriate method.

    The InputManager has to be initialized before any
    object inheriting from one of the listener will be created.
**********************************************************************/

#include "OS/Window/keycodes.h"
#include "../channel_user.hpp"

namespace Core { namespace Input {

    //**********************************************************************
    class IDeviceListener : public IChannelUser
    {
    public:
        //----------------------------------------------------------------------
        IDeviceListener(EInputChannels channels) : IChannelUser(channels) {}
        virtual ~IDeviceListener() = default;
    };

    //**********************************************************************
    class IKeyListener : public IDeviceListener
    {
    public:
        IKeyListener(EInputChannels channel = EInputChannels::Master);
        virtual ~IKeyListener();

    protected:
        friend class Keyboard;
        virtual void OnKeyPressed(Key key, KeyMod mod) {}
        virtual void OnKeyReleased(Key key, KeyMod mod) {}
        virtual void OnChar(char c) {}
    };

    //**********************************************************************
    class IMouseListener : public IDeviceListener
    {
    public:
        IMouseListener(EInputChannels channel = EInputChannels::Master);
        virtual ~IMouseListener();

    protected:
        friend class Mouse;
        virtual void OnMouseMoved(I16 x, I16 y) {}
        virtual void OnMousePressed(MouseKey key, KeyMod mod) {}
        virtual void OnMouseReleased(MouseKey key, KeyMod mod) {}
        virtual void OnMouseWheel(I16 delta) {}
    };

} } // end namespaces