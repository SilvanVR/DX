#pragma once
/**********************************************************************
    class: Several Classes relating to time (durations.h)

    author: S. Hau
    date: November 01, 2017

    Defines high precision durations using floating point.
**********************************************************************/

namespace Core { namespace Time {

    //**********************************************************************
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
            F64 value = 0;
            Duration(F64 val)
                : value( val ) {}
#ifdef _DEBUG
            virtual ~Duration() = 0 {} // Virtual doubles the size of the type because of the vtable
#else
            ~Duration() = default;
#endif

            explicit operator F64() const { return value; }

            Duration&   operator +=  (F64 val)       { value += val; return *this; }
            Duration&   operator -=  (F64 val)       { value -= val; return *this; }
            Duration&   operator *=  (F64 val)       { value *= val; return *this; }
            Duration&   operator /=  (F64 val)       { value /= val; return *this; }
            bool        operator ==  (F64 val) const { return value == val; }
            bool        operator !=  (F64 val) const { return value != val; }
            bool        operator <   (F64 val) const { return value < val; }
            bool        operator >   (F64 val) const { return value > val; }
            bool        operator <=  (F64 val) const { return value <= val; }
            bool        operator >=  (F64 val) const { return value >= val; }
            F64         operator /   (F64 val) const { return value / val; }
        };
    }

    //**********************************************************************
    struct Minutes : public INTERNAL::Duration
    {
        Minutes(F64 minutes = 0)
            : Duration( minutes ) {}

        //----------------------------------------------------------------------
        operator Seconds() const;
        operator Milliseconds() const;
        operator Microseconds() const;
        operator Nanoseconds() const;

        Minutes&    operator +=  (const Minutes& other)         { value += other.value; return *this; }
        Minutes&    operator -=  (const Minutes& other)         { value -= other.value; return *this; }
        Minutes&    operator *=  (const Minutes& other)         { value *= other.value; return *this; }
        Minutes&    operator /=  (const Minutes& other)         { value /= other.value; return *this; }
        bool        operator ==  (const Minutes& other) const   { return value == other.value; }
        bool        operator !=  (const Minutes& other) const   { return value != other.value; }
        bool        operator <   (const Minutes& other) const   { return value < other.value; }
        bool        operator >   (const Minutes& other) const   { return value > other.value; }
        bool        operator <=  (const Minutes& other) const   { return value <= other.value; }
        bool        operator >=  (const Minutes& other) const   { return value >= other.value; }
        F64         operator /   (const Minutes& other) const   { return value / other.value; }
        Minutes     operator %   (const Minutes& other) const   { return fmod( value, other.value ); }
    };

    //**********************************************************************
    struct Seconds : public INTERNAL::Duration
    {
        Seconds(F64 seconds = 0)
            : Duration( seconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Milliseconds() const;
        operator Microseconds() const;
        operator Nanoseconds() const;

        Seconds&    operator +=  (const Seconds& other)         { value += other.value; return *this; }
        Seconds&    operator -=  (const Seconds& other)         { value -= other.value; return *this; }
        Seconds&    operator *=  (const Seconds& other)         { value *= other.value; return *this; }
        Seconds&    operator /=  (const Seconds& other)         { value /= other.value; return *this; }
        bool        operator ==  (const Seconds& other) const   { return value == other.value; }
        bool        operator !=  (const Seconds& other) const   { return value != other.value; }
        bool        operator <   (const Seconds& other) const   { return value < other.value; }
        bool        operator >   (const Seconds& other) const   { return value > other.value; }
        bool        operator <=  (const Seconds& other) const   { return value <= other.value; }
        bool        operator >=  (const Seconds& other) const   { return value >= other.value; }
        F64         operator /   (const Seconds& other) const   { return value / other.value; }
        Seconds     operator %   (const Seconds& other) const   { return fmod( value, other.value ); }
    };

    //**********************************************************************
    struct Milliseconds : public INTERNAL::Duration
    {
        Milliseconds(F64 milliseconds = 0)
            : Duration( milliseconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Seconds() const;
        operator Microseconds() const;
        operator Nanoseconds() const;

        Milliseconds&    operator +=  (const Milliseconds& other)       { value += other.value; return *this; }
        Milliseconds&    operator -=  (const Milliseconds& other)       { value -= other.value; return *this; }
        Milliseconds&    operator *=  (const Milliseconds& other)       { value *= other.value; return *this; }
        Milliseconds&    operator /=  (const Milliseconds& other)       { value /= other.value; return *this; }
        bool             operator ==  (const Milliseconds& other) const { return value == other.value; }
        bool             operator !=  (const Milliseconds& other) const { return value != other.value; }
        bool             operator <   (const Milliseconds& other) const { return value < other.value; }
        bool             operator >   (const Milliseconds& other) const { return value > other.value; }
        bool             operator <=  (const Milliseconds& other) const { return value <= other.value; }
        bool             operator >=  (const Milliseconds& other) const { return value >= other.value; }
        F64              operator /   (const Milliseconds& other) const { return value / other.value; }
        Milliseconds     operator %   (const Milliseconds& other) const { return fmod( value, other.value ); }
        Milliseconds     operator -   (const Milliseconds& other) const { return Milliseconds( value - other.value ); }
        Milliseconds     operator +   (const Milliseconds& other) const { return Milliseconds( value + other.value ); }
    };

    //**********************************************************************
    struct Microseconds : public INTERNAL::Duration
    {
        Microseconds(F64 microseconds = 0)
            : Duration( microseconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Seconds() const;
        operator Milliseconds() const;
        operator Nanoseconds() const;

        Microseconds&    operator +=  (const Microseconds& other)       { value += other.value; return *this; }
        Microseconds&    operator -=  (const Microseconds& other)       { value -= other.value; return *this; }
        Microseconds&    operator *=  (const Microseconds& other)       { value *= other.value; return *this; }
        Microseconds&    operator /=  (const Microseconds& other)       { value /= other.value; return *this; }
        bool             operator ==  (const Microseconds& other) const { return value == other.value; }
        bool             operator !=  (const Microseconds& other) const { return value != other.value; }
        bool             operator <   (const Microseconds& other) const { return value < other.value; }
        bool             operator >   (const Microseconds& other) const { return value > other.value; }
        bool             operator <=  (const Microseconds& other) const { return value <= other.value; }
        bool             operator >=  (const Microseconds& other) const { return value >= other.value; }
        F64              operator /   (const Microseconds& other) const { return value / other.value; }
        Microseconds     operator %   (const Microseconds& other) const { return fmod( value, other.value ); }
    };

    //**********************************************************************
    struct Nanoseconds : public INTERNAL::Duration
    {
        Nanoseconds(F64 nanoseconds = 0)
            : Duration( nanoseconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Seconds() const;
        operator Milliseconds() const;
        operator Microseconds() const;

        Nanoseconds&     operator +=  (const Nanoseconds& other)       { value += other.value; return *this; }
        Nanoseconds&     operator -=  (const Nanoseconds& other)       { value -= other.value; return *this; }
        Nanoseconds&     operator *=  (const Nanoseconds& other)       { value *= other.value; return *this; }
        Nanoseconds&     operator /=  (const Nanoseconds& other)       { value /= other.value; return *this; }
        bool             operator ==  (const Nanoseconds& other) const { return value == other.value; }
        bool             operator !=  (const Nanoseconds& other) const { return value != other.value; }
        bool             operator <   (const Nanoseconds& other) const { return value < other.value; }
        bool             operator >   (const Nanoseconds& other) const { return value > other.value; }
        bool             operator <=  (const Nanoseconds& other) const { return value <= other.value; }
        bool             operator >=  (const Nanoseconds& other) const { return value >= other.value; }
        F64              operator /   (const Nanoseconds& other) const { return value / other.value; }
        Nanoseconds      operator %   (const Nanoseconds& other) const { return fmod( value, other.value ); }
    };


} } // end namespaces