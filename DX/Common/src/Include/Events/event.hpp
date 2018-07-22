#pragma once
/**********************************************************************
    class: Event (event.h)

    author: S. Hau
    date: April 2, 2018
**********************************************************************/

namespace Events {

    using ListenerID = U32;

    //**********************************************************************
    class Event {
    public:
        Event() = default;
        Event(StringID name) : m_name(name) {}
        ~Event() = default;

        //----------------------------------------------------------------------
        // Add a new listener function to this event
        //----------------------------------------------------------------------
        ListenerID addListener(const std::function<void()>& listener) 
        {
            static ListenerID NEXT_ID = 0; 

            ListenerID nextID = NEXT_ID;
            m_listener.push_back({ nextID, listener });
            ++NEXT_ID;

            return nextID;
        }

        //----------------------------------------------------------------------
        // Remove a listener function from this event
        //----------------------------------------------------------------------
        void removeListener(ListenerID listenerID)
        { 
            m_listener.erase( std::remove_if( m_listener.begin(), m_listener.end(), 
                                             [listenerID](const Listener& l) { return l.id == listenerID; }), m_listener.end() );
        }

        //----------------------------------------------------------------------
        // Call all functions which are attached to this event
        //----------------------------------------------------------------------
        void invoke() const
        {
            for (auto& listener : m_listener)
                listener.func();
        }

    private:
        StringID m_name;

        struct Listener
        {
            ListenerID id;
            std::function<void()> func;
        };
        ArrayList<Listener> m_listener;

        //----------------------------------------------------------------------
        Event(const Event& other)                 = delete;
        Event& operator = (const Event& other)    = delete;
        Event(Event&& other)                      = delete;
        Event& operator = (Event&& other)         = delete;
    };

}