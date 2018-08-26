#include "VkPlatform.h"
/**********************************************************************
    class: Platform

    author: S. Hau
    date: August 14, 2018
**********************************************************************/

#include "Vulkan.hpp"
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
    void Platform::Init()
    {
        ASSERT( device && "Init function should be called after creating a device!" );
        for (U32 i = 0; i < NUM_FRAME_DATA; i++)
        {
            VezCommandBufferAllocateInfo allocateInfo{ NULL, graphicsQueue, 1 };
            vezAllocateCommandBuffers( device, &allocateInfo, &m_frameData[i].cmd );
        }
        ctx.Init();
    }

    //----------------------------------------------------------------------
    void Platform::Shutdown()
    {
        vkDeviceWaitIdle( device );
        ctx.Shutdown();
        for (U32 i = 0; i < NUM_FRAME_DATA; i++)
            vezFreeCommandBuffers( device, 1, &m_frameData[i].cmd );
        vkDestroyDevice( device, ALLOCATOR );
#ifdef VALIDATION_LAYERS 
        _DestroyDebugCallback( instance );
#endif
        vezDestroyInstance( instance );
    }

    //----------------------------------------------------------------------
    void Platform::BeginFrame()
    {
        m_frameDataIndex = (m_frameDataIndex + 1) % NUM_FRAME_DATA;

        // Wait on fence, so cmd can be safely reused
        auto& curFD = curFrameData();
        if (curFD.fence)
        {
            vezWaitForFences(device, 1, &curFD.fence, VK_TRUE, ~0);
            vezDestroyFence(device, curFD.fence);
            curFD.fence = VK_NULL_HANDLE;
        }

        // Begin recording
        vezBeginCommandBuffer( curFrameData().cmd, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );

        ctx.BeginFrame();
    }

    //----------------------------------------------------------------------
    void Platform::EndFrame()
    {
        ctx.EndFrame();

        vezEndCommandBuffer( curFrameData().cmd );

        VezSubmitInfo submitInfo = {};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &curFrameData().semRenderingFinished;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &curDrawCmd();
        vezQueueSubmit( graphicsQueue, 1, &submitInfo, &curFrameData().fence );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //**********************************************************************
    // PUBLIC - FRIEND
    //**********************************************************************

    //----------------------------------------------------------------------
    void Platform::CreateInstance( const ArrayList<String>& extensions )
    {
        VezApplicationInfo appInfo{};
        appInfo.pApplicationName = "DX_VkRenderer";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "MyEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        VezInstanceCreateInfo instanceInfo{};
        instanceInfo.pApplicationInfo = &appInfo;

        auto instanceLayers = _GetRequiredInstanceLayers();
        instanceInfo.enabledLayerCount       = (U32)instanceLayers.size();
        instanceInfo.ppEnabledLayerNames     = instanceLayers.data();

        auto instanceExtensions = _GetRequiredInstanceExtensions();
        for (auto& ext : extensions)
            instanceExtensions.push_back( ext.data() );
        _CheckInstanceExtensions( instanceExtensions );
        instanceInfo.enabledExtensionCount   = (U32)instanceExtensions.size();
        instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

        VALIDATE( vezCreateInstance( &instanceInfo, &instance ) );

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
            vezEnumeratePhysicalDevices( instance, &physicalDeviceCount, NULL );
            ArrayList<VkPhysicalDevice> physicalDevices( physicalDeviceCount );
            vezEnumeratePhysicalDevices( instance, &physicalDeviceCount, physicalDevices.data() );
            if (physicalDeviceCount == 0) LOG_ERROR_RENDERING( "VkRenderer: No GPU found on your system." );

            // Pick first discrete gpu
            for (auto& pd : physicalDevices)
            {
                VkPhysicalDeviceProperties props;
                vezGetPhysicalDeviceProperties( pd, &props );
                if (props.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                    gpu.physicalDevice = pd;
            }

            // No discrete gpu found. just pick first one
            if (not gpu.physicalDevice)
            {
                LOG_WARN_RENDERING( "VkRenderer: No Discrete GPU found. Using another one. Performance might be really bad." );
                gpu.physicalDevice = physicalDevices[0];
            }
        }

        // Query all capabilities
        U32 queueCount;
        vezGetPhysicalDeviceQueueFamilyProperties( gpu.physicalDevice, &queueCount, NULL );
        gpu.queueFamilyProperties.resize( queueCount );
        vezGetPhysicalDeviceQueueFamilyProperties( gpu.physicalDevice, &queueCount, gpu.queueFamilyProperties.data() );

        vezGetPhysicalDeviceProperties( gpu.physicalDevice, &gpu.properties );
        vkGetPhysicalDeviceMemoryProperties( gpu.physicalDevice, &gpu.memoryProperties );
        vezGetPhysicalDeviceFeatures( gpu.physicalDevice, &gpu.supportedFeatures );
    }

    //----------------------------------------------------------------------
    void Platform::CreateDevice( VkSurfaceKHR surface, const ArrayList<String>& extensions, const VkPhysicalDeviceFeatures& features )
    {
        ArrayList<CString> deviceExtensions;
        for (auto& ext : extensions)
            deviceExtensions.push_back( ext.data() );

        VezDeviceCreateInfo deviceInfo = {};
        deviceInfo.enabledExtensionCount    = (U32)deviceExtensions.size();
        deviceInfo.ppEnabledExtensionNames  = deviceExtensions.data();

        vezCreateDevice( gpu.physicalDevice, &deviceInfo, &device );

        vezGetDeviceGraphicsQueue( device, 0, &graphicsQueue );
        vezGetDeviceTransferQueue( device, 0, &transferQueue );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

#ifdef VALIDATION_LAYERS 
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
        vezEnumerateInstanceLayerProperties( &instanceLayerCount, NULL );

        ArrayList<VkLayerProperties> instanceLayerProperties( instanceLayerCount );
        VALIDATE( vezEnumerateInstanceLayerProperties( &instanceLayerCount, instanceLayerProperties.data() ) );

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
        vezEnumerateInstanceExtensionProperties( NULL, &extensionCount, NULL );
        ArrayList<VkExtensionProperties> extensionProperties( extensionCount );
        VALIDATE( vezEnumerateInstanceExtensionProperties( NULL, &extensionCount, extensionProperties.data() ) );

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
        vezEnumerateDeviceExtensionProperties( gpu.physicalDevice, NULL, &extensionCount, NULL );
        ArrayList<VkExtensionProperties> extensionProperties( extensionCount );
        VALIDATE( vezEnumerateDeviceExtensionProperties( gpu.physicalDevice, NULL, &extensionCount, extensionProperties.data() ) );

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

    //**********************************************************************
    // CONTEXT
    //**********************************************************************

    //----------------------------------------------------------------------
    void Context::Init()
    {
    }

    //----------------------------------------------------------------------
    void Context::Shutdown()
    {
    }

    //----------------------------------------------------------------------
    void Context::BeginFrame()
    {

    }

    //----------------------------------------------------------------------
    void Context::EndFrame()
    {

    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Context::SetClearColor( Color color )
    {

    }

    //----------------------------------------------------------------------
    void Context::SetClearDepthStencil( F32 depth, U32 stencil ) 
    {

    }

    //----------------------------------------------------------------------
    void Context::SetPipelineLayout( VkPipelineLayout pipelineLayout )
    {

    }

    //----------------------------------------------------------------------
    //void Context::IASetInputLayout( const VertexInputLayout& inputLayout )
    //{

    //}

    //----------------------------------------------------------------------
    void Context::IASetPrimitiveTopology( VkPrimitiveTopology topology )
    {

    }

    //----------------------------------------------------------------------
    void Context::SetVertexShader( VkShaderModule module )
    {

    }

    //----------------------------------------------------------------------
    void Context::SetFragmentShader( VkShaderModule module )
    {

    }

    //----------------------------------------------------------------------
    void Context::SetGeometryShader( VkShaderModule module )
    {

    }

    //----------------------------------------------------------------------
    //void Context::OMSetRenderTarget( ImageView* color, ImageView* depth, VkImageLayout finalColorLayout, VkImageLayout finalDepthLayout )
    //{

    //}

    //----------------------------------------------------------------------
    void Context::OMSetBlendState( U32 index, const VkPipelineColorBlendAttachmentState& blendState )
    {

    }

    //----------------------------------------------------------------------
    void Context::OMSetDepthStencilState( const VkPipelineDepthStencilStateCreateInfo& dsState )
    {

    }

    //----------------------------------------------------------------------
    void Context::RSSetViewports( VkViewport viewport )
    {
 
    }

    //----------------------------------------------------------------------
    void Context::RSSetState( const VkPipelineRasterizationStateCreateInfo& rzState )
    {

    }

    //----------------------------------------------------------------------
    //void Context::ResolveImage( ColorImage* src, ColorImage* dst )
    //{
    //    g_vulkan.curDrawCmd().resolveImage( src, dst );
    //}

    ////----------------------------------------------------------------------
    //void Context::ResolveImage( DepthImage* src, DepthImage* dst )
    //{
    //    g_vulkan.curDrawCmd().resolveImage( src, dst );
    //}

    ////----------------------------------------------------------------------
    //void Context::Draw( U32 vertexCount, U32 instanceCount, U32 firstVertex, U32 firstInstance )
    //{
    //    g_vulkan.curDrawCmd().draw( vertexCount, instanceCount, firstVertex, firstInstance );
    //}

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

} } // End namespaces