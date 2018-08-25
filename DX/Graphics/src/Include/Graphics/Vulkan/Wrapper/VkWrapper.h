#pragma once
/**********************************************************************
    class: Several

    author: S. Hau
    date: August 23, 2018

    Contains several wrapper classes for vulkan objects, e.g. command-buffer,
    render-pass etc.
**********************************************************************/

#include <Vulkan/vulkan.h>
#include "../Ext/vk_mem_alloc.h"

namespace Graphics { namespace Vulkan {
 
    //**********************************************************************
    class IVulkanResource
    {
    public:
        IVulkanResource() = default;
        virtual ~IVulkanResource() {}

        inline bool release();
        inline void addRef();
    private:
        U32 m_useCount = 1;
        NULL_COPY_AND_ASSIGN(IVulkanResource)
    };

    //**********************************************************************
    class IImage : public IVulkanResource
    {
    public:
        IImage(U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples, VkImageLayout layout)
            : width(width), height(height), format(format), samples(samples), layout(layout) {}

        //----------------------------------------------------------------------
        U32                     width   = 0;
        U32                     height  = 0;
        VkFormat                format  = VK_FORMAT_UNDEFINED;
        VkSampleCountFlagBits   samples = VK_SAMPLE_COUNT_1_BIT;
        VkImageLayout           layout  = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImage                 img     = VK_NULL_HANDLE;
        VmaAllocation           mem     = VK_NULL_HANDLE;
    };

    //**********************************************************************
    class ColorImage : public IImage
    {
    public:
        ColorImage(U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VmaMemoryUsage memUsage);
        ColorImage(VkImage image, U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples);
        ~ColorImage();
    };

    //**********************************************************************
    class DepthImage : public IImage
    {
    public:
        //----------------------------------------------------------------------
        DepthImage(U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples);
        ~DepthImage();
    };

    //**********************************************************************
    class ImageView : public IVulkanResource
    {
    public:
        ImageView(ColorImage* image);
        ImageView(DepthImage* image);
        ~ImageView();

        //----------------------------------------------------------------------
        VkImageView view = VK_NULL_HANDLE;
        IImage*     img  = VK_NULL_HANDLE;
    };

    //**********************************************************************
    class RenderPass : public IVulkanResource
    {
    public:
        struct AttachmentDescription
        {
            const IImage*       img;
            VkAttachmentLoadOp  loadOp;
            VkImageLayout       finalLayout;
        };

        //----------------------------------------------------------------------
        RenderPass(const AttachmentDescription& color, const AttachmentDescription& depth);
        ~RenderPass();

        VkRenderPass             renderPass = VK_NULL_HANDLE;
        ArrayList<VkImageLayout> newLayouts;
    };

    //**********************************************************************
    class Framebuffer : public IVulkanResource
    {
    public:
        Framebuffer(RenderPass* renderPass, ImageView* colorView, ImageView* depthView);
        ~Framebuffer();

        U32 getWidth();
        U32 getHeight();

        VkFramebuffer           framebuffer = VK_NULL_HANDLE;
        ArrayList<ImageView*>   attachments;
    };

    //**********************************************************************
    class CmdBuffer : public IVulkanResource
    {
    public:
        CmdBuffer(U32 queueFamilyIndex, VkCommandPoolCreateFlags poolFlags = {}, VkFenceCreateFlags fenceFlags = {});
        ~CmdBuffer();

