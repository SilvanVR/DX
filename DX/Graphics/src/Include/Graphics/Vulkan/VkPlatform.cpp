#include "VkPlatform.h"
/**********************************************************************
    class: Platform

    author: S. Hau
    date: August 14, 2018
**********************************************************************/

#include "Common/estl.hpp"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    VkBool32 DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData )
    {
        String msg;
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)      msg += "[General] ";
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)   msg += "[Validation] ";
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)  msg += "[Performance] ";

        msg += pCallbackData->pMessage;

        // There is a bug in ovr_CreateTextureSwapChainVk which throws this error.
        if (msg.find("[Validation] vkCreateImage: The combination of format") != String::npos)
            return VK_FALSE;

        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)  LOG( msg );
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)     LOG( msg );
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)  LOG_WARN( msg );
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)    LOG_ERROR( msg );

        return VK_FALSE;
    }

    //----------------------------------------------------------------------
    void Platform::Shutdown()
    {
        vkDestroyDevice( device, ALLOCATOR );
#ifdef _DEBUG 
        _DestroyDebugCallback( instance );
#endif
        vkDestroyInstance( instance, ALLOCATOR );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Platform::CreateInstance( const ArrayList<String>& extensions )
    {
        VkInstanceCreateInfo instanceInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

        auto instanceLayers = _GetRequiredInstanceLayers();
        instanceInfo.enabledLayerCount       = (U32)instanceLayers.size();
        instanceInfo.ppEnabledLayerNames     = instanceLayers.data();

        auto instanceExtensions = _GetRequiredInstanceExtensions();
        for (auto& ext : extensions)
            instanceExtensions.push_back( ext.data() );
        _CheckInstanceExtensions( instanceExtensions );
        instanceInfo.enabledExtensionCount   = (U32)instanceExtensions.size();
        instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

        VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.pApplicationName = "DX_VkRenderer";
        appInfo.pEngineName = "DX_Engine";

        U32 apiVersion = 0;
        VALIDATE( vkEnumerateInstanceVersion( &apiVersion ) );
        appInfo.apiVersion = VK_MAKE_VERSION( 1, 1, 0 ) <= apiVersion ? VK_MAKE_VERSION( 1, 1, 0 ) : VK_MAKE_VERSION( 1, 0, 0 );

        instanceInfo.pApplicationInfo = &appInfo;

        VALIDATE( vkCreateInstance( &instanceInfo, ALLOCATOR, &instance ) );

#ifdef _DEBUG
        _EnableDebugCallback( instance );
#endif
    }

    //----------------------------------------------------------------------
    void Platform::SelectPhysicalDevice( VkPhysicalDevice physicalDevice )
    {
        gpu.physicalDevice = physicalDevice;
        if (not physicalDevice)
        {
            U32 physicalDeviceCount;
            vkEnumeratePhysicalDevices( instance, &physicalDeviceCount, NULL );
            ArrayList<VkPhysicalDevice> physicalDevices( physicalDeviceCount );
            vkEnumeratePhysicalDevices( instance, &physicalDeviceCount, physicalDevices.data() );
            if (physicalDeviceCount == 0) LOG_ERROR_RENDERING( "VkRenderer: No GPU found on your system." );

            // Pick first discrete gpu
            for (auto& pd : physicalDevices)
            {
                VkPhysicalDeviceProperties props;
                vkGetPhysicalDeviceProperties( pd, &props );
                if (props.deviceType & VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                    gpu.physicalDevice = pd;
            }

            // No discrete gpu found. just pick first one
            if (not gpu.physicalDevice)
            {
                LOG_WARN_RENDERING( "VkRenderer: No Discrete GPU found. Using another one. Performance might be not optimal." );
                gpu.physicalDevice = physicalDevices[0];
            }
        }

        // Query all capabilities
        U32 queueCount;
        vkGetPhysicalDeviceQueueFamilyProperties( gpu.physicalDevice, &queueCount, NULL );
        gpu.queueFamilyProperties.resize( queueCount );
        vkGetPhysicalDeviceQueueFamilyProperties( gpu.physicalDevice, &queueCount, gpu.queueFamilyProperties.data() );

        vkGetPhysicalDeviceProperties( gpu.physicalDevice, &gpu.properties );
        vkGetPhysicalDeviceMemoryProperties( gpu.physicalDevice, &gpu.memoryProperties );
        vkGetPhysicalDeviceFeatures( gpu.physicalDevice, &gpu.supportedFeatures );
    }

    //----------------------------------------------------------------------
    void Platform::CreateDevice( VkSurfaceKHR surface, const ArrayList<String>& extensions, const VkPhysicalDeviceFeatures& features )
    {
        I32 queueFamilyGraphicsIndex = -1;
        I32 queueFamilyTransferIndex = -1;
        for (I32 i = 0; i < gpu.queueFamilyProperties.size(); ++i)
        {
            VkBool32 supportsPresent;
            vkGetPhysicalDeviceSurfaceSupportKHR( gpu.physicalDevice, i, surface, &supportsPresent );

            if (gpu.queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamilyGraphicsIndex < 0)
                queueFamilyGraphicsIndex = i;
            if (supportsPresent && gpu.queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && queueFamilyTransferIndex < 0)
                queueFamilyTransferIndex = i;
        }
        ASSERT( queueFamilyGraphicsIndex >= 0 && queueFamilyTransferIndex >= 0 && "VkRenderer: Couldn't find required queues." );

        ArrayList<VkDeviceQueueCreateInfo> queueInfos;
        F32 queuePriorities[] = { 1.0f, 0.0f };

        I32 drawQueueIndex, xferQueueIndex;
        VkDeviceQueueCreateInfo graphicQueueInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        if (queueFamilyGraphicsIndex == queueFamilyTransferIndex)
        {
            drawQueueIndex = 0;
            xferQueueIndex = 1;
            graphicQueueInfo.queueFamilyIndex  = queueFamilyGraphicsIndex;
            graphicQueueInfo.queueCount        = 2;
            graphicQueueInfo.pQueuePriorities  = &queuePriorities[0];
            queueInfos.push_back( graphicQueueInfo );
        }
        else
        {
            drawQueueIndex = xferQueueIndex = 0;
            graphicQueueInfo.queueFamilyIndex  = queueFamilyGraphicsIndex;
            graphicQueueInfo.queueCount        = 1;
            graphicQueueInfo.pQueuePriorities  = &queuePriorities[0];
            queueInfos.push_back( graphicQueueInfo );
        
            graphicQueueInfo.queueFamilyIndex  = queueFamilyTransferIndex;
            graphicQueueInfo.queueCount        = 1;
            graphicQueueInfo.pQueuePriorities  = &queuePriorities[1];
            queueInfos.push_back( graphicQueueInfo );
        }

        ArrayList<CString> deviceExtensions;
        for (auto& ext : extensions)
            deviceExtensions.push_back( ext.data() );

        VkDeviceCreateInfo deviceInfo = {};
        deviceInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.pNext                    = nullptr;
        deviceInfo.queueCreateInfoCount     = (U32)queueInfos.size();
        deviceInfo.pQueueCreateInfos        = queueInfos.data();
        deviceInfo.enabledExtensionCount    = (U32)deviceExtensions.size();
        deviceInfo.ppEnabledExtensionNames  = deviceExtensions.data();
        deviceInfo.pEnabledFeatures         = &features;

        vkCreateDevice( gpu.physicalDevice, &deviceInfo, nullptr, &device );

        vkGetDeviceQueue( device, queueFamilyGraphicsIndex, drawQueueIndex, &graphicsQueue );
        vkGetDeviceQueue( device, queueFamilyTransferIndex, xferQueueIndex, &transferQueue );
    }
    
    //**********************************************************************
    // PRIVATE
    //**********************************************************************

#ifdef _DEBUG 
    //----------------------------------------------------------------------
    void Platform::_EnableDebugCallback( VkInstance instance )
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr;

        PFN_vkCreateDebugUtilsMessengerEXT fnc;
        fnc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );
        VALIDATE( fnc( instance, &createInfo, ALLOCATOR, &m_debugMessenger ) );
    }

    //----------------------------------------------------------------------
    void Platform::_DestroyDebugCallback( VkInstance instance )
    {
        PFN_vkDestroyDebugUtilsMessengerEXT fnc;
        fnc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );
        fnc( instance, m_debugMessenger, ALLOCATOR );
    }
