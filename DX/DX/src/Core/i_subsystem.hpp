#pragma once


/**********************************************************************
    class:  ISubSystem (i_subsystem.hpp)

    author: S. Hau
    date: October 10, 2017

    Represents a subsystem in the program, which acts in ideal
    completely independant from any other subsystem.

**********************************************************************/

namespace Core
{

    class ISubSystem
    {
    public:
        ISubSystem() {}
        ~ISubSystem() {}

        virtual void init() = 0;
        virtual void update() {}
        virtual void shutdown() = 0;
    };

}

