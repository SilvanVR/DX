#pragma once
/**********************************************************************
    class: Clock (clock.h)
    
    author: S. Hau
    date: October 28, 2017
**********************************************************************/


namespace Core { namespace Time {

    //**********************************************************************
    class Clock
    {
    public:
        Clock() = default;
        ~Clock() = default;

        //----------------------------------------------------------------------
        CallbackID setInterval(const std::function<void()>& func, U64 ms);
        CallbackID setTimeout(const std::function<void()>& func, U64 ms);
        bool clearCallback(CallbackID id);


    private:
        //----------------------------------------------------------------------
        Clock(const Clock& other)                 = delete;
        Clock& operator = (const Clock& other)    = delete;
        Clock(Clock&& other)                      = delete;
        Clock& operator = (Clock&& other)         = delete;
    };


} } // end namespaces