#pragma once
/**********************************************************************
    class: VertexBuffer, IndexBuffer, ConstantBuffer (D3D11Buffers.h)

    author: S. Hau
    date: December 6, 2017
**********************************************************************/

#include "D3D11IBuffer.h"
#include "../Shaders/D3D11ShaderBase.h"

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

    //**********************************************************************
    // This class encapsulates a constant buffer from a shader buffer description,
    // so you can update properties by name and flush the whole data in one go to the gpu.
    //**********************************************************************
    class MappedConstantBuffer
    {
    public:
        MappedConstantBuffer() = default;
        MappedConstantBuffer(const ConstantBufferInfo& bufferInfo, BufferUsage usage);
        ~MappedConstantBuffer() { _FreeBuffers(); }

        //----------------------------------------------------------------------
        bool gpuIsUpToDate() const { return m_gpuUpToDate; }
        bool hasBuffer()     const { return m_GPUBuffer != nullptr; }

        //----------------------------------------------------------------------
        // Update the given uniform 'name' with the given data. !! ONLY ON THE CPU !!
        // @Return:
        //  False if uniform with the given name does not exist.
        //----------------------------------------------------------------------
        bool update(StringID name, const void* data);

        //----------------------------------------------------------------------
        // Sends the whole buffer on the cpu to the gpu. (This does nothing if gpu data is up to date)
        //---------------------------------------------------------------------
        void flush();

        //----------------------------------------------------------------------
        // Bind the constant buffer to the device context. Might flush it before
        // if the data on the gpu is not up to date.
        // @Params:
        // "shaderType": The shader to bind the constant buffer.
        //----------------------------------------------------------------------
        void bind(ShaderType shaderType);

        //----------------------------------------------------------------------
        // Resizes the buffer on cpu & gpu.
        // @Params:
        // "bufferInfo": New buffer info describing this constant buffer.
        // "usage": New buffer usage.
        //----------------------------------------------------------------------
        void resize(const ConstantBufferInfo& bufferInfo, BufferUsage usage);

    private:
        ConstantBufferInfo  m_bufferInfo;
        Byte*               m_CPUBuffer     = nullptr;
        ConstantBuffer*     m_GPUBuffer     = nullptr;
        bool                m_gpuUpToDate   = false;

        //----------------------------------------------------------------------
        void _CreateBuffers(BufferUsage usage);
        void _FreeBuffers();
    };

} } // End namespaces