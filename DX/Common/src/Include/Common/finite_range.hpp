#pragma once
/**********************************************************************
    class: FiniteRange (finite_range.hpp)

    author: S. Hau
    date: November 02, 2017
**********************************************************************/

namespace Common {

    //**********************************************************************
    // Represents a finite range of a value.
    // It is guaranteed that the value never exceeds or fall below the bounds.
    // Possible range: [min, max) - (max is always equal min)
    // Do NOT use UNSIGNED types. (Or a class with unsigned types in it)
    // Limited support for different operators yet.
    //**********************************************************************
    template < class T >
    class FiniteRange
    {
    public:
        explicit FiniteRange(T lowerBound, T upperBound)
            : m_lowerBound( lowerBound ), m_upperBound( upperBound )
        {
            ASSERT( m_lowerBound < m_upperBound && "Min-Bound must be less than Max-Bound." );
        }

        explicit FiniteRange(T startValue, T lowerBound, T upperBound)
            : m_value( startValue ), m_lowerBound( lowerBound ), m_upperBound( upperBound )
        {
            ASSERT( m_lowerBound < m_upperBound && "Min-Bound must be less than Max-Bound." );
            m_value = _MoveInRange( m_value );
        }

        //----------------------------------------------------------------------
        template <typename T2> explicit operator T2 () const { return m_value; }

        //----------------------------------------------------------------------
        // @Return: A new value which is within the bounds of this range object.
        // Examples:
        //  [0,1000]: placeInRange( 1399 ) -> Returns 399
        //  [0,1000]: placeInRange( -400 ) -> Returns 600
        //----------------------------------------------------------------------
        template <typename T2> T2 getValueInRange(T2 val) const { return _MoveInRange( val ); }

        //----------------------------------------------------------------------
        void    setLowerBound(T minBound) { m_lowerBound = minBound; ASSERT( m_lowerBound < m_upperBound && "Min-Bound must be less than Max-Bound." ); }
        void    setUpperBound(T maxBound) { m_upperBound = maxBound; ASSERT( m_lowerBound < m_upperBound && "Min-Bound must be less than Max-Bound." ); }

        T       value()         const { return m_value; }
        T       getLowerBound() const { return m_lowerBound; }
        T       getUpperBound() const { return m_upperBound; }
        T       getRange()      const { return getUpperBound() - getLowerBound(); }

        //----------------------------------------------------------------------
        FiniteRange& operator += (const T& other) { m_value += other; m_value = _MoveInRange( m_value ); return (*this); }
        FiniteRange& operator -= (const T& other) { m_value -= other; m_value = _MoveInRange( m_value ); return (*this); }

    private:
        T m_value       = 0;
        T m_lowerBound  = 0;
        T m_upperBound  = 0;

        //----------------------------------------------------------------------
        inline T _MoveInRange( T val ) const
        {
            // Check upper bound
            while ( val >= m_upperBound )
                val -= getRange();

            // Check lower bound
            while ( val < m_lowerBound )
                val += getRange();

            return val;
        }
    };


} // end namespaces