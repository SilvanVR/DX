#pragma once
/**********************************************************************
    class: IBuffer (D3D11IBuffer.h)

    author: S. Hau
    date: December 6, 2017
**********************************************************************/

#include "../../D3D11.hpp"

namespace Core { namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class IBuffer
    {
    public:
        IBuffer( UINT bindFlags, D3D11_USAGE usage, U32 size, const void* pData = nullptr);
        virtual ~IBuffer() = 0;

        //----------------------------------------------------------------------
        //virtual void bind() = 0;

        //----------------------------------------------------------------------
        U32 getSize() const { return m_size; }

    protected:
        ID3D11Buffer*   m_pBuffer = nullptr;

    private:
        U32             m_size = 0;

        //----------------------------------------------------------------------
        IBuffer(const IBuffer& other)               = delete;
        IBuffer& operator = (const IBuffer& other)  = delete;
        IBuffer(IBuffer&& other)                    = delete;
        IBuffer& operator = (IBuffer&& other)       = delete;
    };


} } } // End namespaces