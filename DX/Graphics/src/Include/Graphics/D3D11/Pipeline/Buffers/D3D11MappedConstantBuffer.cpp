#include "D3D11MappedConstantBuffer.h"
/**********************************************************************
    class: MappedConstantBuffer (D3D11MappedConstantBuffer.cpp)

    author: S. Hau
    date: May 13, 2018
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    MappedConstantBuffer::MappedConstantBuffer( const ShaderUniformBufferDeclaration& bufferInfo, BufferUsage usage )
        : m_bufferInfo( bufferInfo )
    {
        m_CPUBuffer = (Byte*)_aligned_malloc( m_bufferInfo.getSize(), 16 );
        m_GPUBuffer = new D3D11::ConstantBuffer( m_bufferInfo.getSize(), usage );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    bool MappedConstantBuffer::update( StringID name, const void* data )
    {
        for (auto& member : m_bufferInfo.getMembers())
        {
            if (member.getName() == name)
            {
                memcpy( &m_CPUBuffer[member.getOffset()], data, member.getSize() );
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
            m_GPUBuffer->update( m_CPUBuffer, m_bufferInfo.getSize() );
            m_gpuUpToDate = true;
        }
    }

    //----------------------------------------------------------------------
    void MappedConstantBuffer::bind( ShaderType shaderType )
    {
        flush();

        switch (shaderType)
        {
        case ShaderType::Vertex:    m_GPUBuffer->bindToVertexShader( m_bufferInfo.getBindingSlot() ); break;
        case ShaderType::Fragment:  m_GPUBuffer->bindToPixelShader( m_bufferInfo.getBindingSlot() ); break;
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