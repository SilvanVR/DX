#pragma once
/**********************************************************************
    class: MasterClock (master_clock.h)
    
    author: S. Hau
    date: October 27, 2017

    Represents a clock which measures past time using the PlatformTimer.

    @Consideration:
      - Scale time
      - No dynamic allocations for callbacks
**********************************************************************/

#include "timers.h"

//----------------------------------------------------------------------
namespace Core{ class CoreEngine; }


namespace Core { namespace Time {

    //**********************************************************************
    class MasterClock
    {
    public:
        MasterClock();
        ~MasterClock() = default;

        //----------------------------------------------------------------------
        // @Return: Delta time (in seconds) between two frames.
        //----------------------------------------------------------------------
        F64 getDelta() const { return m_delta; }

        //----------------------------------------------------------------------
        // @Return: Time in seconds since this clock was created.
        //----------------------------------------------------------------------
        F64 getTime();

        //----------------------------------------------------------------------
        // Attach a function to this clock.
        // @Params:
        //  "func": The function to call.
        //  "ms": Time in milliseconds between each function call.
        //----------------------------------------------------------------------
        CallbackID  setInterval(const std::function<void()>& func, F32 ms);

        //----------------------------------------------------------------------
        // Attach a function to this clock.
        // @Params:
        //  "func": The function to call once.
        //  "ms": Time in milliseconds after the function will be called.
        //----------------------------------------------------------------------
        CallbackID  setTimeout(const std::function<void()>& func, F32 ms);

        //----------------------------------------------------------------------
        // Remove a callback from this clock.
        //----------------------------------------------------------------------
        void        clearCallback(CallbackID id);

    private:
        F64 m_delta         = 0;
        U64 m_startTicks    = 0;
        U64 m_curTicks      = 0;

        std::vector<CallbackID> idsToRemove;
        std::map<CallbackID, CallbackTimer> m_timers;

        //----------------------------------------------------------------------
        // Used for other clocks to poll the cpu time.
        // @Return:
        //   Amount of ticks since this clock was created.
        //----------------------------------------------------------------------
        U64 getCurTicks() const { return m_curTicks; }

        //----------------------------------------------------------------------
        // Updates the clock and returns the newly calculated delta.
        //----------------------------------------------------------------------
        friend class Core::CoreEngine;
        F64 _Update();


        //----------------------------------------------------------------------
        CallbackID _AttachCallback(const std::function<void()>& func, F32 ms, bool loop);
        void _UpdateTimer();

        //----------------------------------------------------------------------
        MasterClock(const MasterClock& other)                 = delete;
        MasterClock& operator = (const MasterClock& other)    = delete;
        MasterClock(MasterClock&& other)                      = delete;
        MasterClock& operator = (MasterClock&& other)         = delete;
    };


} } // end namespaces