#endif

    //----------------------------------------------------------------------
    ArrayList<CString> Platform::_GetRequiredInstanceLayers()
    {
        U32 instanceLayerCount;
        vkEnumerateInstanceLayerProperties( &instanceLayerCount, NULL );

        ArrayList<VkLayerProperties> instanceLayerProperties( instanceLayerCount );
        VALIDATE( vkEnumerateInstanceLayerProperties( &instanceLayerCount, instanceLayerProperties.data() ) );

        ArrayList<CString> instanceLayers;
#ifdef _DEBUG
        CString validationLayers = "VK_LAYER_LUNARG_standard_validation";
        bool found = estl::find_if( instanceLayerProperties, [validationLayers](VkLayerProperties prop) { return strcmp(prop.layerName, validationLayers) > 0; } );
        found ? instanceLayers.push_back( validationLayers ) : LOG_WARN_RENDERING( "VkRenderer: Debug extension not supported on your system. Debugging might not work properly." );
#endif
        return instanceLayers;
    }

    //----------------------------------------------------------------------
    ArrayList<CString> Platform::_GetRequiredInstanceExtensions()
    {
#ifdef _DEBUG
        return { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
#else
        return {};
#endif
    }

    //----------------------------------------------------------------------
    void Platform::_CheckInstanceExtensions( ArrayList<CString>& extensions )
    {
        U32 extensionCount;
        vkEnumerateInstanceExtensionProperties( NULL, &extensionCount, NULL );
        ArrayList<VkExtensionProperties> extensionProperties( extensionCount );
        VALIDATE( vkEnumerateInstanceExtensionProperties( NULL, &extensionCount, extensionProperties.data() ) );

        for (auto it = extensions.begin(); it != extensions.end();)
        {
            auto& extension = *it;
            bool found = estl::find_if( extensionProperties, [extension](VkExtensionProperties prop) { return strcmp(prop.extensionName, extension) == 0; } );
            if (not found)
            {
                LOG_WARN_RENDERING( String("VkRenderer: Could not find extension '") + extension + "' on your system. Extension will be disabled." );
                it = extensions.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }

    //----------------------------------------------------------------------
    void Platform::_CheckDeviceExtensions( ArrayList<CString>& extensions )
    {
        U32 extensionCount;
        vkEnumerateDeviceExtensionProperties( gpu.physicalDevice, NULL, &extensionCount, NULL );
        ArrayList<VkExtensionProperties> extensionProperties( extensionCount );
        VALIDATE( vkEnumerateInstanceExtensionProperties( NULL, &extensionCount, extensionProperties.data() ) );

        for (auto it = extensions.begin(); it != extensions.end();)
        {
            auto& extension = *it;
            bool found = estl::find_if( extensionProperties, [extension](VkExtensionProperties prop) { return strcmp(prop.extensionName, extension) == 0; } );
            if (not found)
            {
                LOG_WARN_RENDERING( String("VkRenderer: Could not find extension '") + extension + "' on your system. Extension will be disabled." );
                it = extensions.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }

} } // End namespaces