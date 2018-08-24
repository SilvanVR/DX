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
            CmdBuffer   cmd;
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

        //----------------------------------------------------------------------
        CmdBuffer& curDrawCmd() { return m_frameData[m_frameDataIndex].cmd; }
        FrameData& curFrameData() { return m_frameData[m_frameDataIndex]; }

    private:
        // Persistent data
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

    public:
        //**********************************************************************
        class Context
        {
            enum class ClearMode
            {
                None,
                Clear
            };

        public:
            void SetClearColor(Color color);
            void SetClearDepthStencil(F32 depth, U32 stencil);
            void OMSetRenderTarget(ImageView* color, ImageView* depth);

        private:
            friend class Platform;
            void Init();
            void Shutdown();
            void BeginFrame();
            void EndFrame();

            // All dynamically built vulkan objects
            std::unordered_map<U64, RenderPass>     m_renderPasses;
            std::unordered_map<U64, Framebuffer>    m_framebuffers;

            // Context information
            ClearMode                   m_colorClearMode        = ClearMode::None;
            ClearMode                   m_depthStencilClearMode = ClearMode::None;
            std::array<VkClearValue, 2> m_clearValues;
            ImageView*                  m_colorView = VK_NULL_HANDLE;
            ImageView*                  m_depthView = VK_NULL_HANDLE;
            RenderPass*                 m_currentRenderPass = VK_NULL_HANDLE;
            Framebuffer*                m_currentFramebuffer = VK_NULL_HANDLE;

            void _CreateNewFrameBuffer();
            void _CreateNewRenderPass();

            VkAttachmentLoadOp _GetLoadOp(ClearMode clearMode);

            void _CmdBeginRenderPass();
        } ctx;
    };


} } // End namespaces