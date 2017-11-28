#pragma once


/**********************************************************************
    class:  ISubSystem (i_subsystem.hpp)

    author: S. Hau
    date: October 10, 2017

    Represents a subsystem in the program, which acts in ideal
    completely independant from any other subsystem.

**********************************************************************/

#include "Time/durations.h"

namespace Core
{
    //----------------------------------------------------------------------
    class CoreEngine;

    //**********************************************************************
    class ISubSystem
    {
    public:
        ISubSystem() = default;
        virtual ~ISubSystem() = default;

        // Those two must be overriden
        virtual void init(CoreEngine* coreEngine) = 0;
        virtual void shutdown() = 0;

        // Those can be overriden. They will only be called if a subsystem subscribes to the core engine.
        virtual void OnTick(Time::Seconds delta) {}
        virtual void OnUpdate(Time::Seconds delta) {}
    };

}

