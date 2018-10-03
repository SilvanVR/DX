#pragma once
/**********************************************************************
    class: Buffer

    author: S. Hau
    date: August 27, 2018
**********************************************************************/

#include "Vulkan/Vulkan.hpp"
#include "../../enums.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Buffer
    {
    public:
        Buffer(U32 sizeInBytes, BufferUsage usage, VkBufferUsageFlags vkUsageFlags);
        ~Buffer();

        //----------------------------------------------------------------------
        void update(const void* data, U32 sizeInBytes);

        VkBuffer    buffer = VK_NULL_HANDLE;
        U32         size   = 0;
        BufferUsage usage  = BufferUsage::Unknown;

    private:
        void* pData = nullptr; // For persistent mapped buffers
    };

    //**********************************************************************
    class RingBuffer
    {
    public:
        RingBuffer(U32 sizeInBytes, BufferUsage usage, VkBufferUsageFlags vkUsageFlags, U32 numBuffers = 2);
        ~RingBuffer() = default;

        //----------------------------------------------------------------------
        void update(const void* data, U32 sizeInBytes);

        U32 getSize() const { return m_buffers.front()->size; }
        VkBuffer getBuffer() const;

    private:
        I32 m_bufferIndex = 0;
        ArrayList<std::unique_ptr<Buffer>> m_buffers;
    };

} } // End namespaces