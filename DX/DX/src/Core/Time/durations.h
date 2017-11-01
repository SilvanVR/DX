#pragma once
/**********************************************************************
    class: Several Classes relating to time (durations.h)

    author: S. Hau
    date: November 01, 2017

    Defines high precision durations using floating point.
**********************************************************************/

namespace Core { namespace Time {

    //**********************************************************************
    struct Hours;
    struct Minutes;
    struct Seconds;
    struct Milliseconds;
    struct Microseconds;
    struct Nanoseconds;

    //**********************************************************************
    namespace INTERNAL
    {
        struct Duration
        {
            F64 value;
            Duration(F64 val)
                : value( val ) {}
#ifdef _DEBUG
            virtual ~Duration() = 0 {} // Virtual doubles the size of the type because of the vtable
#else
            ~Duration() = default;
#endif

            explicit operator F64() const { return value; }

            Duration&   operator+=  (const Duration& other)         { value += other.value; return *this; }
            Duration&   operator-=  (const Duration& other)         { value -= other.value; return *this; }
            Duration&   operator*=  (const Duration& other)         { value *= other.value; return *this; }
            Duration&   operator/=  (const Duration& other)         { value /= other.value; return *this; }
            bool        operator<   (const Duration& other) const   { return value < other.value; }
            bool        operator>   (const Duration& other) const   { return value > other.value; }
            bool        operator<=  (const Duration& other) const   { return value <= other.value; }
            bool        operator>=  (const Duration& other) const   { return value >= other.value; }
            F64         operator/   (const Duration& other) const   { return value / other.value; }

            Duration&   operator+=  (F64 val)                       { value += val; return *this; }
            Duration&   operator-=  (F64 val)                       { value -= val; return *this; }
            Duration&   operator*=  (F64 val)                       { value *= val; return *this; }
            Duration&   operator/=  (F64 val)                       { value /= val; return *this; }
            bool        operator<   (F64 val) const                 { return value < val; }
            bool        operator>   (F64 val) const                 { return value > val; }
            bool        operator<=  (F64 val) const                 { return value <= val; }
            bool        operator>=  (F64 val) const                 { return value >= val; }
            F64         operator/   (F64 val) const                 { return value / val; }
        };
    }

    //**********************************************************************
    struct Minutes : public INTERNAL::Duration
    {
        Minutes(F64 minutes)
            : Duration( minutes ) {}

        //----------------------------------------------------------------------
        operator Seconds() const;
        operator Milliseconds() const;
        operator Microseconds() const;
        operator Nanoseconds() const;
    };

    //**********************************************************************
    struct Seconds : public INTERNAL::Duration
    {
        Seconds(F64 seconds)
            : Duration( seconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Milliseconds() const;
        operator Microseconds() const;
        operator Nanoseconds() const;
    };

    //**********************************************************************
    struct Milliseconds : public INTERNAL::Duration
    {
        Milliseconds(F64 milliseconds)
            : Duration( milliseconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Seconds() const;
        operator Microseconds() const;
        operator Nanoseconds() const;
    };

    //**********************************************************************
    struct Microseconds : public INTERNAL::Duration
    {
        Microseconds(F64 microseconds)
            : Duration( microseconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Seconds() const;
        operator Milliseconds() const;
        operator Nanoseconds() const;
    };

    //**********************************************************************
    struct Nanoseconds : public INTERNAL::Duration
    {
        Nanoseconds(F64 nanoseconds)
            : Duration( nanoseconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Seconds() const;
        operator Milliseconds() const;
        operator Microseconds() const;
    };


} } // end namespaces