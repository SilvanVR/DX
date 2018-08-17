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

        //----------------------------------------------------------------------
        VkSurfaceKHR getSurfaceKHR() const { return m_surface; }

        //----------------------------------------------------------------------
        void init(VkInstance instance, OS::Window* window);
        void create(VkPhysicalDevice physicalDevice, VkDevice device);

        //----------------------------------------------------------------------
        void recreate(U16 width, U16 height);
        void present(VkQueue queue, const ArrayList<VkSemaphore>& waitSemaphores, U32 imageIndex);
        U32 getNextImageIndex(U64 timeout, VkSemaphore signalSem = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE);

        //----------------------------------------------------------------------
        //void bindForRendering();
        //void clear(Color color);

    private:
        VkSurfaceKHR            m_surface = VK_NULL_HANDLE;
        VkSwapchainKHR          m_swapchain = VK_NULL_HANDLE;
        ArrayList<VkImageView>  m_imageViews;

        //----------------------------------------------------------------------
        VkPresentModeKHR _ChoosePresentMode(VkPhysicalDevice physicalDevice, bool vsync);
        VkSurfaceFormatKHR _ChooseSurfaceFormat(VkPhysicalDevice physicalDevice, VkFormat requestedFormat);
        U32 _GetDesiredNumberOfSwapchainImages(VkSurfaceCapabilitiesKHR surfaceCapabilities, VkPresentModeKHR swapchainPresentMode);
        void _CreateImageViews(VkDevice device, VkFormat format);

        NULL_COPY_AND_ASSIGN(Swapchain)
    };


} } // End namespaces