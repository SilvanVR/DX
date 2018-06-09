#pragma once

/**********************************************************************
    class: _ByteArray (byte_array.hpp)

    author: S. Hau
    date: October 15, 2017

    See below for a class description.
**********************************************************************/

namespace Common {

    //**********************************************************************
    // Constructs an Byte-Array of the given size. The idea is to use it
    // as an intermediate storage buffer for arbitrary data and when the
    // buffer is (almost) full, use the whole data at once.
    //**********************************************************************
    template <typename T, Size m_capacity>
    class _ByteArray
    {
    public:
        //----------------------------------------------------------------------
        _ByteArray()
        {
            clear();
        }

        //----------------------------------------------------------------------
        bool isFull()                   const { return m_index == m_capacity; }
        bool isEmpty()                  const { return m_index == 0; }
        bool hasEnoughPlace(Size size)  const { return (m_index + size) <= m_capacity; }
        Size capacity()                 const { return m_capacity; }
        Size size()                     const { return m_index; }

        //----------------------------------------------------------------------
        // Write the given string into this buffer. Assert if not enough space.
        //----------------------------------------------------------------------
        void write(const char* str)
        {
            Size len = strlen( str );
            ASSERT( hasEnoughPlace( len ) );

            memcpy( m_data + m_index, str, len );
            m_index += len;
        }

        //----------------------------------------------------------------------
        // Write the given amount of bytes into this buffer. Assert if not enough space.
        //----------------------------------------------------------------------
        void write( const char* str, Size bytes )
        {
            ASSERT( hasEnoughPlace(bytes) );

            memcpy( m_data + m_index, str, bytes );
            m_index += bytes;
        }

        //----------------------------------------------------------------------
        // Write a single byte into this buffer. Assert if not enough space.
        //----------------------------------------------------------------------
        void write( T byte )
        {
            ASSERT( !isFull() );
            m_data[m_index] = byte;
            m_index++;
        }

        //----------------------------------------------------------------------
        // Erase "numElements" from the end of the buffer.
        //----------------------------------------------------------------------
        void erase( Size numElements )
        {
            numElements = (numElements > m_index ? m_index : numElements);
            m_index -= numElements;

            memset( m_data + m_index, 0, numElements );
        }

        //----------------------------------------------------------------------
        // Zero out whole buffer and begin next writing from front.
        //----------------------------------------------------------------------
        void clear()
        {
            memset( m_data, 0, sizeof(T) * m_capacity );
            m_index = 0;
        }

        //----------------------------------------------------------------------
        // Return a pointer to the byte array.
        //----------------------------------------------------------------------
        const T* data() const { return m_data; }

    private:
        T       m_data[m_capacity];
        Size    m_index;

        //----------------------------------------------------------------------
        _ByteArray(const _ByteArray& other)               = delete;
        _ByteArray& operator = (const _ByteArray& other)  = delete;
        _ByteArray(_ByteArray&& other)                    = delete;
        _ByteArray& operator = (_ByteArray&& other)       = delete;
    };

    template <Size cap>
    using ByteArray = _ByteArray<Byte, cap>;

    template <Size cap>
    using CharArray = _ByteArray<char, cap>;

} // end namespaces
