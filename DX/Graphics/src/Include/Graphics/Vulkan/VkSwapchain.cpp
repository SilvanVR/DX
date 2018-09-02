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

        _CreateSwapchainFBO( width, height, m_surfaceFormat.format );
    }

    //----------------------------------------------------------------------
    void Swapchain::shutdown( VkInstance instance, VkDevice device )
    {
        _DestroySwapchainFBO();
        vezDestroySwapchain( device, m_swapchain );
        vkDestroySurfaceKHR( instance, m_surface, ALLOCATOR );
    }

    //----------------------------------------------------------------------
    void Swapchain::recreate( U32 w, U32 h )
    {
        _DestroySwapchainFBO();
        _CreateSwapchainFBO( w, h, m_surfaceFormat.format );
    }

    //----------------------------------------------------------------------
    void Swapchain::present( VkQueue queue, VkSemaphore waitSemaphore )
    {
        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VezPresentInfo presentInfo = {};
        presentInfo.waitSemaphoreCount  = waitSemaphore ? 1 : 0;
        presentInfo.pWaitSemaphores     = &waitSemaphore;
        presentInfo.pWaitDstStageMask   = &waitDstStageMask;
        presentInfo.swapchainCount      = 1;
        presentInfo.pSwapchains         = &m_swapchain;
        presentInfo.pImages             = &m_framebuffer.img;

        auto result = vezQueuePresent( queue, &presentInfo );
        if ( result != VK_SUCCESS )
            LOG_WARN_RENDERING( "VkRenderer: Could not present image to the surface." );
    }

    //----------------------------------------------------------------------
    void Swapchain::bindForRendering()
    {
        g_vulkan.ctx.OMSetRenderTarget( m_framebuffer.fbo );
    }

    //----------------------------------------------------------------------
    void Swapchain::setClearColor( Color color )
    {
        m_framebuffer.fbo.setClearColor( 0, color );
    }

    //----------------------------------------------------------------------
    void Swapchain::setVSync( bool enabled )
    {
        VALIDATE( vezSwapchainSetVSync( m_swapchain, enabled ) );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Swapchain::_CreateSwapchainFBO( U32 width, U32 height, VkFormat format )
    {
        VezImageCreateInfo imageCreateInfo{};
        imageCreateInfo.imageType   = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format      = format;
        imageCreateInfo.extent      = { width, height, 1 };
        imageCreateInfo.mipLevels   = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples     = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling      = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage       = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        VALIDATE( vezCreateImage( g_vulkan.device, VEZ_MEMORY_GPU_ONLY, &imageCreateInfo, &m_framebuffer.img ) );

        VezImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.image    = m_framebuffer.img;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format   = format;
        viewCreateInfo.subresourceRange = { 0, 1, 0, 1 };
        VALIDATE( vezCreateImageView( g_vulkan.device, &viewCreateInfo, &m_framebuffer.view ) );

        m_framebuffer.fbo.create( width, height, 1, &m_framebuffer.view );
        setClearColor( Color::BLACK );
    }

    //----------------------------------------------------------------------
    void Swapchain::_DestroySwapchainFBO()
    {
        vezDestroyImage( g_vulkan.device, m_framebuffer.img );
        vezDestroyImageView( g_vulkan.device, m_framebuffer.view );
        m_framebuffer.fbo.destroy();
    }

} } // End namespaces