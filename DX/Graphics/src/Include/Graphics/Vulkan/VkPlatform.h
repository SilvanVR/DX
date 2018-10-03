#pragma once
/**********************************************************************
    class: Platform

    author: S. Hau
    date: August 14, 2018

    Global state maintaining vulkan instance, device, per frame data etc.
**********************************************************************/

#define VK_USE_PLATFORM_WIN32_KHR
#include "Ext/VEZ.h"
#include "Logging/logging.h"
#include <functional>
#include <mutex>

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
    // Simple wrapper for a VezFramebuffer and VezAttachmentReferences
    //**********************************************************************
    class Framebuffer
    {
    public:
        VezFramebuffer framebuffer = VK_NULL_HANDLE;
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

        //----------------------------------------------------------------------
        void create(U32 width, U32 height, U32 attachmentCount, const VkImageView* pImageViews, VkSampleCountFlagBits samples);
        void destroy();

        //----------------------------------------------------------------------
        // Set's the clear color for all attachments.
        //----------------------------------------------------------------------
        void setClearColor(U32 attachmentIndex, Color color);

        //----------------------------------------------------------------------
        // Set's the clear depth/stencil for all attachments.
        //----------------------------------------------------------------------
        void setClearDepthStencil(U32 attachmentIndex, F32 depth, U32 stencil);

        //----------------------------------------------------------------------
        // Called after the render-pass has been ended. Used to resolve multisampled images.
        //----------------------------------------------------------------------
        void endRenderPass() const { if (m_endRenderPassCallback) m_endRenderPassCallback(); }

        //----------------------------------------------------------------------
        const ArrayList<VezAttachmentReference>& getAttachmentReferences() const { return m_attachmentRefs; }
        void setEndRenderPassCallback(const std::function<void()>& cb) { m_endRenderPassCallback = cb; }

    private:
        ArrayList<VezAttachmentReference>   m_attachmentRefs;
        std::function<void()>               m_endRenderPassCallback;
    };

    //**********************************************************************
    class Context
    {
        static const I32 NUM_FRAME_DATA = 3;
        struct FrameData
        {
            VkFence         fence = VK_NULL_HANDLE; // Signaled when cmd has finished execution
            VkSemaphore     semRenderingFinished = VK_NULL_HANDLE;
            VkCommandBuffer cmd = VK_NULL_HANDLE;
        };
        std::array<FrameData, NUM_FRAME_DATA> m_frameData;
        U32 m_frameDataIndex = 0;

    public:
        //----------------------------------------------------------------------
        VkCommandBuffer&    curDrawCmd() { return m_frameData[m_frameDataIndex].cmd; }
        FrameData&          curFrameData() { return m_frameData[m_frameDataIndex]; }

        //----------------------------------------------------------------------
        void IASetInputLayout(const VezVertexInputFormat& inputLayout);
        void IASetPrimitiveTopology(VkPrimitiveTopology topology);
        void IASetVertexBuffers(U32 firstBinding, U32 bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* offsets);
        void IASetIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);
        void BindPipeline(VezPipeline pipeline);
        void OMSetRenderTarget(const Framebuffer& fbo, const std::function<void()>& endRenderPassCallback = nullptr);
        void OMSetBlendState(U32 index, const VezColorBlendAttachmentState& blendState);
        void OMSetDepthStencilState(const VezDepthStencilState& dsState);
        void OMSetMultiSampleState(VkBool32 alphaToCoverageEnable, VkBool32 alphaToOneEnable);
        void RSSetViewports(VkViewport viewport);
        void RSSetScissor(VkRect2D scissor);
        void RSSetState(const VezRasterizationState& rzState);
        void ResolveImage(VkImage src, VkImage dst, VkExtent2D extent);
        void Draw(U32 vertexCount, U32 instanceCount = 1, U32 firstVertex = 0, U32 firstInstance = 0);
        void DrawIndexed(U32 indexCount, U32 instanceCount, U32 firstVertex, U32 vertexOffset, U32 firstInstance);
        void PushConstants(U32 offset, U32 size, const void* pValues);
        void EndRenderPass();
        void SetBuffer(VkBuffer buffer, U32 set, U32 binding);
        void SetImage(VkImageView imageView, VkSampler sampler, U32 set, U32 binding);
        void GenerateMips(VkImage img, U32 width, U32 height, U32 mipLevels, VkFilter filter = VK_FILTER_LINEAR);
        void GenerateMips(VkImage img, U32 width, U32 height, U32 mipLevels, U32 layers, VkFilter filter = VK_FILTER_LINEAR);
        void CopyImage(VkImage src, VkImage dst, U32 regionCount,const VezImageCopy* pRegions);

    private:
        friend class VkRenderer;
        void Init();
        void Shutdown();
        void BeginFrame();
        void EndFrame();

        void _ClearContext();

        const Framebuffer* m_curFramebuffer = nullptr;

        // Stores functions for commands which must be executed outside of an render-pass and be thread-safe
        std::mutex m_prePassFunctionLock;
        ArrayList<std::pair<VkImage, std::function<void()>>> m_prePassFunctions;
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
    public:
        Platform() = default;
        ~Platform() = default;

        VkInstance      instance        = VK_NULL_HANDLE;
        VkDevice        device          = VK_NULL_HANDLE;
        GPU             gpu;
        VkQueue         graphicsQueue   = VK_NULL_HANDLE;
        Context         ctx;

    private:
        //----------------------------------------------------------------------
        friend class Graphics::VkRenderer;
        void CreateInstance(const ArrayList<String>& extensions);
        void SelectPhysicalDevice(VkPhysicalDevice gpu = nullptr);
        void CreateDevice(const ArrayList<String>& extensions, const VkPhysicalDeviceFeatures& features);
        void Shutdown();

        //----------------------------------------------------------------------
        ArrayList<CString> _GetRequiredInstanceLayers();
        ArrayList<CString> _GetRequiredInstanceExtensions();
        void _CheckInstanceExtensions(ArrayList<CString>& extensions);
        void _CheckDeviceExtensions(ArrayList<CString>& extensions);

#ifdef VALIDATION_LAYERS
        VkDebugUtilsMessengerEXT m_debugMessenger;
        void _EnableDebugCallback(VkInstance instance);
        void _DestroyDebugCallback(VkInstance instance);
#endif
        NULL_COPY_AND_ASSIGN(Platform)
    };


} } // End namespaces