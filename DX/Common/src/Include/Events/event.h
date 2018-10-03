#pragma once
/**********************************************************************
    class: Event

    author: S. Hau
    date: April 2, 2018
**********************************************************************/

namespace Events {

    using ListenerID = U32;

    class Event;

    //**********************************************************************
    // RAII Listener. Solely for unsubscribing from the corresponding event in the destructor.
    //**********************************************************************
    class EventListener
    {
        static const ListenerID NULL_ID = 0;
    public:
        EventListener() = default;
        ~EventListener() { _RemoveListenerFromEvent(); }

        EventListener(EventListener&& other) { _RemoveListenerFromEvent(); id = other.id; evt = other.evt; other.id = NULL_ID; other.evt = nullptr; }
        EventListener& operator = (EventListener&& other) { _RemoveListenerFromEvent(); id = other.id; evt = other.evt; other.id = NULL_ID; other.evt = nullptr; return *this; }

    private:
        friend class Event;
        EventListener(ListenerID id, Event* evt) : id(id), evt(evt) {}

        void _RemoveListenerFromEvent();

        ListenerID  id = NULL_ID;
        Event*      evt = nullptr;
    };

    //**********************************************************************
    class Event {
    public:
        Event() = default;
        Event(StringID name) : m_name(name) {}
        ~Event() = default;

        //----------------------------------------------------------------------
        // Add a new listener function to this event
        //----------------------------------------------------------------------
        EventListener addListener(const std::function<void()>& listener);

        //----------------------------------------------------------------------
        // Remove a listener function from this event
        //----------------------------------------------------------------------
        void removeListener(const EventListener& listener);

        //----------------------------------------------------------------------
        // Call all functions which are attached to this event
        //----------------------------------------------------------------------
        void invoke() const;

    private:
        StringID m_name;

        struct Listener
        {
            ListenerID id;
            std::function<void()> func;
        };
        ArrayList<Listener> m_listener;

        //----------------------------------------------------------------------
        NULL_COPY_AND_ASSIGN(Event)
    };

}