#include "durations.h"

/**********************************************************************
    class: Several Classes relating to time (durations.cpp)
    
    author: S. Hau
    date: November 01, 2017
**********************************************************************/


namespace Core { namespace Time {

    //----------------------------------------------------------------------
    Minutes::operator Seconds() const
    {
        return Seconds( value * 60.0 );
    }

    Minutes::operator Milliseconds() const
    {
        return Milliseconds( value * 60.0 * 1000.0 );
    }

    Minutes::operator Microseconds() const
    {
        return Microseconds( value * 60.0 * 1000.0 * 1000.0 );
    }

    Minutes::operator Nanoseconds() const
    {
        return Nanoseconds( value * 60.0 * 1000.0 * 1000.0 * 1000.0 );
    }


    //----------------------------------------------------------------------
    Seconds::operator Minutes() const
    {
        return Minutes( value / 60.0 );
    }

    Seconds::operator Milliseconds() const
    {
        return Milliseconds( value * 1000.0 );
    }
 
    Seconds::operator Microseconds() const
    {
        return Microseconds( value * 1000.0 * 1000.0 );
    }

    Seconds::operator Nanoseconds() const
    {
        return Nanoseconds( value * 1000.0 * 1000.0 * 1000.0 );
    }

    //----------------------------------------------------------------------
    Milliseconds::operator Minutes() const
    {
        return Minutes( ( value * 0.001 ) / 60.0 );
    }

    Milliseconds::operator Seconds() const
    {
        return Seconds( value * 0.001 );
    }

    Milliseconds::operator Microseconds() const
    {
        return Microseconds( value * 1000.0 );
    }

    Milliseconds::operator Nanoseconds() const
    {
        return Nanoseconds( value * 1000.0 * 1000.0 );
    }

    //----------------------------------------------------------------------
    Microseconds::operator Minutes() const
    {
        return Minutes( ( value * 0.001 * 0.001 ) / 60.0 );
    }

    Microseconds::operator Seconds() const
    {
        return Seconds( value * 0.001 * 0.001);
    }

    Microseconds::operator Milliseconds() const
    {
        return Milliseconds( value * 0.001 );
    }

    Microseconds::operator Nanoseconds() const
    {
        return Nanoseconds( value * 1000.0 );
    }

    //----------------------------------------------------------------------
    Nanoseconds::operator Minutes() const
    {
        return Minutes( ( value * 0.001 * 0.001 * 0.001 ) / 60.0 );
    }

    Nanoseconds::operator Seconds() const
    {
        return Seconds(value * 0.001 * 0.001 * 0.001);
    }

    Nanoseconds::operator Milliseconds() const
    {
        return Milliseconds( value * 0.001 * 0.001 );
    }

    Nanoseconds::operator Microseconds() const
    {
        return Microseconds( value * 0.001 );
    }


} } // end namespaces
