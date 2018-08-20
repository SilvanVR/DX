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
        for (auto& view : m_imageViews)
            vkDestroyImageView( device, view, ALLOCATOR );
        vkDestroySwapchainKHR( device, m_swapchain, ALLOCATOR );
        vkDestroySurfaceKHR( instance, m_surface, ALLOCATOR );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Swapchain::create( VkPhysicalDevice physicalDevice, VkDevice device )
    {
        VkSurfaceCapabilitiesKHR surfCaps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice, m_surface, &surfCaps );

        auto vkFormat = _ChooseSurfaceFormat( physicalDevice, FORMAT );
        auto presentMode = _ChoosePresentMode( physicalDevice, true );

        VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainInfo.surface           = m_surface;
        swapchainInfo.minImageCount     = _GetDesiredNumberOfSwapchainImages( surfCaps, presentMode );
        swapchainInfo.imageFormat       = vkFormat.format;
        swapchainInfo.imageColorSpace   = vkFormat.colorSpace;
        swapchainInfo.imageExtent       = surfCaps.currentExtent;
        swapchainInfo.imageArrayLayers  = 1;
        swapchainInfo.imageUsage        = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.imageSharingMode  = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.preTransform      = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha    = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.presentMode       = presentMode;
        swapchainInfo.clipped           = true;
        swapchainInfo.oldSwapchain      = m_swapchain;
        vkCreateSwapchainKHR( device, &swapchainInfo, nullptr, &m_swapchain );

        _CreateImageViews( device, vkFormat.format );
    }

    //----------------------------------------------------------------------
    void Swapchain::recreate( U16 width, U16 height )
    {
        for (auto& view : m_imageViews)
            vkDestroyImageView( g_vulkan.device, view, ALLOCATOR );
        create( g_vulkan.gpu.physicalDevice, g_vulkan.device );
    }

    //----------------------------------------------------------------------
    U32 Swapchain::getNextImageIndex( U64 timeout, VkSemaphore signalSem, VkFence fence )
    {
        U32 imageIndex;
        vkAcquireNextImageKHR( g_vulkan.device, m_swapchain, timeout, signalSem, fence, &imageIndex );
        return imageIndex;
    }

    //----------------------------------------------------------------------
    void Swapchain::present( VkQueue queue, const ArrayList<VkSemaphore>& waitSemaphores, U32 imageIndex )
    {
        const uint32_t imageIndices[] = { imageIndex };

        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.waitSemaphoreCount  = (U32)waitSemaphores.size();
        presentInfo.pWaitSemaphores     = waitSemaphores.data();
        presentInfo.swapchainCount      = 1;
        presentInfo.pSwapchains         = &m_swapchain;
        presentInfo.pImageIndices       = imageIndices;

        vkQueuePresentKHR( queue, &presentInfo );
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
    void Swapchain::_CreateImageViews( VkDevice device, VkFormat format )
    {
        U32 swapchainImageCount;
        vkGetSwapchainImagesKHR( device, m_swapchain, &swapchainImageCount, NULL );
        ArrayList<VkImage> swapchainImages( swapchainImageCount );
        vkGetSwapchainImagesKHR( device, m_swapchain, &swapchainImageCount, swapchainImages.data() );

        m_imageViews.resize( swapchainImageCount );

        for (I32 i = 0; i < swapchainImages.size(); ++i)
        {
            VkImageViewCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            createInfo.format   = format;
            createInfo.image    = swapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
            createInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
            vkCreateImageView( device, &createInfo, ALLOCATOR, &m_imageViews[i] );
        }
    }

} } // End namespaces