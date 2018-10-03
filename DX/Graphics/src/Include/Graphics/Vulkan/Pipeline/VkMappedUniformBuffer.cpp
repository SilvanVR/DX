#include "VkMappedUniformBuffer.h"
/**********************************************************************
    class: MappedUniformBuffer

    author: S. Hau
    date: August 29, 2018
**********************************************************************/

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // MappedUniformBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    MappedUniformBuffer::MappedUniformBuffer( const ShaderUniformBufferDeclaration& bufferInfo, BufferUsage usage )
        : m_bufferInfo( bufferInfo )
    {
        m_CPUBuffer = (Byte*)_aligned_malloc( m_bufferInfo.getSize(), 16 );
        memset( m_CPUBuffer, 0, m_bufferInfo.getSize() );
        m_GPUBuffer = std::make_unique<Buffer>( m_bufferInfo.getSize(), usage, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT );
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
        m_GPUBuffer->update( data, sizeInBytes );
        m_gpuUpToDate = true;
    }

    //----------------------------------------------------------------------
    void MappedUniformBuffer::flush()
    {
        if ( not m_gpuUpToDate )
        {
            m_GPUBuffer->update( m_CPUBuffer, m_bufferInfo.getSize() );
            m_gpuUpToDate = true;
        }
    }

    //----------------------------------------------------------------------
    void MappedUniformBuffer::bind()
    {
        flush();
        g_vulkan.ctx.SetBuffer( m_GPUBuffer->buffer, m_bufferInfo.getBindingSet(), m_bufferInfo.getBindingSlot() );
    }

    //**********************************************************************
    // CachedMappedUniformBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    CachedMappedUniformBuffer::~CachedMappedUniformBuffer()
    {
        for (auto& ubo : m_mappedUBOs)
            SAFE_DELETE( ubo );
    }

    //----------------------------------------------------------------------
    void CachedMappedUniformBuffer::newFrame()
    {
        m_bufferIndex = 0;
    }

    //----------------------------------------------------------------------
    void CachedMappedUniformBuffer::beginBuffer()
    {
        if (m_bufferIndex == m_mappedUBOs.size())
            m_mappedUBOs.push_back( new MappedUniformBuffer( m_bufferDecl, m_bufferUsage ) );
        m_bufferIndex++;
    }

    //----------------------------------------------------------------------
    bool CachedMappedUniformBuffer::update( StringID name, const void* data )
    {
        I32 index = m_bufferIndex - 1;
        return m_mappedUBOs[index]->update( name, data );
    }

    //----------------------------------------------------------------------
    void CachedMappedUniformBuffer::bind()
    {
        I32 index = m_bufferIndex - 1;
        m_mappedUBOs[index]->bind();
    }

} } // End namespaces