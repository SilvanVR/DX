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
        template <class T>
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
            //----------------------------------------------------------------------
            explicit operator F64() const { return value; }

            //----------------------------------------------------------------------
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
            F64         operator +   (F64 val) const { return value + val; }
            F64         operator -   (F64 val) const { return value - val; }
            F64         operator *   (F64 val) const { return value * val; }
            F64         operator /   (F64 val) const { return value / val; }

            //----------------------------------------------------------------------
            void        operator +=  (const T& other)       { value += other.value; }
            void        operator -=  (const T& other)       { value -= other.value; }
            void        operator *=  (const T& other)       { value *= other.value; }
            void        operator /=  (const T& other)       { value /= other.value; }
            bool        operator ==  (const T& other) const { return value == other.value; }
            bool        operator !=  (const T& other) const { return value != other.value; }
            bool        operator <   (const T& other) const { return value < other.value; }
            bool        operator >   (const T& other) const { return value > other.value; }
            bool        operator <=  (const T& other) const { return value <= other.value; }
            bool        operator >=  (const T& other) const { return value >= other.value; }
            T           operator %   (const T& other) const { return T( fmod( value, other.value ) ); }
            T           operator +   (const T& other) const { return T( value + other.value ); }
            T           operator -   (const T& other) const { return T( value - other.value ); }
            T           operator *   (const T& other) const { return T( value * other.value ); }
            T           operator /   (const T& other) const { return T( value / other.value ); }
        };
    }

    //**********************************************************************
    struct Minutes : public INTERNAL::Duration<Minutes>
    {
        Minutes(F64 minutes = 0)
            : Duration( minutes ) {}

        //----------------------------------------------------------------------
        operator Seconds() const;
        operator Milliseconds() const;
        operator Microseconds() const;
        operator Nanoseconds() const;
    };

    //**********************************************************************
    struct Seconds : public INTERNAL::Duration<Seconds>
    {
        Seconds(F64 seconds = 0)
            : Duration( seconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Milliseconds() const;
        operator Microseconds() const;
        operator Nanoseconds() const;
    };

    //**********************************************************************
    struct Milliseconds : public INTERNAL::Duration<Milliseconds>
    {
        Milliseconds(F64 milliseconds = 0)
            : Duration( milliseconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Seconds() const;
        operator Microseconds() const;
        operator Nanoseconds() const;
    };

    //**********************************************************************
    struct Microseconds : public INTERNAL::Duration<Microseconds>
    {
        Microseconds(F64 microseconds = 0)
            : Duration( microseconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Seconds() const;
        operator Milliseconds() const;
        operator Nanoseconds() const;
    };

    //**********************************************************************
    struct Nanoseconds : public INTERNAL::Duration<Nanoseconds>
    {
        Nanoseconds(F64 nanoseconds = 0)
            : Duration( nanoseconds ) {}

        //----------------------------------------------------------------------
        operator Minutes() const;
        operator Seconds() const;
        operator Milliseconds() const;
        operator Microseconds() const;
    };


} } // end namespaces