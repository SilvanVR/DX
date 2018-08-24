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
        void create(U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VmaMemoryUsage memUsage);
        void create(VkImage image, U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples);
        void release();
    };

    //**********************************************************************
    class DepthImage : public IImage
    {
    public:
        //----------------------------------------------------------------------
        void create(U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples);
        void release();
    };

    //**********************************************************************
    class ImageView
    {
    public:
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
        void create(U32 queueFamilyIndex, VkCommandPoolCreateFlags poolFlags = {}, VkFenceCreateFlags fenceFlags = {});
        void release();
        void begin(VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        void exec(VkQueue queue, VkSemaphore waitSemaphore = VK_NULL_HANDLE, VkSemaphore signalSemaphore = VK_NULL_HANDLE);
        void wait();
        void reset();

        void setImageLayout(ColorImage* img, VkImageLayout newLayout, VkAccessFlags srcAccess, VkAccessFlags dstAccess,
                                             VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
        void resolveImage(ColorImage* src, ColorImage* dst);
        void resolveImage(DepthImage* src, DepthImage* dst);
        void setViewport(VkViewport viewport);
        void draw(U32 vertexCount, U32 instanceCount, U32 firstVertex, U32 firstInstance);

        void beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, std::array<VkClearValue, 2> clearValues);
        void endRenderPass();

        VkCommandPool   pool = VK_NULL_HANDLE;
        VkCommandBuffer cmd  = VK_NULL_HANDLE;
        VkFence         fence = VK_NULL_HANDLE;
    };

    //**********************************************************************
    class GraphicsPipeline
    {
    public:
        void addShaderModule(VkShaderStageFlagBits shaderStage, VkShaderModule shaderModule, CString entryPoint);
        void setVertexInputState(const ArrayList<VkVertexInputBindingDescription>& inputDesc, const ArrayList<VkVertexInputAttributeDescription>& attrDesc);
        void setInputAssemblyState(VkPrimitiveTopology topology, VkBool32 restartEnable = VK_FALSE);
        void setRasterizationState(VkBool32 depthClampEnable, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace);
        void setRasterizationState(VkBool32 depthBiasEnable, F32 depthBiasConstantFactor, F32 depthBiasClamp, F32 depthBiasSlopeFactor);
        void setMultisampleState(VkSampleCountFlagBits samples, VkBool32 alphaToCoverabeEnable, VkBool32 alphaToOneEnable);
        void setMultisampleState(VkBool32 sampleShadingEnable, F32 minSampleShading);
        void setDepthStencilState(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp);
        void setDepthStencilState(VkBool32 stencilTestEnable, VkStencilOpState front, VkStencilOpState back);
        void setDepthStencilState(VkBool32 depthBoundsTestEnable, F32 minBounds, F32 maxBounds);
        void addBlendAttachment(VkBlendFactor srcColorBlend, VkBlendFactor dstColorBlend, VkBlendOp colorBlendOp);
        void addBlendAttachment(VkBlendFactor srcColorBlend, VkBlendFactor dstColorBlend, VkBlendOp colorBlendOp,
                                VkBlendFactor srcAlphaBlend, VkBlendFactor dstAlphaBlend, VkBlendOp alphaBlendOp);
        void addDynamicState(VkDynamicState dynamicState);
        void buildPipeline(VkPipelineLayout layout, VkRenderPass renderPass, U32 subPass = 0);
        void release();

        VkPipeline pipeline;

    private:
        ArrayList<VkPipelineShaderStageCreateInfo>      m_shaderStages;

        ArrayList<VkVertexInputBindingDescription>      m_vertexInputBindingDesc;
        ArrayList<VkVertexInputAttributeDescription>    m_vertexInputAttrDesc;

        VkPipelineInputAssemblyStateCreateInfo          m_inputAssemblyState{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };

        VkPipelineViewportStateCreateInfo               m_viewportState{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };

        VkPipelineRasterizationStateCreateInfo          m_rasterizationState{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };

        VkPipelineMultisampleStateCreateInfo            m_multisampleState{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };

        VkPipelineDepthStencilStateCreateInfo           m_depthStencilState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

        ArrayList<VkPipelineColorBlendAttachmentState>  m_colorBlendAttachmentStates;
        VkPipelineColorBlendStateCreateInfo             m_colorBlendState{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };

        ArrayList<VkDynamicState>                       m_dynamicStates;

    };

} } // End namespaces