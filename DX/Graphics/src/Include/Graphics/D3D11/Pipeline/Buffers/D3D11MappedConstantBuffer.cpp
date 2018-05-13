#include "D3D11MappedConstantBuffer.h"
/**********************************************************************
    class: MappedConstantBuffer (D3D11MappedConstantBuffer.cpp)

    author: S. Hau
    date: May 13, 2018
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    MappedConstantBuffer::MappedConstantBuffer( const ConstantBufferInfo& bufferInfo, BufferUsage usage ) 
        : m_bufferInfo( bufferInfo )
    {
        m_CPUBuffer = (Byte*)_aligned_malloc( m_bufferInfo.sizeInBytes, 16 );
        m_GPUBuffer = new D3D11::ConstantBuffer( (U32)m_bufferInfo.sizeInBytes, usage );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

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
    void MappedConstantBuffer::update( const void* data, Size sizeInBytes )
    {
        m_GPUBuffer->update( data, sizeInBytes );
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
    void MappedConstantBuffer::_FreeBuffers()
    {
        SAFE_DELETE( m_GPUBuffer );
        _aligned_free( m_CPUBuffer );
    }

} } // End namespaces