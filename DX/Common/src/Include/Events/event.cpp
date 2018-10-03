#include "event.h"
/**********************************************************************
    class: Event

    author: S. Hau
    date: April 2, 2018
**********************************************************************/

namespace Events {

    //**********************************************************************
    // EventListener
    //**********************************************************************

    //----------------------------------------------------------------------
    void EventListener::_RemoveListenerFromEvent()
    {
        if (evt)
            evt->removeListener( *this );
    }

    //**********************************************************************
    // Event
    //**********************************************************************

    //----------------------------------------------------------------------
    EventListener Event::addListener( const std::function<void()>& listener )
    {
        static ListenerID NEXT_ID = 1; 

        ListenerID nextID = NEXT_ID;
        m_listener.push_back({ nextID, listener });
        ++NEXT_ID;

        return EventListener{ nextID, this };
    }

    //----------------------------------------------------------------------
    void Event::removeListener( const EventListener& listener )
    { 
        m_listener.erase( std::remove_if( m_listener.begin(), m_listener.end(), 
                                          [&](const Listener& l) { return l.id == listener.id; }), m_listener.end() );
    }

    //----------------------------------------------------------------------
    void Event::invoke() const
    {
        for (auto& listener : m_listener)
            listener.func();
    }

}