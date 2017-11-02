#pragma once
/**********************************************************************
    class: FiniteRange (finite_range.hpp)

    author: S. Hau
    date: November 02, 2017
**********************************************************************/

//**********************************************************************
// Represents a finite range of a value.
// It is guaranteed that the value never exceeds or fall below the bounds.
// Possible range: [min, max) - (max is always equal min)
// Do not UNSIGNED types. (Or a class with unsigned types in it)
// Limited support for different operators yet.
//**********************************************************************
template < class T >
class FiniteRange
{
public:
    FiniteRange(T lowerBound = 0, T upperBound = 0) 
        : m_lowerBound( lowerBound ), m_upperBound( upperBound ) 
    {
        ASSERT( m_lowerBound < m_upperBound && "Min-Bound must be less than Max-Bound." );
    }
    ~FiniteRange() = default;

    //----------------------------------------------------------------------
    T value() const { return m_value; }
    template <class T2> explicit operator T2 () const { return m_value; }

    //----------------------------------------------------------------------
    void    setLowerBound(T minBound) { m_lowerBound = minBound; ASSERT( m_lowerBound < m_upperBound && "Min-Bound must be less than Max-Bound." ); }
    void    setUpperBound(T maxBound) { m_upperBound = maxBound; ASSERT( m_lowerBound < m_upperBound && "Min-Bound must be less than Max-Bound." ); }

    T       getLowerBound() const { return m_lowerBound; }
    T       getUpperBound() const { return m_upperBound; }

    //----------------------------------------------------------------------
    FiniteRange& operator += (const T& other) { _Add( other ); return (*this); }
    FiniteRange& operator -= (const T& other) { _Add( -other ); return (*this); }

private:
    T m_value       = 0;
    T m_lowerBound  = 0;
    T m_upperBound  = 0;

    //----------------------------------------------------------------------
    void _Add( T value )
    {
        m_value += value;

        // Check upper bound
        while ( m_value >= m_upperBound )
            m_value -= (m_upperBound - m_lowerBound);

        // Check lower bound
        while ( m_value < m_lowerBound )
            m_value += (m_upperBound - m_lowerBound);
    }
};
