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
        void init(VkInstance instance, OS::Window* window);
        void create(VkPhysicalDevice physicalDevice, VkDevice device, bool vsync);
        void shutdown(VkInstance instance, VkDevice device);

        //----------------------------------------------------------------------
        void acquireNextImageIndex(U64 timeout, VkSemaphore signalSem = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE);
        void recreate(U16 width, U16 height, bool vsync);
        void present(VkQueue queue, VkSemaphore waitSemaphore = VK_NULL_HANDLE);
        void bindForRendering();
        void clear(Color color);
        void setImageLayout(CmdBuffer& cmd, VkImageLayout targetLayout);

    private:
        VkSurfaceKHR    m_surface = VK_NULL_HANDLE;
        VkSwapchainKHR  m_swapchain = VK_NULL_HANDLE;
        VkExtent2D      m_currentExtent{};

        struct SwapchainImage
        {
            ColorImage image;
            ImageView view;
        };
        ArrayList<SwapchainImage>   m_images;
        U32                         m_currentImageIndex = 0;

        //----------------------------------------------------------------------
        VkPresentModeKHR _ChoosePresentMode(VkPhysicalDevice physicalDevice, bool vsync);
        VkSurfaceFormatKHR _ChooseSurfaceFormat(VkPhysicalDevice physicalDevice, VkFormat requestedFormat);
        U32 _GetDesiredNumberOfSwapchainImages(VkSurfaceCapabilitiesKHR surfaceCapabilities, VkPresentModeKHR swapchainPresentMode);
        void _CreateImageViews(VkDevice device, U32 width, U32 height, VkFormat format);

        NULL_COPY_AND_ASSIGN(Swapchain)
    };


} } // End namespaces