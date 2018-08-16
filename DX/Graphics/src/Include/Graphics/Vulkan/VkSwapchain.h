#pragma once
/**********************************************************************
    class: Swapchain

    author: S. Hau
    date: August 16, 2018
**********************************************************************/

#include "Vulkan.hpp"
#include "OS/Window/window.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Swapchain
    {
    public:
        Swapchain() = default;
        ~Swapchain();

        void init(OS::Window* window);
        void create(VkPhysicalDevice physicalDevice, VkDevice device);

        VkSurfaceKHR getSurfaceKHR() const { return m_surface; }

    private:
        VkSurfaceKHR    m_surface;
        VkSwapchainKHR  m_swapchain;

        NULL_COPY_AND_ASSIGN(Swapchain)
    };


} } // End namespaces