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
        void create();

    private:
        VkSurfaceKHR m_surface;

        NULL_COPY_AND_ASSIGN(Swapchain)
    };


} } // End namespaces