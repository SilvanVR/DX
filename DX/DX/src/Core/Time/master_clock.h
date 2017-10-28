#pragma once
/**********************************************************************
    class: MasterClock (master_clock.h)
    
    author: S. Hau
    date: October 27, 2017

    Represents a clock which measures past time using the PlatformTimer.

    @Consideration:
      - Add mechanism for binding functions to a clock
        -> Function called only once / intervals
      - Add super class: Clock
      - Loop clock
      - Scale time
**********************************************************************/

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

        // void addFunction(TimePoint);

    private:
        F64 m_delta         = 0;
        U64 m_startTicks    = 0;
        U64 m_curTicks      = 0;


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
        MasterClock(const MasterClock& other)                 = delete;
        MasterClock& operator = (const MasterClock& other)    = delete;
        MasterClock(MasterClock&& other)                      = delete;
        MasterClock& operator = (MasterClock&& other)         = delete;
    };


} } // end namespaces