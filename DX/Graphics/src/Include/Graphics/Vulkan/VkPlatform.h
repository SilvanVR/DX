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

        //----------------------------------------------------------------------
        void create(U32 width, U32 height, U32 attachmentCount, const VkImageView* pImageViews);
        void destroy();

        //----------------------------------------------------------------------
        // Set's the clear color for all attachments.
        //----------------------------------------------------------------------
        void setClearColor(Color color);

        //----------------------------------------------------------------------
        // Set's the clear depth/stencil for all attachments.
        //----------------------------------------------------------------------
        void setClearDepthStencil(F32 depth, U32 stencil);

        //----------------------------------------------------------------------
        const ArrayList<VezAttachmentReference>& getAttachmentReferences() const { return m_attachmentRefs; }

    private:
        ArrayList<VezAttachmentReference> m_attachmentRefs;
    };

    //**********************************************************************
    class Context
    {
        static const I32 NUM_FRAME_DATA = 2;
        struct FrameData
        {
            VkFence             fence = VK_NULL_HANDLE; // Signaled when cmd has finished execution
            VkSemaphore         semRenderingFinished = VK_NULL_HANDLE;
            VezCommandBuffer    cmd = VK_NULL_HANDLE;
        };
        std::array<FrameData, NUM_FRAME_DATA> m_frameData;
        U32 m_frameDataIndex = 0;

    public:
        //----------------------------------------------------------------------
        VezCommandBuffer&   curDrawCmd() { return m_frameData[m_frameDataIndex].cmd; }
        FrameData&          curFrameData() { return m_frameData[m_frameDataIndex]; }

        //----------------------------------------------------------------------
        void IASetInputLayout(const VezVertexInputFormat& inputLayout);
        void IASetPrimitiveTopology(VkPrimitiveTopology topology);
        void IASetVertexBuffers(U32 firstBinding, U32 bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* offsets);
        void IASetIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);
        void BindPipeline(VezPipeline pipeline);
        void OMSetRenderTarget(const Framebuffer& fbo);
        void OMSetBlendState(U32 index, const VezColorBlendAttachmentState& blendState);
        void OMSetDepthStencilState(const VezDepthStencilState& dsState);
        void RSSetViewports(VkViewport viewport);
        void RSSetScissor(VkRect2D scissor);
        void RSSetState(const VezRasterizationState& rzState);
        void ResolveImage(VkImage src, VkImage dst, VkExtent2D extent);
        void Draw(U32 vertexCount, U32 instanceCount = 1, U32 firstVertex = 0, U32 firstInstance = 0);
        void DrawIndexed(U32 indexCount, U32 instanceCount, U32 firstVertex, U32 vertexOffset, U32 firstInstance);
        void PushConstants(U32 offset, U32 size, const void* pValues);
        void EndRenderPass();

    private:
        friend class VkRenderer;
        void Init();
        void Shutdown();
        void BeginFrame();
        void EndFrame();

        void _ClearContext();
        bool m_insideRenderPass = false;
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

        VkInstance      instance                    = VK_NULL_HANDLE;
        VkDevice        device                      = VK_NULL_HANDLE;
        GPU             gpu;
        VkQueue         graphicsQueue               = VK_NULL_HANDLE;
        VkQueue         transferQueue               = VK_NULL_HANDLE;
        Context         ctx;

    private:
        //----------------------------------------------------------------------
        friend class Graphics::VkRenderer;
        void CreateInstance(const ArrayList<String>& extensions);
        void SelectPhysicalDevice(VkPhysicalDevice gpu = nullptr);
        void CreateDevice(VkSurfaceKHR surface, const ArrayList<String>& extensions, const VkPhysicalDeviceFeatures& features);
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