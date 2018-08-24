#include "VkWrapper.h"
/**********************************************************************
    class: Several

    author: S. Hau
    date: August 23, 2018
**********************************************************************/

#include "../Vulkan.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // Image
    //**********************************************************************

    //**********************************************************************
    // ColorImage
    //**********************************************************************

    //----------------------------------------------------------------------
    void ColorImage::create( U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VmaMemoryUsage memUsage )
    {
        this->width = width; this->height = height; this->format = format; this->samples = samples; this->layout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        createInfo.imageType     = VK_IMAGE_TYPE_2D;
        createInfo.format        = format;
        createInfo.extent        = { width, height, 1 };
        createInfo.mipLevels     = 1;
        createInfo.arrayLayers   = 1;
        createInfo.samples       = samples;
        createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage         = usage;
        createInfo.initialLayout = layout;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memUsage;
        vmaCreateImage( g_vulkan.allocator, &createInfo, &allocInfo, &img, &mem, nullptr );
    }

    //----------------------------------------------------------------------
    void ColorImage::create( VkImage image, U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples )
    {
        this->img = image; this->width = width; this->height = height; this->format = format; this->samples = samples; this->layout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    //----------------------------------------------------------------------
    void ColorImage::release()
    {
        if (img)
        {
            vkDeviceWaitIdle( g_vulkan.device );
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
        this->width = width; this->height = height; this->format = format; this->samples = samples; this->layout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        createInfo.imageType     = VK_IMAGE_TYPE_2D;
        createInfo.format        = format;
        createInfo.extent        = { width, height, 1 };
        createInfo.mipLevels     = 1;
        createInfo.arrayLayers   = 1;
        createInfo.samples       = samples;
        createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        createInfo.initialLayout = layout;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        vmaCreateImage( g_vulkan.allocator, &createInfo, &allocInfo, &img, &mem, nullptr );
    }

    //----------------------------------------------------------------------
    void DepthImage::release()
    {
        if (img)
        {
            vkDeviceWaitIdle( g_vulkan.device );
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
            vkDeviceWaitIdle( g_vulkan.device );
            vkDestroyImageView( g_vulkan.device, view, ALLOCATOR );
            view = VK_NULL_HANDLE;
        }
    }

    //**********************************************************************
    // RenderPass
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderPass::create( const AttachmentDescription& color, const AttachmentDescription& depth )
    {
        ArrayList<VkAttachmentDescription> attachments;

        VkSubpassDescription subPass{};
        subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkAttachmentReference colorAttachment{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        if (color.img)
        {
            subPass.colorAttachmentCount = 1;
            subPass.pColorAttachments = &colorAttachment;
            VkAttachmentDescription attachment{};
            attachment.format           = color.img->format;
            attachment.samples          = color.img->samples;
            attachment.loadOp           = color.loadOp;
            attachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout      = color.finalLayout;
            attachments.push_back( attachment );
            newLayouts.push_back( color.finalLayout );
        }

        VkAttachmentReference depthAttachment{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
        if (depth.img)
        {
            subPass.pDepthStencilAttachment = &depthAttachment;

            VkAttachmentDescription attachment{};
            attachment.format           = depth.img->format;
            attachment.samples          = depth.img->samples;
            attachment.loadOp           = depth.loadOp;
            attachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout      = depth.finalLayout;
            attachments.push_back( attachment );
            newLayouts.push_back( depth.finalLayout );
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
            vkDeviceWaitIdle( g_vulkan.device );
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
        ArrayList<VkImageView> views;
        VkFramebufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        if (colorView)
        {
            attachments.push_back( colorView );
            views.push_back( colorView->view );
        }
        if (depthView)
        {
            attachments.push_back( depthView );
            views.push_back( depthView->view );
        }
        createInfo.renderPass       = renderPass->renderPass;
        createInfo.attachmentCount  = (U32)views.size();
        createInfo.pAttachments     = views.data();
        createInfo.width            = getWidth();
        createInfo.height           = getHeight();
        createInfo.layers           = 1;

        vkCreateFramebuffer( g_vulkan.device, &createInfo, ALLOCATOR, &framebuffer );
    }

    //----------------------------------------------------------------------
    void Framebuffer::release()
    {
        if (framebuffer)
        {
            vkDeviceWaitIdle( g_vulkan.device );
            vkDestroyFramebuffer( g_vulkan.device, framebuffer, ALLOCATOR );
            framebuffer = VK_NULL_HANDLE;
        }
    }

    //----------------------------------------------------------------------
    U32 Framebuffer::getWidth()
    {
        return attachments.front()->img->width;
    }

    //----------------------------------------------------------------------
    U32 Framebuffer::getHeight()
    {
        return attachments.front()->img->height;
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
            vkDeviceWaitIdle( g_vulkan.device );
            vkDestroyCommandPool( g_vulkan.device, pool, ALLOCATOR );
            pool = VK_NULL_HANDLE;
        }
        if (fence)
        {
            vkDeviceWaitIdle( g_vulkan.device );
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
    void CmdBuffer::setImageLayout( ColorImage* img, VkImageLayout newLayout,
                                                     VkAccessFlags srcAccess, VkAccessFlags dstAccess,
                                                     VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask )
    {
        VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.srcAccessMask       = srcAccess;
        barrier.dstAccessMask       = dstAccess;
        barrier.oldLayout           = img->layout;
        barrier.newLayout           = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; 
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image               = img->img;
        barrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0,  VK_REMAINING_ARRAY_LAYERS };

        vkCmdPipelineBarrier( cmd, srcStageMask, dstStageMask, 0,
                               0, VK_NULL_HANDLE,
                               0, VK_NULL_HANDLE,
                               1, &barrier);
        img->layout = newLayout;
    }

    //----------------------------------------------------------------------
    void CmdBuffer::beginRenderPass( RenderPass* renderPass, Framebuffer* fbo, std::array<VkClearValue, 2> clearValues )
    {
        VkRenderPassBeginInfo beginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        beginInfo.renderPass        = renderPass->renderPass;
        beginInfo.framebuffer       = fbo->framebuffer;
        beginInfo.renderArea.extent = { fbo->getWidth(), fbo->getHeight() };
        beginInfo.clearValueCount   = (U32)clearValues.size();
        beginInfo.pClearValues      = clearValues.data();

        vkCmdBeginRenderPass( cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE );

        // Update layout from images in the fbo
        for (I32 i = 0; i < fbo->attachments.size(); ++i)
            fbo->attachments[i]->img->layout = renderPass->newLayouts[i];
    }

    //----------------------------------------------------------------------
    void CmdBuffer::endRenderPass()
    {
        vkCmdEndRenderPass( cmd );
    }


} } // End namespaces