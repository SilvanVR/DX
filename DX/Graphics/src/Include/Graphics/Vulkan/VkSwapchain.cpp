#include "VkSwapchain.h"
/**********************************************************************
    class: Swapchain

    author: S. Hau
    date: August 16, 2018
**********************************************************************/

#define FORMAT VK_FORMAT_B8G8R8A8_UNORM

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
    void Swapchain::create( VkPhysicalDevice physicalDevice, VkDevice device )
    {
        VkSurfaceCapabilitiesKHR surfCaps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &surfCaps );

        U32 surfFmtCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, m_surface, &surfFmtCount, NULL );
        ArrayList<VkSurfaceFormatKHR> surfaceFormats( surfFmtCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, m_surface, &surfFmtCount, surfaceFormats.data() );
        U32 foundFmt;
        for (foundFmt = 0; foundFmt < surfFmtCount; ++foundFmt)
        {
            if (surfaceFormats[foundFmt].format == FORMAT)
                break;
        }

        ASSERT(foundFmt < surfFmtCount);

        U32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_surface, &presentModeCount, NULL );
        ArrayList<VkPresentModeKHR> presentModes( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, m_surface, &presentModeCount, presentModes.data() );

        // Do not use VSYNC for the mirror window, but Nvidia doesn't support IMMEDIATE so fall back to MAILBOX
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        for (uint32_t i = 0; i < presentModeCount; ++i)
        {
            if ((presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) || (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR))
            {
                presentMode = presentModes[i];
                break;
            }
        }

        VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainInfo.flags = 0;
        swapchainInfo.surface = m_surface;
        swapchainInfo.minImageCount = surfCaps.minImageCount;
        swapchainInfo.imageFormat = FORMAT;
        swapchainInfo.imageColorSpace = surfaceFormats[foundFmt].colorSpace;
        swapchainInfo.imageExtent = surfCaps.currentExtent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.queueFamilyIndexCount = 0;
        swapchainInfo.pQueueFamilyIndices = nullptr;
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.presentMode = presentMode;
        swapchainInfo.clipped = true;
        swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
        vkCreateSwapchainKHR( device, &swapchainInfo, nullptr, &m_swapchain );

        //// Fence to throttle host on aquire
        //VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        //vkCreateFence(Platform.device, &fenceInfo, nullptr, &readyFence);

        //swapchainCount = countof(image);
        //vkGetSwapchainImagesKHR(Platform.device, swapchain, &swapchainCount, image);
        //if (swapchainCount > maxImages)
        //{
        //    Debug.Log("Reducing swapchain length from " + std::to_string(swapchainCount) + " to " + std::to_string(maxImages));
        //    swapchainCount = maxImages;
        //}
    }

} } // End namespaces