#include "VkSwapchain.h"
/**********************************************************************
    class: Swapchain

    author: S. Hau
    date: August 16, 2018
**********************************************************************/

#define FORMAT VK_FORMAT_B8G8R8A8_UNORM

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    void Swapchain::init( VkInstance instance, OS::Window* window )
    {
        VkWin32SurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        createInfo.hinstance = window->getInstance();
        createInfo.hwnd = window->getHWND();

        VALIDATE( vkCreateWin32SurfaceKHR( instance, &createInfo, ALLOCATOR, &m_surface ) );
    }

    //----------------------------------------------------------------------
    void Swapchain::shutdown( VkInstance instance, VkDevice device )
    {
        for (auto& swapImg : m_images)
            swapImg.view.release();
        vkDestroySwapchainKHR( device, m_swapchain, ALLOCATOR );
        vkDestroySurfaceKHR( instance, m_surface, ALLOCATOR );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Swapchain::create( VkPhysicalDevice physicalDevice, VkDevice device, bool vsync )
    {
        VkSurfaceCapabilitiesKHR surfCaps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice, m_surface, &surfCaps );
        m_currentExtent = surfCaps.currentExtent;

        auto vkFormat = _ChooseSurfaceFormat( physicalDevice, FORMAT );
        auto presentMode = _ChoosePresentMode( physicalDevice, vsync );

        auto oldSwapchain = m_swapchain;

        VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainInfo.surface           = m_surface;
        swapchainInfo.minImageCount     = _GetDesiredNumberOfSwapchainImages( surfCaps, presentMode );
        swapchainInfo.imageFormat       = vkFormat.format;
        swapchainInfo.imageColorSpace   = vkFormat.colorSpace;
        swapchainInfo.imageExtent       = m_currentExtent;
        swapchainInfo.imageArrayLayers  = 1;
        swapchainInfo.imageUsage        = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.imageSharingMode  = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.preTransform      = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha    = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.presentMode       = presentMode;
        swapchainInfo.clipped           = true;
        swapchainInfo.oldSwapchain      = m_swapchain;
        vkCreateSwapchainKHR( device, &swapchainInfo, nullptr, &m_swapchain );

        if (oldSwapchain != VK_NULL_HANDLE)
            vkDestroySwapchainKHR( device, oldSwapchain, nullptr );

        _CreateImageViews( device, m_currentExtent.width, m_currentExtent.height, vkFormat.format );
    }

    //----------------------------------------------------------------------
    void Swapchain::recreate( U16 width, U16 height, bool vsync )
    {
        for (auto& swapImg : m_images)
            swapImg.view.release();
        create( g_vulkan.gpu.physicalDevice, g_vulkan.device, vsync );
    }

    //----------------------------------------------------------------------
    void Swapchain::acquireNextImageIndex( U64 timeout, VkSemaphore signalSem, VkFence fence )
    {
        auto result = vkAcquireNextImageKHR( g_vulkan.device, m_swapchain, timeout, signalSem, fence, &m_currentImageIndex );
        if (result != VK_SUCCESS)
            recreate( m_currentExtent.width, m_currentExtent.height, true );
    }

    //----------------------------------------------------------------------
    void Swapchain::setImageLayout( CmdBuffer& cmd, VkImageLayout targetLayout )
    {
        if (targetLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            // Transition layout only if image is not already in that layout. 
            // This can happen when the swapchain image was actually used as a rendertarget.
            if (m_images[m_currentImageIndex].image.layout != VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
                cmd.setImageLayout( &m_images[m_currentImageIndex].image, targetLayout,
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT );
        }
        else if (targetLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            cmd.setImageLayout( &m_images[m_currentImageIndex].image, targetLayout,
                                VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 
                                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        }
        else
        {
            ASSERT( false && "Wrong target-layout for swapchain-image." );
        }
    }

    //----------------------------------------------------------------------
    void Swapchain::present( VkQueue queue, VkSemaphore waitSemaphore )
    {
        const uint32_t imageIndices[] = { m_currentImageIndex };

        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        if (waitSemaphore)
        {
            presentInfo.waitSemaphoreCount  = 1;
            presentInfo.pWaitSemaphores     = &waitSemaphore;
        }
        presentInfo.swapchainCount      = 1;
        presentInfo.pSwapchains         = &m_swapchain;
        presentInfo.pImageIndices       = imageIndices;

        vkQueuePresentKHR( queue, &presentInfo );
    }

    //----------------------------------------------------------------------
    void Swapchain::bindForRendering()
    {
        g_vulkan.ctx.OMSetRenderTarget( &m_images[m_currentImageIndex].view, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
    }

    //----------------------------------------------------------------------
    void Swapchain::clear( Color color )
    {
        g_vulkan.ctx.SetClearColor( color );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    VkSurfaceFormatKHR Swapchain::_ChooseSurfaceFormat( VkPhysicalDevice physicalDevice, VkFormat requestedFormat )
    {
        U32 surfFmtCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, m_surface, &surfFmtCount, NULL );
        ArrayList<VkSurfaceFormatKHR> surfaceFormats( surfFmtCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, m_surface, &surfFmtCount, surfaceFormats.data() );

        for (auto surfFormat : surfaceFormats)
        {
            if (surfFormat.format == requestedFormat)
                return surfFormat;
        }
        LOG_WARN_RENDERING( "Vulkan::Swapchain: Could not find the requested format. Using another one instead." );
        return surfaceFormats[0];
    }

    //----------------------------------------------------------------------
    VkPresentModeKHR Swapchain::_ChoosePresentMode( VkPhysicalDevice physicalDevice, bool vsync )
    {
        U32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_surface, &presentModeCount, NULL );
        ArrayList<VkPresentModeKHR> presentModes( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_surface, &presentModeCount, presentModes.data() );

        if (not vsync)
        {
            for (auto pm : presentModes) // Try to find and use mailbox mode if available
                if (pm == VK_PRESENT_MODE_MAILBOX_KHR)
                    return VK_PRESENT_MODE_MAILBOX_KHR;
            for (auto pm : presentModes) // Otherwise use immediate mode if available
                if (pm == VK_PRESENT_MODE_IMMEDIATE_KHR)
                    return VK_PRESENT_MODE_IMMEDIATE_KHR;
            LOG_WARN_RENDERING( "Vulkan::Swapchain: NOT using vsync is not supported. Presentation will be vsynced." );
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    //----------------------------------------------------------------------
    U32 Swapchain::_GetDesiredNumberOfSwapchainImages( VkSurfaceCapabilitiesKHR surfaceCapabilities, VkPresentModeKHR swapchainPresentMode )
    {
        //Try to use Tripple buffering if Mailbox mode is available
        U32 imageCount = swapchainPresentMode == VK_PRESENT_MODE_MAILBOX_KHR ? 3 : 2;

        // Application must settle for fewer images than desired
        if (imageCount > surfaceCapabilities.maxImageCount)
            imageCount = surfaceCapabilities.maxImageCount;

        return imageCount;
    }

    //----------------------------------------------------------------------
    void Swapchain::_CreateImageViews( VkDevice device, U32 width, U32 height, VkFormat format )
    {
        U32 swapchainImageCount;
        vkGetSwapchainImagesKHR( device, m_swapchain, &swapchainImageCount, NULL );
        ArrayList<VkImage> swapchainImages( swapchainImageCount );
        vkGetSwapchainImagesKHR( device, m_swapchain, &swapchainImageCount, swapchainImages.data() );

        CmdBuffer cmd;
        cmd.create( g_vulkan.queueFamilyGraphicsIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT );
        cmd.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );

        m_images.resize( swapchainImageCount );
        for (I32 i = 0; i < swapchainImages.size(); ++i)
        {
            m_images[i].image.create( swapchainImages[i], width, height, format, VK_SAMPLE_COUNT_1_BIT );
            m_images[i].view.create( m_images[i].image );
            cmd.setImageLayout( &m_images[i].image, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_WRITE_BIT,
                                VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT );
        }
        cmd.exec( g_vulkan.graphicsQueue );
        cmd.wait();
        cmd.release();
    }

} } // End namespaces