#include "VkBuffer.h"
/**********************************************************************
    class: Buffer

    author: S. Hau
    date: August 27, 2018
**********************************************************************/

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // BUFFER
    //**********************************************************************

    //----------------------------------------------------------------------
    Buffer::Buffer( U32 sizeInBytes, BufferUsage updateFreq, VkBufferUsageFlags vkUsageFlags )
    {
        ASSERT( buffer == VK_NULL_HANDLE && "Buffer was already created." );

        this->size  = sizeInBytes;
        this->usage = updateFreq;

        VezMemoryFlagsBits memType = VEZ_MEMORY_GPU_ONLY;

        VezBufferCreateInfo bufferCreateInfo = {};
        switch (usage)
        {
        case BufferUsage::Immutable:
        case BufferUsage::LongLived:
            vkUsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT; memType = VEZ_MEMORY_GPU_ONLY;  break;
        case BufferUsage::Frequently:
            memType = VEZ_MEMORY_CPU_TO_GPU;
            break;
        }

        bufferCreateInfo.size = sizeInBytes;
        bufferCreateInfo.usage = vkUsageFlags;
        VALIDATE( vezCreateBuffer( g_vulkan.device, memType, &bufferCreateInfo, &buffer ) );

        if (usage == BufferUsage::Frequently)
            VALIDATE( vezMapBuffer( g_vulkan.device, buffer, 0, VK_WHOLE_SIZE, &pData ) );
    }

    //----------------------------------------------------------------------
    Buffer::~Buffer()
    {
        if (buffer)
        {
            vezDeviceWaitIdle( g_vulkan.device );
            if (usage == BufferUsage::Frequently)
                vezUnmapBuffer( g_vulkan.device, buffer );
            vezDestroyBuffer( g_vulkan.device, buffer );
            buffer = VK_NULL_HANDLE;
            size = 0;
            usage = BufferUsage::Unknown;
        }
    }

    //----------------------------------------------------------------------
    void Buffer::update( const void* data, U32 sizeInBytes )
    {
        ASSERT( buffer && "Buffer does not exist." );
        ASSERT( sizeInBytes <= size && "Update size too large!" );

        switch (usage)
        {
        case BufferUsage::Immutable:
        case BufferUsage::LongLived:
            VALIDATE( vezBufferSubData( g_vulkan.device, buffer, 0, sizeInBytes, data) );
            break;
        case BufferUsage::Frequently:
        {
            memcpy( pData, data, sizeInBytes );
            break;
        } 
        }
    }

    //**********************************************************************
    // CACHEDBUFFER
    //**********************************************************************

    //----------------------------------------------------------------------
    RingBuffer::RingBuffer( U32 sizeInBytes, BufferUsage usage, VkBufferUsageFlags vkUsageFlags, U32 numBuffers )
    {
        if (usage == BufferUsage::Immutable)
            numBuffers = 1;

        m_buffers.resize( numBuffers );
        for (U32 i = 0; i < numBuffers; i++)
            m_buffers[i] = std::make_unique<Buffer>( sizeInBytes, usage, vkUsageFlags );
    }

    //----------------------------------------------------------------------
    void RingBuffer::update( const void* data, U32 sizeInBytes )
    {
        m_bufferIndex = (m_bufferIndex + 1) % m_buffers.size();
        m_buffers[m_bufferIndex]->update( data, sizeInBytes );
    }

    //----------------------------------------------------------------------
    VkBuffer RingBuffer::getBuffer() const
    {
        return m_buffers[m_bufferIndex]->buffer;
    }

} } // End namespaces