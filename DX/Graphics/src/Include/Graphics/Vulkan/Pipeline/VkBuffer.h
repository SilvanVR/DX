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
        //----------------------------------------------------------------------
        void update(const void* data, U32 sizeInBytes);
        void destroy();

        VkBuffer    buffer = VK_NULL_HANDLE;
        U32         size = 0;
        BufferUsage usage = BufferUsage::Unknown;

    protected:
        void create(U32 sizeInBytes, BufferUsage usage, VkBufferUsageFlags vkUsageFlags, const void* pInitialData = nullptr);
    };

    //**********************************************************************
    class VertexBuffer : public Buffer
    {
    public:
        //----------------------------------------------------------------------
        void create(U32 sizeInBytes, BufferUsage usage, const void* pInitialData = nullptr);
    };

    //**********************************************************************
    class IndexBuffer : public Buffer
    {
    public:
        //----------------------------------------------------------------------
        void create(U32 sizeInBytes, BufferUsage usage, const void* pInitialData = nullptr);
    };

    //**********************************************************************
    class UniformBuffer : public Buffer
    {
    public:
        //----------------------------------------------------------------------
        void create(U32 sizeInBytes, BufferUsage usage, const void* pInitialData = nullptr);
    };

} } // End namespaces