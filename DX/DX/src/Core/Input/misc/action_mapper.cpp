#include "action_mapper.h"
/**********************************************************************
    class: ActionMapper (action_mapper.cpp)

    author: S. Hau
    date: November 18, 2017
**********************************************************************/

#include "locator.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    void ActionMapper::attachKeyboardEvent( const char* name, Key key )
    {
        InputDeviceEvent evt;
        evt.device      = EInputDevice::Keyboard;
        evt.keyboardKey = key;

        m_actionEvents[ SID( name ) ].events.push_back( evt );
    }

    //----------------------------------------------------------------------
    void ActionMapper::attachMouseEvent( const char* name, MouseKey key )
    {
        InputDeviceEvent evt;
        evt.device   = EInputDevice::Mouse;
        evt.mouseKey = key;

        m_actionEvents[ SID( name ) ].events.push_back( evt );
    }

    //----------------------------------------------------------------------
    bool ActionMapper::isKeyDown( const char* name )
    {
        StringID action = SID( name );
        if (m_actionEvents.count(action) == 0)
        {
            WARN( "ActionMapper::isActionKeyDown(): Action '" + action.toString() + "' does not exist." );
            return false;
        }

        return m_actionEvents[ action ].activeThisTick;
    }

    //----------------------------------------------------------------------
    bool ActionMapper::wasKeyPressed( const char* name )
    {
        StringID action = SID( name );
        if (m_actionEvents.count( action ) == 0)
        {
            WARN( "ActionMapper::wasActionKeyPressed(): Action '" + action.toString() + "' does not exist." );
            return false;
        }

        return ( m_actionEvents[ action ].activeThisTick && not m_actionEvents[ action ].activeLastTick );
    }

    //----------------------------------------------------------------------
    bool ActionMapper::wasKeyReleased( const char* name )
    {
        StringID action = SID( name );
        if (m_actionEvents.count( action ) == 0)
        {
            WARN( "ActionMapper::wasActionKeyPressed(): Action '" + action.toString() + "' does not exist." );
            return false;
        }

        return ( not m_actionEvents[ action ].activeThisTick && m_actionEvents[ action ].activeLastTick );
    }

    //----------------------------------------------------------------------
    void ActionMapper::_UpdateInternalState( const Keyboard& keyboard, const Mouse& mouse )
    {
        // Loop through each Action and check if it should be fired
        for (auto& pair : m_actionEvents)
        {
            Action& action = pair.second;
            action.activeLastTick = action.activeThisTick;

            // Loop through each attached event and check if it happened
            for (auto& evt : action.events)
            {
                switch (evt.device)
                {
                case EInputDevice::Keyboard:
                    action.activeThisTick = keyboard.isKeyDown( evt.keyboardKey );
                    break;
                case EInputDevice::Mouse:
                    action.activeThisTick = mouse.isKeyDown( evt.mouseKey );
                    break;
                default:
                    WARN( "ActionMapper::_UpdateInternalState: Unknown input device." );
                }

                // Action should be fired. Break out of loop.
                if (action.activeThisTick)
                    break;
            }
        }
    }


} } // End namespaces