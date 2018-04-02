#include "event_dispatcher.h"
/**********************************************************************
    class: EventDispatcher (event_dispatcher.cpp)

    author: S. Hau
    date: April 2, 2018
**********************************************************************/

namespace Events {

    //----------------------------------------------------------------------
    HashMap<StringID, Event> EventDispatcher::m_eventMap;

    //----------------------------------------------------------------------
    Event& EventDispatcher::GetEvent( StringID eventName )
    {
        if ( m_eventMap.count( eventName ) == 0 )
            m_eventMap.emplace( eventName, eventName );

        return m_eventMap[eventName];
    }

}