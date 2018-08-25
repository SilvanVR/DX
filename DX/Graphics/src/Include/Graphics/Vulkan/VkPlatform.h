#pragma once
/**********************************************************************
    class: Platform

    author: S. Hau
    date: August 14, 2018

    Global state maintaining vulkan instance, device, per frame data etc.
**********************************************************************/

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include "OS/Window/window.h"
#include "Logging/logging.h"
#include "Ext/vk_mem_alloc.h"
#include "Wrapper/VkWrapper.h"

#define ALLOCATOR nullptr

#ifdef _DEBUG
    String ResultToString(VkResult result);

    #define VALIDATE(x) \
        if ( x != VK_SUCCESS ) { \
            LOG_ERROR_RENDERING( String( "VkRenderer: @" ) + __FILE__ + ", line " + TS(__LINE__) + ". "\
                                 "Function: " + #x + ". Reason: " + ResultToString(x) );\
        }
    #define VALIDATION_LAYERS
#else
    #define VALIDATE(x) (x)
#endif

namespace Graphics { class VkRenderer; }

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Context
    {
        enum class ClearMode { DontCare = 0, Clear, Load };

    public:
        void SetClearColor(Color color);
        void SetClearDepthStencil(F32 depth, U32 stencil);
        void SetPipelineLayout(VkPipelineLayout pipelineLayout);
        void IASetInputLayout(const VertexInputLayout& inputLayout);
        void IASetPrimitiveTopology(VkPrimitiveTopology topology);
        void SetVertexShader(VkShaderModule module, CString entryPoint);
        void SetFragmentShader(VkShaderModule module, CString entryPoint);
        void SetGeometryShader(VkShaderModule module, CString entryPoint);
        void OMSetRenderTarget(ImageView* color, ImageView* depth, 
                               VkImageLayout finalColorLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
                               VkImageLayout finalDepthLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        void OMSetBlendState(U32 index, const VkPipelineColorBlendAttachmentState& blendState);
        void OMSetDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& dsState);
        void RSSetViewports(VkViewport viewport);
        void RSSetState(const VkPipelineRasterizationStateCreateInfo& rzState);
        void ResolveImage(ColorImage* src, ColorImage* dst);
        void ResolveImage(DepthImage* src, DepthImage* dst);
        void Draw(U32 vertexCount, U32 instanceCount = 1, U32 firstVertex = 0, U32 firstInstance = 0);

    private:
        friend class Platform;
        void Init();
        void Shutdown();
        void BeginFrame();
        void EndFrame();

        // Dynamically built vulkan objects
        std::unordered_map<U64, RenderPass*>        m_renderPasses;
        std::unordered_map<U64, Framebuffer*>       m_framebuffers;
        std::unordered_map<U64, GraphicsPipeline*>  m_pipelines;
        GraphicsPipeline*                           m_currentPipeline;

        // Context information
        RenderPass*         m_currentRenderPass;
        Framebuffer*        m_currentFramebuffer;
        bool                m_pipelineWasModified = false;
        GraphicsPipeline*   m_currentBoundPipeline;

        struct Attachment
        {
            ImageView*      view;
            ClearMode       clearMode;
            VkClearValue    clearValue;
            VkImageLayout   finalLayout;
        };
        Attachment m_colorAttachment;
        Attachment m_depthAttachment;

        void _ClearContext();
        RenderPass*     _GetRenderPass();
        Framebuffer*    _GetFramebuffer(RenderPass* renderPass);

        U64 _RenderPassHash(Attachment& attachment);
        U64 _FramebufferHash(RenderPass* renderPass, Attachment& attachment);

        VkAttachmentLoadOp _GetLoadOp(ClearMode clearMode);

        void _BindGraphicsPipeline();
    } ;

    //----------------------------------------------------------------------
    struct GPU
    {
        VkPhysicalDevice                    physicalDevice;
        ArrayList<VkQueueFamilyProperties>  queueFamilyProperties;
        VkPhysicalDeviceProperties          properties;
        VkPhysicalDeviceMemoryProperties    memoryProperties;
        VkPhysicalDeviceFeatures            supportedFeatures;
    };

    //**********************************************************************
    class Platform
    {
        struct FrameData
        {
            VkSemaphore semPresentComplete;
            VkSemaphore semRenderingFinished;
            CmdBuffer*  cmd;
        };
    public:
        Platform() = default;
        ~Platform() = default;

        VkInstance      instance                    = VK_NULL_HANDLE;
        VkDevice        device                      = VK_NULL_HANDLE;
        GPU             gpu;
        I32             queueFamilyGraphicsIndex    = -1;
        I32             queueFamilyTransferIndex    = -1;
        VkQueue         graphicsQueue               = VK_NULL_HANDLE;
        VkQueue         transferQueue               = VK_NULL_HANDLE;
        VmaAllocator    allocator                   = VK_NULL_HANDLE;
        Context         ctx;

        //----------------------------------------------------------------------
        CmdBuffer& curDrawCmd() { return *m_frameData[m_frameDataIndex].cmd; }
        FrameData& curFrameData() { return m_frameData[m_frameDataIndex]; }

        //----------------------------------------------------------------------
        ColorImage*         createColorImage(U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VmaMemoryUsage memUsage);
        ColorImage*         createColorImage(VkImage image, U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples);
        DepthImage*         createDepthImage(U32 width, U32 height, VkFormat format, VkSampleCountFlagBits samples);
        ImageView*          createImageView(ColorImage* color);
        ImageView*          createImageView(DepthImage* depth);
        RenderPass*         createRenderPass(const RenderPass::AttachmentDescription& color, const RenderPass::AttachmentDescription& depth);
        Framebuffer*        createFramebuffer(RenderPass* renderPass, ImageView* colorView, ImageView* depthView);
        GraphicsPipeline*   createGraphicsPipeline();

    private:
        static const I32 NUM_FRAME_DATA = 2;
        std::array<FrameData, NUM_FRAME_DATA> m_frameData;
        U32 m_frameDataIndex = 0;

        //----------------------------------------------------------------------
        friend class Graphics::VkRenderer;
        void CreateInstance(const ArrayList<String>& extensions);
        void SelectPhysicalDevice(VkPhysicalDevice gpu = nullptr);
        void CreateDevice(VkSurfaceKHR surface, const ArrayList<String>& extensions, const VkPhysicalDeviceFeatures& features);
        void Init();
        void Shutdown();

        void BeginFrame();
        void EndFrame(VkSemaphore waitSemaphore, VkSemaphore signalSemaphore);

        //----------------------------------------------------------------------
        ArrayList<CString> _GetRequiredInstanceLayers();
        ArrayList<CString> _GetRequiredInstanceExtensions();
        void _CheckInstanceExtensions(ArrayList<CString>& extensions);
        void _CheckDeviceExtensions(ArrayList<CString>& extensions);
        void _CreateGPUAllocator();

#ifdef VALIDATION_LAYERS
        VkDebugUtilsMessengerEXT m_debugMessenger;
        void _EnableDebugCallback(VkInstance instance);
        void _DestroyDebugCallback(VkInstance instance);
#endif
        NULL_COPY_AND_ASSIGN(Platform)
    };


} } // End namespaces