        void begin(VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        void exec(VkQueue queue, VkSemaphore waitSemaphore = VK_NULL_HANDLE, VkSemaphore signalSemaphore = VK_NULL_HANDLE);
        void wait();
        void reset();

        void setImageLayout(ColorImage* img, VkImageLayout newLayout, VkAccessFlags srcAccess, VkAccessFlags dstAccess,
                                             VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
        void resolveImage(ColorImage* src, ColorImage* dst);
        void resolveImage(DepthImage* src, DepthImage* dst);
        void beginRenderPass(RenderPass* renderPass, Framebuffer* fbo, std::array<VkClearValue, 2> clearValues);
        void endRenderPass();
        void bindGraphicsPipeline(VkPipeline pipeline);
        void setViewport(VkViewport viewport);
        void draw(U32 vertexCount, U32 instanceCount, U32 firstVertex, U32 firstInstance);

        VkCommandPool   pool = VK_NULL_HANDLE;
        VkCommandBuffer cmd  = VK_NULL_HANDLE;
        VkFence         fence = VK_NULL_HANDLE;
    };

    //**********************************************************************
    struct VertexInputLayout
    {
        ArrayList<VkVertexInputBindingDescription>   bindingDesc;
        ArrayList<VkVertexInputAttributeDescription> attrDesc;
    };

    //**********************************************************************
    class GraphicsPipeline : public IVulkanResource
    {
    public:
        GraphicsPipeline();
        ~GraphicsPipeline();

        void setShaderModule(VkShaderStageFlagBits shaderStage, VkShaderModule shaderModule, CString entryPoint);
        void setVertexInputState(const VertexInputLayout& inputLayout);
        void setInputAssemblyState(VkPrimitiveTopology topology, VkBool32 restartEnable = VK_FALSE);
        void setRasterizationState(const VkPipelineRasterizationStateCreateInfo& rzState);
        void setRasterizationState(VkBool32 depthClampEnable, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace);
        void setRasterizationState(VkBool32 depthBiasEnable, F32 depthBiasConstantFactor, F32 depthBiasClamp, F32 depthBiasSlopeFactor);
        void setMultisampleState(VkSampleCountFlagBits samples);
        void setMultisampleState(VkBool32 alphaToCoverageEnable, VkBool32 alphaToOneEnable);
        void setMultisampleState(VkBool32 sampleShadingEnable, F32 minSampleShading);
        void setDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& dsState);
        void setDepthStencilState(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp);
        void setDepthStencilState(VkBool32 stencilTestEnable, VkStencilOpState front, VkStencilOpState back);
        void setDepthStencilState(VkBool32 depthBoundsTestEnable, F32 minBounds, F32 maxBounds);
        void setBlendState(U32 index, const VkPipelineColorBlendAttachmentState& blendState);
        void setBlendAttachment(U32 index, VkBlendFactor srcColorBlend, VkBlendFactor dstColorBlend, VkBlendOp colorBlendOp);
        void setBlendAttachment(U32 index, VkBlendFactor srcColorBlend, VkBlendFactor dstColorBlend, VkBlendOp colorBlendOp,
                                VkBlendFactor srcAlphaBlend, VkBlendFactor dstAlphaBlend, VkBlendOp alphaBlendOp);
        void addDynamicState(VkDynamicState dynamicState);
        void setPipelineLayout(VkPipelineLayout pipelineLayout);
        void buildPipeline(VkRenderPass renderPass, U32 subPass = 0);

        VkPipeline pipeline = VK_NULL_HANDLE;

    private:
        HashMap<VkShaderStageFlagBits, VkPipelineShaderStageCreateInfo> m_shaderStages;

        VertexInputLayout                               m_vertexInputLayout;

        VkPipelineInputAssemblyStateCreateInfo          m_inputAssemblyState{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };

        VkPipelineViewportStateCreateInfo               m_viewportState{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, 0, 0, 1, 0, 1 };

        VkPipelineRasterizationStateCreateInfo          m_rasterizationState{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };

        VkPipelineMultisampleStateCreateInfo            m_multisampleState{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };

        VkPipelineDepthStencilStateCreateInfo           m_depthStencilState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

        ArrayList<VkPipelineColorBlendAttachmentState>  m_colorBlendAttachmentStates;
        VkPipelineColorBlendStateCreateInfo             m_colorBlendState{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };

        ArrayList<VkDynamicState>                       m_dynamicStates;

        VkPipelineLayout                                m_pipelineLayout;
    };

} } // End namespaces