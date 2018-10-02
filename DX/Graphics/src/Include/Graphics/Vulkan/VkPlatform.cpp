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

        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)  LOG( msg );
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)     LOG( msg );
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)  LOG_WARN( msg );
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)    LOG( "[ERROR] " + msg, Color::RED );

        return VK_FALSE;
    }

    //----------------------------------------------------------------------
    void Platform::Shutdown()
    {
        vkDeviceWaitIdle( device );
        vezDestroyDevice( device );
#ifdef VALIDATION_LAYERS 
        _DestroyDebugCallback( instance );
#endif
        vezDestroyInstance( instance );
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
    void Platform::CreateDevice( const ArrayList<String>& extensions, const VkPhysicalDeviceFeatures& features )
    {
        ArrayList<CString> deviceExtensions;
        for (auto& ext : extensions)
            deviceExtensions.push_back( ext.data() );

        VezDeviceCreateInfo deviceInfo = {};
        deviceInfo.enabledExtensionCount    = (U32)deviceExtensions.size();
        deviceInfo.ppEnabledExtensionNames  = deviceExtensions.data();

        vezCreateDevice( gpu.physicalDevice, &deviceInfo, &device );

        vezGetDeviceGraphicsQueue( device, 0, &graphicsQueue );
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

        static ArrayList<CString> instanceLayers;
        instanceLayers.clear();
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
    // FRAMEBUFFER
    //**********************************************************************

    //----------------------------------------------------------------------
    void Framebuffer::create( U32 width, U32 height, U32 attachmentCount, const VkImageView* pImageViews, VkSampleCountFlagBits samples )
    {
        this->samples = samples;

        m_attachmentRefs.resize( attachmentCount );
        for (I32 i = 0; i < m_attachmentRefs.size(); i++)
        {
            m_attachmentRefs[i].clearValue = {};
            m_attachmentRefs[i].loadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;
            m_attachmentRefs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        }

        VezFramebufferCreateInfo fboCreateInfo{};
        fboCreateInfo.width             = width;
        fboCreateInfo.height            = height;
        fboCreateInfo.attachmentCount   = attachmentCount;
        fboCreateInfo.pAttachments      = pImageViews;
        fboCreateInfo.layers            = 1;
        vezCreateFramebuffer( g_vulkan.device, &fboCreateInfo, &framebuffer );
    }

    //----------------------------------------------------------------------
    void Framebuffer::destroy()
    { 
        vezDeviceWaitIdle( g_vulkan.device );
        vezDestroyFramebuffer( g_vulkan.device, framebuffer );
        framebuffer = VK_NULL_HANDLE;
        m_attachmentRefs.clear();
    }

    //----------------------------------------------------------------------
    void Framebuffer::setClearColor( U32 attachmentIndex, Color color )
    {
        auto& ref = m_attachmentRefs[attachmentIndex];
        ref.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        auto colorNorm = color.normalized();
        ref.clearValue.color = { colorNorm[0], colorNorm[1], colorNorm[2], colorNorm[3] };
    }

    //----------------------------------------------------------------------
    void Framebuffer::setClearDepthStencil( U32 attachmentIndex, F32 depth, U32 stencil )
    {
        auto& ref = m_attachmentRefs[attachmentIndex];
        ref.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        ref.clearValue.depthStencil = { depth, stencil };
    }

    //**********************************************************************
    // CONTEXT
    //**********************************************************************

    //----------------------------------------------------------------------
    void Context::Init()
    {
        ASSERT( g_vulkan.device && "Init function should be called after creating a device!" );
        for (U32 i = 0; i < NUM_FRAME_DATA; i++)
        {
            VezCommandBufferAllocateInfo allocateInfo{ NULL, g_vulkan.graphicsQueue, 1 };
            vezAllocateCommandBuffers( g_vulkan.device, &allocateInfo, &m_frameData[i].cmd );
        }
    }

    //----------------------------------------------------------------------
    void Context::Shutdown()
    {
        for (U32 i = 0; i < NUM_FRAME_DATA; i++)
        {
            vezFreeCommandBuffers( g_vulkan.device, 1, &m_frameData[i].cmd );
            m_frameData[i].fence = VK_NULL_HANDLE;
            m_frameData[i].semRenderingFinished = VK_NULL_HANDLE;
        }
    }

    //----------------------------------------------------------------------
    void Context::BeginFrame()
    {
        _ClearContext();

        // Wait on fence, so cmd can be safely reused
        auto& curFD = curFrameData();
        if (curFD.fence)
        {
            VALIDATE( vezWaitForFences( g_vulkan.device, 1, &curFD.fence, VK_TRUE, ~0 ) );
            vezDestroyFence( g_vulkan.device, curFD.fence );
            curFD.fence = VK_NULL_HANDLE;
        }

        // Begin recording
        VALIDATE( vezBeginCommandBuffer( curFrameData().cmd, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT ) );

        // Execute pre-pass functions
        std::unique_lock<std::mutex> lock( m_prePassFunctionLock );
        if (not m_prePassFunctions.empty())
        {
            for (auto& func : m_prePassFunctions)
                func.second();
            m_prePassFunctions.clear();
        }
    }

    //----------------------------------------------------------------------
    void Context::EndFrame()
    {
        if (m_curFramebuffer)
            EndRenderPass();

        VALIDATE( vezEndCommandBuffer() );

        VezSubmitInfo submitInfo = {};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &curFrameData().semRenderingFinished;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &curDrawCmd();
        VALIDATE( vezQueueSubmit( g_vulkan.graphicsQueue, 1, &submitInfo, &curFrameData().fence ) );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Context::IASetInputLayout( const VezVertexInputFormat& inputLayout )
    {
        vezCmdSetVertexInputFormat( inputLayout );
    }

    //----------------------------------------------------------------------
    void Context::IASetPrimitiveTopology( VkPrimitiveTopology topology )
    {
        VezInputAssemblyState stateInfo{};
        stateInfo.topology = topology;
        vezCmdSetInputAssemblyState( &stateInfo );
    }

    //----------------------------------------------------------------------
    void Context::IASetVertexBuffers( U32 firstBinding, U32 bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets )
    {
        vezCmdBindVertexBuffers( firstBinding, bindingCount, pBuffers, pOffsets );
    }

    //----------------------------------------------------------------------
    void Context::IASetIndexBuffer( VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType )
    {
        vezCmdBindIndexBuffer( buffer, offset, indexType );
    }

    //----------------------------------------------------------------------
    void Context::BindPipeline( VezPipeline pipeline )
    {
        vezCmdBindPipeline( pipeline );
    }

    //----------------------------------------------------------------------
    void Context::OMSetRenderTarget( const Framebuffer& fbo, const std::function<void()>& endRenderPassCallback )
    {
        if (m_curFramebuffer) // End previous renderpass if any
            EndRenderPass();

        // Begin a new render pass
        auto& attachmentReferences = fbo.getAttachmentReferences();
        VezRenderPassBeginInfo beginInfo = {};
        beginInfo.framebuffer       = fbo.framebuffer;
        beginInfo.attachmentCount   = static_cast<uint32_t>( attachmentReferences.size() );
        beginInfo.pAttachments      = attachmentReferences.data();
        vezCmdBeginRenderPass( &beginInfo );
        m_curFramebuffer = &fbo;
    }

    //----------------------------------------------------------------------
    void Context::OMSetBlendState( U32 index, const VezColorBlendAttachmentState& blendAttachmentState )
    {
        VezColorBlendState blendState{};
        blendState.attachmentCount = 1;
        blendState.pAttachments = &blendAttachmentState;
        vezCmdSetColorBlendState( &blendState );
    }

    //----------------------------------------------------------------------
    void Context::OMSetDepthStencilState( const VezDepthStencilState& dsState )
    {
        vezCmdSetDepthStencilState( &dsState );
    }

    //----------------------------------------------------------------------
    void Context::OMSetMultiSampleState( VkBool32 alphaToCoverageEnable, VkBool32 alphaToOneEnable )
    {
        ASSERT( m_curFramebuffer != nullptr );

        VezMultisampleState stateInfo{};
        stateInfo.alphaToCoverageEnable = alphaToCoverageEnable;
        stateInfo.alphaToOneEnable      = alphaToOneEnable;
        stateInfo.rasterizationSamples  = m_curFramebuffer->samples;
        stateInfo.sampleShadingEnable   = VK_FALSE;
        stateInfo.minSampleShading      = 1.0f;
        vezCmdSetMultisampleState( &stateInfo );
    }

    //----------------------------------------------------------------------
    void Context::RSSetViewports( VkViewport viewport )
    {
        vezCmdSetViewport( 0, 1, &viewport );
        vezCmdSetViewportState( 1 );
    }

    //----------------------------------------------------------------------
    void Context::RSSetScissor( VkRect2D scissor )
    {
        vezCmdSetScissor( 0, 1, &scissor );
    }

    //----------------------------------------------------------------------
    void Context::RSSetState( const VezRasterizationState& rzState )
    {
        vezCmdSetDepthBias( rzState.depthBiasConstantFactor, rzState.depthBiasClamp, rzState.depthBiasSlopeFactor );
        vezCmdSetRasterizationState( &rzState );
    }

    //----------------------------------------------------------------------
    void Context::ResolveImage( VkImage src, VkImage dst, VkExtent2D extent )
    {
        VezImageResolve region{};
        region.srcSubresource.layerCount = 1;
        region.dstSubresource.layerCount = 1;
        region.extent.width  = extent.width;
        region.extent.height = extent.height;
        region.extent.depth  = 1;
        vezCmdResolveImage( src, dst, 1, &region );
    }

    //----------------------------------------------------------------------
    void Context::Draw( U32 vertexCount, U32 instanceCount, U32 firstVertex, U32 firstInstance )
    {
        vezCmdDraw( vertexCount, instanceCount, firstVertex, firstInstance );
    }

    //----------------------------------------------------------------------
    void Context::DrawIndexed( U32 indexCount, U32 instanceCount, U32 firstVertex, U32 vertexOffset, U32 firstInstance )
    {
        vezCmdDrawIndexed( indexCount, instanceCount, firstVertex, vertexOffset, firstInstance );
    }

    //----------------------------------------------------------------------
    void Context::PushConstants( U32 offset, U32 size, const void* pValues )
    {
        vezCmdPushConstants( offset, size, pValues );
    }

    //----------------------------------------------------------------------
    void Context::EndRenderPass()
    {
        vezCmdEndRenderPass();
        m_curFramebuffer->endRenderPass();
        m_curFramebuffer = nullptr;
    }

    //----------------------------------------------------------------------
    void Context::SetBuffer( VkBuffer buffer, U32 set, U32 binding )
    {
        vezCmdBindBuffer( buffer, 0, VK_WHOLE_SIZE, set, binding, 0 );
    }

    //----------------------------------------------------------------------
    void Context::SetImage( VkImageView imageView, VkSampler sampler, U32 set, U32 binding )
    {
        vezCmdBindImageView( imageView, sampler, set, binding, 0 );
    }

    //----------------------------------------------------------------------
    void Context::GenerateMips( VkImage img, U32 width, U32 height, U32 mipLevels, VkFilter filter )
    {
        GenerateMips( img, width, height, mipLevels, 1, filter );
    }

    //----------------------------------------------------------------------
    void Context::GenerateMips( VkImage img, U32 width, U32 height, U32 mipLevels, U32 layers, VkFilter filter )
    {
        ASSERT( img != VK_NULL_HANDLE );
        for (auto& prePassFnc : m_prePassFunctions)
            if (prePassFnc.first == img) // Already generating mips this frame
                return;

        std::unique_lock<std::mutex> lock( m_prePassFunctionLock );
        m_prePassFunctions.emplace_back( img, [img, width, height, mipLevels, layers, filter] {
            for (U32 layer = 0; layer < layers; ++layer)
            {
                for (U32 mip = 0; mip < (mipLevels - 1); ++mip)
                {
                    VezImageBlit blitInfo{};
                    blitInfo.srcSubresource.mipLevel        = mip;
                    blitInfo.srcSubresource.baseArrayLayer  = layer;
                    blitInfo.srcSubresource.layerCount      = 1;
                    blitInfo.srcOffsets[1].x                = I32(width  >> mip);
                    blitInfo.srcOffsets[1].y                = I32(height >> mip);
                    blitInfo.srcOffsets[1].z                = 1;

                    blitInfo.dstSubresource.mipLevel        = mip + 1;
                    blitInfo.dstSubresource.baseArrayLayer  = layer;
                    blitInfo.dstSubresource.layerCount      = 1;
                    blitInfo.dstOffsets[1].x                = I32(width  >> (mip + 1));
                    blitInfo.dstOffsets[1].y                = I32(height >> (mip + 1));
                    blitInfo.dstOffsets[1].z                = 1;
                    vezCmdBlitImage( img, img, 1, &blitInfo, filter );
                }
            }
        });
    }

    //----------------------------------------------------------------------
    void Context::CopyImage( VkImage src, VkImage dst, U32 regionCount, const VezImageCopy* pRegions )
    {
        vezCmdCopyImage( src, dst, regionCount, pRegions );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Context::_ClearContext()
    {
        m_frameDataIndex = (m_frameDataIndex + 1) % NUM_FRAME_DATA;
        curFrameData().semRenderingFinished = VK_NULL_HANDLE;
    }

} } // End namespaces