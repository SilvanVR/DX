#pragma once
/**********************************************************************
    class: VertexBuffer, IndexBuffer, ConstantBuffer (D3D11Buffers.h)

    author: S. Hau
    date: December 6, 2017
**********************************************************************/

#include "D3D11IBuffer.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class VertexBuffer : public IBuffer
    {
    public:
        VertexBuffer(const void* pData, U32 size, BufferUsage usage);
        ~VertexBuffer() {}

        //----------------------------------------------------------------------
        void bind(U32 slot, U32 stride, U32 offset);

    private:
        //----------------------------------------------------------------------
        VertexBuffer(const VertexBuffer& other)               = delete;
        VertexBuffer& operator = (const VertexBuffer& other)  = delete;
        VertexBuffer(VertexBuffer&& other)                    = delete;
        VertexBuffer& operator = (VertexBuffer&& other)       = delete;
    };

    //**********************************************************************
    class IndexBuffer : public IBuffer
    {
    public:
        IndexBuffer(const void* pData, U32 size, BufferUsage usage);
        ~IndexBuffer() {}

        //----------------------------------------------------------------------
        void bind(DXGI_FORMAT format, U32 offset);

    private:
        //----------------------------------------------------------------------
        IndexBuffer(const IndexBuffer& other)               = delete;
        IndexBuffer& operator = (const IndexBuffer& other)  = delete;
        IndexBuffer(IndexBuffer&& other)                    = delete;
        IndexBuffer& operator = (IndexBuffer&& other)       = delete;
    };

    //**********************************************************************
    class ConstantBuffer : public IBuffer
    {
    public:
        ConstantBuffer(U32 size, BufferUsage usage);
        ~ConstantBuffer() {}

        //----------------------------------------------------------------------
        void bindToVertexShader(U32 slot) const;
        void bindToPixelShader(U32 slot) const;

    private:
        //----------------------------------------------------------------------
        ConstantBuffer(const ConstantBuffer& other)               = delete;
        ConstantBuffer& operator = (const ConstantBuffer& other)  = delete;
        ConstantBuffer(ConstantBuffer&& other)                    = delete;
        ConstantBuffer& operator = (ConstantBuffer&& other)       = delete;
    };

} } // End namespaces