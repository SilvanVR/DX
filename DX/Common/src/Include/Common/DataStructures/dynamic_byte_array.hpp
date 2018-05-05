#pragma once

/**********************************************************************
    class: DynamicByteArray (dynamic_byte_array.hpp)

    author: S. Hau
    date: March 17, 2018

    See below for a class description.
**********************************************************************/

namespace Common {
  
    //**********************************************************************
    // Not used anymore. Used instead ArrayList<Byte>
    //**********************************************************************
    class DynamicByteArray
    {
    public:
        DynamicByteArray( Size sizeInBytes = 0) : m_size(sizeInBytes)
        {
            createBuffer(sizeInBytes);
        }
        ~DynamicByteArray(){ if(m_pData) SAFE_DELETE(m_pData); }

        void createBuffer(Size sizeInBytes)
        {
            m_size = sizeInBytes;
            if (m_pData) 
                SAFE_DELETE(m_pData);
            if (sizeInBytes != 0)
                m_pData = new Byte[sizeInBytes];
        }

        const Byte* data()      const { return m_pData; }
        Size        size()      const { return m_size; }


        //----------------------------------------------------------------------
        void push(U32 offset, void* pData, Size sizeInBytes)
        {
            ASSERT(offset < m_size);
            memcpy( m_pData + offset, pData, sizeInBytes );
        }

    private:
        Byte*   m_pData = nullptr;
        Size    m_size = 0;

        //----------------------------------------------------------------------
        DynamicByteArray(const DynamicByteArray& other)                 = delete;
        DynamicByteArray& operator = (const DynamicByteArray& other)    = delete;
        DynamicByteArray(DynamicByteArray&& other)                      = delete;
        DynamicByteArray& operator = (DynamicByteArray&& other)         = delete;
    };

} // end namespaces
