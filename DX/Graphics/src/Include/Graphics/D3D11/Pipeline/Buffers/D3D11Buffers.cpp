#include "D3D11Buffers.h"
/**********************************************************************
    class: VertexBuffer, IndexBuffer, ConstantBuffer (D3D11Buffers.cpp)

    author: S. Hau
    date: December 6, 2017
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    // VertexBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    VertexBuffer::VertexBuffer( const void* pData, U32 size, BufferUsage usage )
        : IBuffer( D3D11_BIND_VERTEX_BUFFER, usage, size, pData )
    {
    }

    //----------------------------------------------------------------------
    void VertexBuffer::bind( U32 slot, U32 stride, U32 offset )
    {
        g_pImmediateContext->IASetVertexBuffers( slot, 1, &m_pBuffer, &stride, &offset );
    }

    //**********************************************************************
    // IndexBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    IndexBuffer::IndexBuffer( const void* pData, U32 size, BufferUsage usage )
        : IBuffer( D3D11_BIND_INDEX_BUFFER, usage, size, pData )
    {
    }

    //----------------------------------------------------------------------
    void IndexBuffer::bind( DXGI_FORMAT format, U32 offset )
    {
        g_pImmediateContext->IASetIndexBuffer( m_pBuffer, format, offset );
    }

    //**********************************************************************
    // ConstantBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    ConstantBuffer::ConstantBuffer( U32 size, BufferUsage usage )
        : IBuffer( D3D11_BIND_CONSTANT_BUFFER, usage, size )
    {
    }

    //----------------------------------------------------------------------
    void ConstantBuffer::bindToVertexShader( U32 slot ) const
    {
        g_pImmediateContext->VSSetConstantBuffers( slot, 1, &m_pBuffer );
    }

    //----------------------------------------------------------------------
    void ConstantBuffer::bindToPixelShader( U32 slot ) const
    {
        g_pImmediateContext->PSSetConstantBuffers( slot, 1, &m_pBuffer );
    }

    //**********************************************************************
    // MappedConstantBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    MappedConstantBuffer::MappedConstantBuffer( const ConstantBufferInfo& bufferInfo, BufferUsage usage ) 
        : m_bufferInfo( bufferInfo )
    {
        _CreateBuffers( usage );
    }

    //----------------------------------------------------------------------
    bool MappedConstantBuffer::update( StringID name, const void* data )
    {
        for (auto& member : m_bufferInfo.members)
        {
            if (member.name == name)
            {
                memcpy( &m_CPUBuffer[member.offset], data, member.size );
                m_gpuUpToDate = false;
                return true;
            }
        }
        return false;
    }

    //----------------------------------------------------------------------
    void MappedConstantBuffer::flush()
    {
        if ( not m_gpuUpToDate )
        {
            m_GPUBuffer->update( m_CPUBuffer, m_bufferInfo.sizeInBytes );
            m_gpuUpToDate = true;
        }
    }

    //----------------------------------------------------------------------
    void MappedConstantBuffer::bind( ShaderType shaderType )
    {
        flush();

        switch (shaderType)
        {
        case ShaderType::Vertex:    m_GPUBuffer->bindToVertexShader( m_bufferInfo.slot ); break;
        case ShaderType::Fragment:  m_GPUBuffer->bindToPixelShader( m_bufferInfo.slot ); break;
        default: ASSERT( false );
        }
    }

    //----------------------------------------------------------------------
    void MappedConstantBuffer::resize( const ConstantBufferInfo& bufferInfo, BufferUsage usage )
    {
        m_bufferInfo = bufferInfo;
        _FreeBuffers();
        _CreateBuffers( usage );
    }

    //----------------------------------------------------------------------
    void MappedConstantBuffer::_CreateBuffers( BufferUsage usage )
    {
        ASSERT( m_GPUBuffer == nullptr );
        m_CPUBuffer = (Byte*)_aligned_malloc( m_bufferInfo.sizeInBytes, 16 );
        m_GPUBuffer = new D3D11::ConstantBuffer( (U32)m_bufferInfo.sizeInBytes, usage );
    }

    //----------------------------------------------------------------------
    void MappedConstantBuffer::_FreeBuffers()
    {
        SAFE_DELETE( m_GPUBuffer );
        _aligned_free( m_CPUBuffer );
    }

} } // End namespaces