#include "VkWrapper.h"
/**********************************************************************
    class: Several

    author: S. Hau
    date: August 23, 2018
**********************************************************************/

#include "../Vulkan.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // ColorImage
    //**********************************************************************

    //----------------------------------------------------------------------
    void ColorImage::create( U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VmaMemoryUsage memUsage )
    {
        this->width = width; this->height = height; this->format = format; this->samples = samples;

        VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        createInfo.imageType     = VK_IMAGE_TYPE_2D;
        createInfo.format        = format;
        createInfo.extent        = { width, height, 1 };
        createInfo.mipLevels     = 1;
        createInfo.arrayLayers   = 1;
        createInfo.samples       = samples;
        createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage         = usage;
        createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memUsage;
        vmaCreateImage( g_vulkan.allocator, &createInfo, &allocInfo, &img, &mem, nullptr );
    }

    //----------------------------------------------------------------------
    void ColorImage::create( VkImage image, U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples )
    {
        this->img = image; this->width = width; this->height = height; this->format = format; this->samples = samples;
    }

    //----------------------------------------------------------------------
    void ColorImage::release()
    {
        if (img)
        {
            vmaDestroyImage( g_vulkan.allocator, img, mem );
            img = VK_NULL_HANDLE;
        }
    }

    //**********************************************************************
    // DepthImage
    //**********************************************************************

    //----------------------------------------------------------------------
    void DepthImage::create( U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples )
    {
        this->width = width; this->height = height; this->format = format; this->samples = samples;

        VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        createInfo.imageType     = VK_IMAGE_TYPE_2D;
        createInfo.format        = format;
        createInfo.extent        = { width, height, 1 };
        createInfo.mipLevels     = 1;
        createInfo.arrayLayers   = 1;
        createInfo.samples       = samples;
        createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        vmaCreateImage( g_vulkan.allocator, &createInfo, &allocInfo, &img, &mem, nullptr );
    }

    //----------------------------------------------------------------------
    void DepthImage::release()
    {
        if (img)
        {
            vmaDestroyImage( g_vulkan.allocator, img, mem );
            img = VK_NULL_HANDLE;
        }
    }

    //**********************************************************************
    // ImageView
    //**********************************************************************

    //----------------------------------------------------------------------
    void ImageView::create( ColorImage& image )
    {
        img = &image;

        VkImageViewCreateInfo viewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewCreateInfo.image                        = image.img;
        viewCreateInfo.viewType                     = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format                       = image.format;
        viewCreateInfo.subresourceRange.aspectMask  = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.layerCount  = VK_REMAINING_ARRAY_LAYERS;
        viewCreateInfo.subresourceRange.levelCount  = VK_REMAINING_MIP_LEVELS;
        vkCreateImageView( g_vulkan.device, &viewCreateInfo, ALLOCATOR, &view );
    }

    //----------------------------------------------------------------------
    void ImageView::create( DepthImage& image )
    {
        img = &image;

        VkImageViewCreateInfo viewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewCreateInfo.image                        = image.img;
        viewCreateInfo.viewType                     = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format                       = image.format;
        viewCreateInfo.subresourceRange.aspectMask  = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewCreateInfo.subresourceRange.layerCount  = VK_REMAINING_ARRAY_LAYERS;
        viewCreateInfo.subresourceRange.levelCount  = VK_REMAINING_MIP_LEVELS;
        vkCreateImageView( g_vulkan.device, &viewCreateInfo, ALLOCATOR, &view );
    }

    //----------------------------------------------------------------------
    void ImageView::release()
    {
        if (view)
        {
            vkDestroyImageView( g_vulkan.device, view, ALLOCATOR );
            view = VK_NULL_HANDLE;
        }
    }

    //**********************************************************************
    // RenderPass
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderPass::create( const ColorImage* color, const DepthImage* depth )
    {
        ArrayList<VkAttachmentDescription> attachments;

        VkSubpassDescription subPass{};
        subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkAttachmentReference colorAttachment{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        if (color)
        {
            subPass.colorAttachmentCount = 1;
            subPass.pColorAttachments = &colorAttachment;
            VkAttachmentDescription attachment{};
            attachment.format           = color->format;
            attachment.samples          = color->samples;
            attachment.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout      = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            attachments.push_back( attachment );
        }

        VkAttachmentReference depthAttachment{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
        if (depth)
        {
            subPass.pDepthStencilAttachment = &depthAttachment;

            VkAttachmentDescription attachment{};
            attachment.format           = depth->format;
            attachment.samples          = depth->samples;
            attachment.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout      = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            attachments.push_back( attachment );
        }

        std::array<VkSubpassDependency, 2> dependencies{};
        dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass      = 0;
        dependencies[0].srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass      = 0;
        dependencies[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo createInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        createInfo.attachmentCount  = (U32)attachments.size();
        createInfo.pAttachments     = attachments.data();
        createInfo.subpassCount     = 1;
        createInfo.pSubpasses       = &subPass;
        createInfo.dependencyCount  = (U32)dependencies.size();
        createInfo.pDependencies    = dependencies.data();

        vkCreateRenderPass( g_vulkan.device, &createInfo, ALLOCATOR, &renderPass );
    }

    //----------------------------------------------------------------------
    void RenderPass::release()
    {
        if (renderPass)
        {
            vkDestroyRenderPass( g_vulkan.device, renderPass, ALLOCATOR );
            renderPass = VK_NULL_HANDLE;
        }
    }

    //**********************************************************************
    // Framebuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    void Framebuffer::create( RenderPass* renderPass, ImageView* colorView, ImageView* depthView )
    {
        ArrayList<VkImageView> attachments;

        VkFramebufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        if (colorView)
        {
            attachments.push_back( colorView->view );
            createInfo.width = colorView->img->width;
            createInfo.height = colorView->img->height;
        }
        if (depthView)
        {
            attachments.push_back( depthView->view );
            createInfo.width = depthView->img->width;
            createInfo.height = depthView->img->height;
        }

        createInfo.renderPass       = renderPass->renderPass;
        createInfo.attachmentCount  = (U32)attachments.size();
        createInfo.pAttachments     = attachments.data();

        vkCreateFramebuffer( g_vulkan.device, &createInfo, ALLOCATOR, &framebuffer );
    }

    //----------------------------------------------------------------------
    void Framebuffer::release()
    {
        if (framebuffer)
        {
            vkDestroyFramebuffer( g_vulkan.device, framebuffer, ALLOCATOR );
            framebuffer = VK_NULL_HANDLE;
        }
    }

    //**********************************************************************
    // CommandBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    void CmdBuffer::create( U32 queueFamilyIndex, VkCommandPoolCreateFlags poolFlags, VkFenceCreateFlags fenceFlags )
    {
        VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        createInfo.flags = poolFlags;
        createInfo.queueFamilyIndex = queueFamilyIndex;
        vkCreateCommandPool( g_vulkan.device, &createInfo, ALLOCATOR, &pool );

        VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocateInfo.commandPool = pool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;
        vkAllocateCommandBuffers( g_vulkan.device, &allocateInfo, &cmd );

        VkFenceCreateInfo fenceCreateInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fenceCreateInfo.flags = fenceFlags;
        vkCreateFence( g_vulkan.device, &fenceCreateInfo, ALLOCATOR, &fence );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::release()
    {
        if (pool)
        {
            vkDestroyCommandPool( g_vulkan.device, pool, ALLOCATOR );
            pool = VK_NULL_HANDLE;
        }
        if (fence)
        {
            vkDestroyFence( g_vulkan.device, fence, ALLOCATOR );
            fence = VK_NULL_HANDLE;
        }
        cmd = VK_NULL_HANDLE;
    }

    //----------------------------------------------------------------------
    void CmdBuffer::begin( VkCommandBufferUsageFlags flags )
    {
        VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = flags;
        vkBeginCommandBuffer( cmd, &beginInfo );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::exec( VkQueue queue, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore )
    {
        vkEndCommandBuffer( cmd );

        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        if (waitSemaphore != VK_NULL_HANDLE)
        {
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = &waitSemaphore;
            submitInfo.pWaitDstStageMask    = &waitDstStageMask;
        }
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &cmd;
        if (signalSemaphore != VK_NULL_HANDLE)
        {
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &signalSemaphore;
        }

        vkQueueSubmit( queue, 1, &submitInfo, fence );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::wait()
    {
        const uint32_t timeoutNs = 1000 * 1000 * 1000;
        vkWaitForFences( g_vulkan.device, 1, &fence, VK_FALSE, timeoutNs );
        vkResetFences( g_vulkan.device, 1, &fence );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::reset()
    {
        VkCommandBufferResetFlags flags{};
        vkResetCommandBuffer( cmd, flags );
    }

    //----------------------------------------------------------------------
    void CmdBuffer::setImageLayout( ColorImage* img, VkImageLayout oldLayout, VkImageLayout newLayout,
                                                     VkAccessFlags srcAccess, VkAccessFlags dstAccess,
                                                     VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask )
    {
        VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.srcAccessMask       = srcAccess;
        barrier.dstAccessMask       = dstAccess;
        barrier.oldLayout           = oldLayout;
        barrier.newLayout           = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; 
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image               = img->img;
        barrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0,  VK_REMAINING_ARRAY_LAYERS };

        vkCmdPipelineBarrier( cmd, srcStageMask, dstStageMask, 0,
                               0, VK_NULL_HANDLE,
                               0, VK_NULL_HANDLE,
                               1, &barrier);
    }

} } // End namespaces