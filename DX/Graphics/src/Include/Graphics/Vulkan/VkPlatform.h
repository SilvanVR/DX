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
    class Context
    {
    public:
        void SetClearColor(Color color);
        void SetClearDepthStencil(F32 depth, U32 stencil);
        void SetPipelineLayout(VkPipelineLayout pipelineLayout);
        //void IASetInputLayout(const VertexInputLayout& inputLayout);
        void IASetPrimitiveTopology(VkPrimitiveTopology topology);
        void SetVertexShader(VkShaderModule module);
        void SetFragmentShader(VkShaderModule module);
        void SetGeometryShader(VkShaderModule module);
        //void OMSetRenderTarget(ImageView* color, ImageView* depth, 
        //                       VkImageLayout finalColorLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
        //                       VkImageLayout finalDepthLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        void OMSetBlendState(U32 index, const VkPipelineColorBlendAttachmentState& blendState);
        void OMSetDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& dsState);
        void RSSetViewports(VkViewport viewport);
        void RSSetState(const VkPipelineRasterizationStateCreateInfo& rzState);
        //void ResolveImage(ColorImage* src, ColorImage* dst);
        //void ResolveImage(DepthImage* src, DepthImage* dst);
        //void Draw(U32 vertexCount, U32 instanceCount = 1, U32 firstVertex = 0, U32 firstInstance = 0);

    private:
        friend class Platform;
        void Init();
        void Shutdown();
        void BeginFrame();
        void EndFrame();
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
            VkFence             fence = VK_NULL_HANDLE;
            VkSemaphore         semRenderingFinished = VK_NULL_HANDLE;
            VezCommandBuffer    cmd = VK_NULL_HANDLE;
        };

    public:
        Platform() = default;
        ~Platform() = default;

        VkInstance      instance                    = VK_NULL_HANDLE;
        VkDevice        device                      = VK_NULL_HANDLE;
        GPU             gpu;
        VkQueue         graphicsQueue               = VK_NULL_HANDLE;
        VkQueue         transferQueue               = VK_NULL_HANDLE;
        Context         ctx;

        //----------------------------------------------------------------------
        VezCommandBuffer&   curDrawCmd()    { return m_frameData[m_frameDataIndex].cmd; }
        FrameData&          curFrameData()  { return m_frameData[m_frameDataIndex]; }

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
        void EndFrame();

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