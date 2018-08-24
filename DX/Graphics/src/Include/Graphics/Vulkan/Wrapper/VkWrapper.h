#pragma once
/**********************************************************************
    class: Several

    author: S. Hau
    date: August 23, 2018

    Contains several wrapper classes for vulkan objects, e.g. command-buffer,
    render-pass etc.
    P.S. All types are default copyable. This means there is nothing done in
    the deconstructor. To ensure that a resource will be deleted "release()" MUST be called.
**********************************************************************/

#include <Vulkan/vulkan.h>
#include "../Ext/vk_mem_alloc.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class IImage
    {
    public:
        //----------------------------------------------------------------------
        U32                     width   = 0;
        U32                     height  = 0;
        VkFormat                format  = VK_FORMAT_UNDEFINED;
        VkSampleCountFlagBits   samples = VK_SAMPLE_COUNT_1_BIT;
        VkImage                 img     = VK_NULL_HANDLE;
        VmaAllocation           mem     = VK_NULL_HANDLE;
        VkImageLayout           layout  = VK_IMAGE_LAYOUT_UNDEFINED;
    };

    //**********************************************************************
    class ColorImage : public IImage
    {
    public:
        //----------------------------------------------------------------------
        // IVulkanResource Interface
        //----------------------------------------------------------------------
        void release();

        //----------------------------------------------------------------------
        void create(U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VmaMemoryUsage memUsage);
        void create(VkImage image, U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples);
    };

    //**********************************************************************
    class DepthImage : public IImage
    {
    public:
        //----------------------------------------------------------------------
        // IVulkanResource Interface
        //----------------------------------------------------------------------
        void release();

        //----------------------------------------------------------------------
        void create(U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples);
    };

    //**********************************************************************
    class ImageView
    {
    public:
        //----------------------------------------------------------------------
        void create(ColorImage& image);
        void create(DepthImage& image);
        void release();

        //----------------------------------------------------------------------
        VkImageView view = VK_NULL_HANDLE;
        IImage*     img  = VK_NULL_HANDLE;
    };

    //**********************************************************************
    class RenderPass
    {
    public:
        struct AttachmentDescription
        {
            const IImage*       img;
            VkAttachmentLoadOp  loadOp;
            VkImageLayout       finalLayout;
        };

        //----------------------------------------------------------------------
        void create(const AttachmentDescription& color, const AttachmentDescription& depth);
        void release();

        VkRenderPass             renderPass = VK_NULL_HANDLE;
        ArrayList<VkImageLayout> newLayouts;
    };

    //**********************************************************************
    class Framebuffer
    {
    public:
        //----------------------------------------------------------------------
        void create(RenderPass* renderPass, ImageView* colorView, ImageView* depthView);
        void release();

        U32 getWidth();
        U32 getHeight();

        VkFramebuffer           framebuffer = VK_NULL_HANDLE;
        ArrayList<ImageView*>   attachments;
    };

    //**********************************************************************
    class CmdBuffer
    {
    public:
        //----------------------------------------------------------------------
        void create(U32 queueFamilyIndex, VkCommandPoolCreateFlags poolFlags = {}, VkFenceCreateFlags fenceFlags = {});
        void release();
        void begin(VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        void exec(VkQueue queue, VkSemaphore waitSemaphore = VK_NULL_HANDLE, VkSemaphore signalSemaphore = VK_NULL_HANDLE);
        void wait();
        void reset();

        void setImageLayout(ColorImage* img, VkImageLayout newLayout, VkAccessFlags srcAccess, VkAccessFlags dstAccess,
                                             VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

        void beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, std::array<VkClearValue, 2> clearValues);
        void endRenderPass();

        VkCommandPool   pool = VK_NULL_HANDLE;
        VkCommandBuffer cmd  = VK_NULL_HANDLE;
        VkFence         fence = VK_NULL_HANDLE;
    };


} } // End namespaces