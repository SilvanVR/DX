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

#define ALLOCATOR nullptr

#ifdef _DEBUG
    String ResultToString(VkResult result);

    #define VALIDATE(x) \
        if ( x != VK_SUCCESS ) { \
            LOG_ERROR_RENDERING( String( "VkRenderer: @" ) + __FILE__ + ", line " + TS(__LINE__) + ". "\
                                 "Function: " + #x + ". Reason: " + ResultToString(x) );\
        }
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

    public:
        Platform() = default;
        ~Platform() = default;

        VkInstance  instance;
        VkDevice    device;
        VkQueue     graphicsQueue;
        VkQueue     transferQueue;
        GPU         gpu;

    private:

#ifdef _DEBUG
        VkDebugUtilsMessengerEXT m_debugMessenger;
        void _EnableDebugCallback(VkInstance instance);
        void _DestroyDebugCallback(VkInstance instance);
#endif

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

        NULL_COPY_AND_ASSIGN(Platform)
    };


} } // End namespaces