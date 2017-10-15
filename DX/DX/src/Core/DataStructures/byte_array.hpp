#pragma once

/**********************************************************************
    class: ByteArray (byte_array.hpp)

    author: S. Hau
    date: October 15, 2017

    See below for a class description.
**********************************************************************/

namespace Core {

    //**********************************************************************
    // Constructs an Byte-Array of the given size. The idea is to use it
    // as an intermediate storage buffer for arbitrary data and when the
    // buffer is (almost) full, use the whole data at once.
    //**********************************************************************
    template <Size m_capacity>
    class ByteArray
    {
    public:
        //----------------------------------------------------------------------
        ByteArray()
        {
            clear();
        }

        //----------------------------------------------------------------------
        bool isFull()                   const { return m_index == m_capacity; }
        bool hasEnoughPlace(Size size)  const { return (m_index + size) < m_capacity; }
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
        // Write a single byte into this buffer. Assert if not enough space.
        //----------------------------------------------------------------------
        void write(Byte byte)
        {
            ASSERT( !isFull() );
            m_data[m_index] = byte;
            m_index++;
        }

        //----------------------------------------------------------------------
        // Zero out whole buffer and begin next writing from front.
        //----------------------------------------------------------------------
        void clear()
        {
            memset(m_data, 0, sizeof(Byte) * m_capacity);
            m_index = 0;
        }

        //----------------------------------------------------------------------
        // Return a pointer to the byte array.
        //----------------------------------------------------------------------
        const Byte* data() const { return m_data; }

    private:
        Byte    m_data[m_capacity];
        Size    m_index;

        //----------------------------------------------------------------------
        ByteArray(const ByteArray& other)               = delete;
        ByteArray& operator = (const ByteArray& other)  = delete;
        ByteArray(ByteArray&& other)                    = delete;
        ByteArray& operator = (ByteArray&& other)       = delete;
    };

} // end namespaces
