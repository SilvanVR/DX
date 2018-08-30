#include "VkMappedUniformBuffer.h"
/**********************************************************************
    class: MappedUniformBuffer

    author: S. Hau
    date: August 29, 2018
**********************************************************************/

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    MappedUniformBuffer::MappedUniformBuffer( const ShaderUniformBufferDeclaration& bufferInfo, BufferUsage usage )
        : m_bufferInfo( bufferInfo )
    {
        m_CPUBuffer = (Byte*)_aligned_malloc( m_bufferInfo.getSize(), 16 );
        memset( m_CPUBuffer, 0, m_bufferInfo.getSize() );
        m_GPUBuffer.create( m_bufferInfo.getSize(), usage );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    bool MappedUniformBuffer::update( StringID name, const void* data )
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
    void MappedUniformBuffer::update( const void* data, U32 sizeInBytes )
    {
        m_GPUBuffer.update( data, sizeInBytes );
        m_gpuUpToDate = true;
    }

    //----------------------------------------------------------------------
    void MappedUniformBuffer::flush()
    {
        if ( not m_gpuUpToDate )
        {
            m_GPUBuffer.update( m_CPUBuffer, m_bufferInfo.getSize() );
            m_gpuUpToDate = true;
        }
    }

    //----------------------------------------------------------------------
    void MappedUniformBuffer::bind()
    {
        flush();
        g_vulkan.ctx.SetBuffer( m_GPUBuffer.buffer, m_bufferInfo.getBindingSet(), m_bufferInfo.getBindingSlot() );
    }

    //----------------------------------------------------------------------
    void MappedUniformBuffer::_FreeBuffers()
    {
        m_GPUBuffer.destroy();
        _aligned_free( m_CPUBuffer );
    }

} } // End namespaces