#include "VkSwapchain.h"
/**********************************************************************
    class: Swapchain

    author: S. Hau
    date: August 16, 2018
**********************************************************************/

#include "VkPlatform.h"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    void Swapchain::init( OS::Window* window )
    {
        VkWin32SurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        createInfo.hinstance = window->getInstance();
        createInfo.hwnd = window->getHWND();

        VALIDATE( vkCreateWin32SurfaceKHR( g_vulkan.instance, &createInfo, ALLOCATOR, &m_surface ) );
    }

    //----------------------------------------------------------------------
    Swapchain::~Swapchain()
    {
        vkDestroySurfaceKHR( g_vulkan.instance, m_surface, ALLOCATOR );
    }

    //----------------------------------------------------------------------
    void Swapchain::create()
    {

        //vkCreateSwapchainKHR( device, &createInfo, ALLOCATOR, m_pSwapchain );
    }

} } // End namespaces