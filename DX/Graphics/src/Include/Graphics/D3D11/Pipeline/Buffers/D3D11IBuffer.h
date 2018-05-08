#pragma once
/**********************************************************************
    class: IBuffer (D3D11IBuffer.h)

    author: S. Hau
    date: December 6, 2017
**********************************************************************/

#include "../../D3D11.hpp"
#include "enums.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class IBuffer
    {
    public:
        IBuffer(UINT bindFlags, BufferUsage usage, U32 size, const void* pData = nullptr);
        virtual ~IBuffer() = 0;

        //----------------------------------------------------------------------
        // Updates the contents of the buffer.
        // @Params:
        // "pData": Data to be copied into the buffer.
        // "sizeInBytes": Size of the given data.
        // "offset": Offset in bytes from the beginning of this buffer.
        //----------------------------------------------------------------------
        void update(const void* pData, Size sizeInBytes, U32 offset = 0);


        //----------------------------------------------------------------------
        U32             getSize()           const { return m_size; }
        bool            isImmutable()       const { return m_usage == BufferUsage::Immutable; }
        BufferUsage     getBufferUsage()    const { return m_usage; }
        ID3D11Buffer*   getBuffer()         const { return m_pBuffer; }

    protected:
        ID3D11Buffer*   m_pBuffer = nullptr;

    private:
        U32             m_size = 0;
        BufferUsage     m_usage;

        //----------------------------------------------------------------------
        IBuffer(const IBuffer& other)               = delete;
        IBuffer& operator = (const IBuffer& other)  = delete;
        IBuffer(IBuffer&& other)                    = delete;
        IBuffer& operator = (IBuffer&& other)       = delete;
    };


} } // End namespaces