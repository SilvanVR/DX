#pragma once
/**********************************************************************
    class: Event (event.h)

    author: S. Hau
    date: April 2, 2018
**********************************************************************/

namespace Events {

    //**********************************************************************
    class Event {
    public:
        Event() = default;
        Event(StringID name) : m_name(name) {}
        ~Event() = default;

        //----------------------------------------------------------------------
        // Add a new listener function to this event
        //----------------------------------------------------------------------
        void addListener(const std::function<void()>& listener) { m_listener.push_back( listener ); }

        //----------------------------------------------------------------------
        // Remove a listener function from this event
        //----------------------------------------------------------------------
        //void removeListener(const std::function<void()>& listenerToRemove) {}

        //----------------------------------------------------------------------
        // Call all functions which are attached to this event
        //----------------------------------------------------------------------
        void invoke() const
        {
            for (auto& listener : m_listener)
                listener();
        }

    private:
        StringID m_name;
        ArrayList<std::function<void()>> m_listener;

        //----------------------------------------------------------------------
        Event(const Event& other)                 = delete;
        Event& operator = (const Event& other)    = delete;
        Event(Event&& other)                      = delete;
        Event& operator = (Event&& other)         = delete;
    };

}