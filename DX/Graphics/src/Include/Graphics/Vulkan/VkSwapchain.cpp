#include "VkSwapchain.h"
/**********************************************************************
    class: Swapchain

    author: S. Hau
    date: August 16, 2018
**********************************************************************/

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Swapchain::createSurface( VkInstance instance, OS::Window* window )
    {
        VkWin32SurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        createInfo.hinstance = window->getInstance();
        createInfo.hwnd      = window->getHWND();

        VALIDATE( vkCreateWin32SurfaceKHR( instance, &createInfo, ALLOCATOR, &m_surface ) );
    }

    //----------------------------------------------------------------------
    void Swapchain::createSwapchain(  VkDevice device, U32 width, U32 height, VkFormat requestedFormat )
    {
        m_extent = { width, height };

        // Destroy old swapchain if any
        if (m_swapchain)
            vezDestroySwapchain( device, m_swapchain );

        // Create the swapchain.
        VezSwapchainCreateInfo swapchainCreateInfo = {};
        swapchainCreateInfo.surface = m_surface;
        swapchainCreateInfo.format = { requestedFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        VALIDATE( vezCreateSwapchain( device, &swapchainCreateInfo, &m_swapchain ) );

        vezGetSwapchainSurfaceFormat( m_swapchain, &m_surfaceFormat );

        _CreateImage( width, height, m_surfaceFormat.format );
    }

    //----------------------------------------------------------------------
    void Swapchain::shutdown( VkInstance instance, VkDevice device )
    {
        _DestroyImage();
        vezDestroySwapchain( device, m_swapchain );
        vkDestroySurfaceKHR( instance, m_surface, ALLOCATOR );
    }

    //----------------------------------------------------------------------
    void Swapchain::recreate( U32 w, U32 h )
    {
        _DestroyImage();
        _CreateImage( w, h, m_surfaceFormat.format );
    }

    //----------------------------------------------------------------------
    void Swapchain::present( VkQueue queue, VkSemaphore waitSemaphore )
    {
        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VezPresentInfo presentInfo = {};
        presentInfo.waitSemaphoreCount  = 1;
        presentInfo.pWaitSemaphores     = &waitSemaphore;
        presentInfo.pWaitDstStageMask   = &waitDstStageMask;
        presentInfo.swapchainCount      = 1;
        presentInfo.pSwapchains         = &m_swapchain;
        presentInfo.pImages             = &m_swapchainImage;
        if ( vezQueuePresent( queue, &presentInfo ) != VK_SUCCESS )
            recreate( m_extent.width, m_extent.height );
    }

    //----------------------------------------------------------------------
    void Swapchain::bindForRendering()
    {
        //g_vulkan.ctx.OMSetRenderTarget( m_images[m_currentImageIndex].view, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
    }

    //----------------------------------------------------------------------
    void Swapchain::clear( Color color )
    {
        g_vulkan.ctx.SetClearColor( color );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    ////----------------------------------------------------------------------
    //VkSurfaceFormatKHR Swapchain::_ChooseSurfaceFormat( VkPhysicalDevice physicalDevice, VkFormat requestedFormat )
    //{
    //    U32 surfFmtCount;
    //    vezGetPhysicalDeviceSurfaceFormats( physicalDevice, m_surface, &surfFmtCount, NULL );
    //    ArrayList<VkSurfaceFormatKHR> surfaceFormats( surfFmtCount );
    //    vezGetPhysicalDeviceSurfaceFormats( physicalDevice, m_surface, &surfFmtCount, surfaceFormats.data() );

    //    for (auto surfFormat : surfaceFormats)
    //    {
    //        if (surfFormat.format == requestedFormat)
    //            return surfFormat;
    //    }
    //    LOG_WARN_RENDERING( "Vulkan::Swapchain: Could not find the requested format. Using another one instead." );
    //    return surfaceFormats[0];
    //}

    ////----------------------------------------------------------------------
    //VkPresentModeKHR Swapchain::_ChoosePresentMode( VkPhysicalDevice physicalDevice, bool vsync )
    //{
    //    U32 presentModeCount;
    //    vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_surface, &presentModeCount, NULL );
    //    ArrayList<VkPresentModeKHR> presentModes( presentModeCount );
    //    vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_surface, &presentModeCount, presentModes.data() );

    //    if (not vsync)
    //    {
    //        for (auto pm : presentModes) // Try to find and use mailbox mode if available
    //            if (pm == VK_PRESENT_MODE_MAILBOX_KHR)
    //                return VK_PRESENT_MODE_MAILBOX_KHR;
    //        for (auto pm : presentModes) // Otherwise use immediate mode if available
    //            if (pm == VK_PRESENT_MODE_IMMEDIATE_KHR)
    //                return VK_PRESENT_MODE_IMMEDIATE_KHR;
    //        LOG_WARN_RENDERING( "Vulkan::Swapchain: NOT using vsync is not supported. Presentation will be vsynced." );
    //    }
    //    return VK_PRESENT_MODE_FIFO_KHR;
    //}

    ////----------------------------------------------------------------------
    //U32 Swapchain::_GetDesiredNumberOfSwapchainImages( VkSurfaceCapabilitiesKHR surfaceCapabilities, VkPresentModeKHR swapchainPresentMode )
    //{
    //    //Try to use Tripple buffering if Mailbox mode is available
    //    U32 imageCount = swapchainPresentMode == VK_PRESENT_MODE_MAILBOX_KHR ? 3 : 2;

    //    // Application must settle for fewer images than desired
    //    if (imageCount > surfaceCapabilities.maxImageCount)
    //        imageCount = surfaceCapabilities.maxImageCount;

    //    return imageCount;
    //}

    //----------------------------------------------------------------------
    void Swapchain::_CreateImage( U32 width, U32 height, VkFormat format )
    {
        VezImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.imageType   = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format      = format;
        imageCreateInfo.extent      = { width, height, 1 };
        imageCreateInfo.mipLevels   = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples     = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling      = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage       = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        vezCreateImage( g_vulkan.device, VEZ_MEMORY_GPU_ONLY, &imageCreateInfo, &m_swapchainImage );
    }

    //----------------------------------------------------------------------
    void Swapchain::_DestroyImage()
    {
        vezDestroyImage( g_vulkan.device, m_swapchainImage );
    }

} } // End namespaces