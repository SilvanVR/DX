#pragma once
/**********************************************************************
    class: Buffer

    author: S. Hau
    date: August 27, 2018
**********************************************************************/

#include "Vulkan/Vulkan.hpp"
#include "enums.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Buffer
    {
    public:
        //----------------------------------------------------------------------
        void update(const void* data, U32 sizeInBytes);
        void destroy();

        VkBuffer    buffer = VK_NULL_HANDLE;
        U32         size = 0;
        BufferUsage usage = BufferUsage::Unknown;

    protected:
        void create(const void* data, U32 sizeInBytes, BufferUsage usage, VkBufferUsageFlags vkUsageFlags);
    };

    //**********************************************************************
    class VertexBuffer : public Buffer
    {
    public:
        //----------------------------------------------------------------------
        void create(const void* data, U32 sizeInBytes, BufferUsage usage);
    };

    //**********************************************************************
    class IndexBuffer : public Buffer
    {
    public:
        //----------------------------------------------------------------------
        void create(const void* data, U32 sizeInBytes, BufferUsage usage);
    };

    //**********************************************************************
    class UniformBuffer : public Buffer
    {
    public:
        //----------------------------------------------------------------------
        void create(const void* data, U32 sizeInBytes, BufferUsage usage);
    };

} } // End namespaces