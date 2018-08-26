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
        ~Swapchain() = default;

        //----------------------------------------------------------------------
        VkSurfaceKHR getSurfaceKHR() const { return m_surface; }

        //----------------------------------------------------------------------
        void createSurface(VkInstance instance, OS::Window* window);
        void createSwapchain(VkDevice device, U32 width, U32 height, VkFormat requestedFormat);
        void shutdown(VkInstance instance, VkDevice device);
        void present(VkQueue queue, VkSemaphore waitSemaphore = VK_NULL_HANDLE);
        void bindForRendering();
        void clear(Color color);
        void recreate(U32 w, U32 h);

    private:
        VkSurfaceKHR        m_surface   = VK_NULL_HANDLE;
        VezSwapchain        m_swapchain = VK_NULL_HANDLE;
        VkSurfaceFormatKHR  m_surfaceFormat{ VK_FORMAT_UNDEFINED };
        VkImage             m_swapchainImage = VK_NULL_HANDLE;
        VkExtent2D          m_extent{};

        //----------------------------------------------------------------------
        void _CreateImage(U32 width, U32 height, VkFormat format);
        void _DestroyImage();

        NULL_COPY_AND_ASSIGN(Swapchain)
    };


} } // End namespaces