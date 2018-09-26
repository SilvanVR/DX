#pragma once
/**********************************************************************
    class: OculusRiftVk

    author: S. Hau
    date: August 18, 2018
**********************************************************************/

#include "oculus_rift.h"

// Include API dependant header
#include "LibOVR/OVR_CAPI_Vk.h"

#include "Vulkan/Ext/VEZ_ext.h"

namespace Graphics { namespace VR {

    //----------------------------------------------------------------------
    // Splits given extensions (splitted by whitespace) into an array-list
    //----------------------------------------------------------------------
    ArrayList<String> ParseOVRExtentionsVk( char* extentions, U32 size )
    {
        ArrayList<String> result(1);
        U32 count = 0;
        while (++count < size)
        {
            if (*extentions == ' ')
                result.emplace_back( "" );
            else
                result.back() += *extentions;
            extentions++; 
        }
        return result;
    }

    //**********************************************************************
    class OculusSwapchainVk : public OculusSwapchain
    {
    public:
        OculusSwapchainVk( ovrSession session, VkDevice device, I32 width, I32 height )
        {
            ovrTextureSwapChainDesc colorDesc = {};
            colorDesc.Type        = ovrTexture_2D;
            colorDesc.Format      = OVR_FORMAT_B8G8R8A8_UNORM; // OVR_FORMAT_B8G8R8A8_UNORM_SRGB
            colorDesc.ArraySize   = 1;
            colorDesc.Width       = width;
            colorDesc.Height      = height;
            colorDesc.MipLevels   = 1;
            colorDesc.SampleCount = 1;
            colorDesc.StaticImage = ovrFalse;
            colorDesc.MiscFlags   = ovrTextureMisc_DX_Typeless;
            colorDesc.BindFlags   = ovrTextureBind_DX_RenderTarget;

            ovr_CreateTextureSwapChainVk( session, device, &colorDesc, &m_swapChain );
            I32 count = 0;
            ovr_GetTextureSwapChainLength( session, m_swapChain, &count );
            m_imageViews.resize( count );
            m_fbos.resize( count );
            for (I32 i = 0; i < count; ++i)
            {
                VkImage image;
                ovr_GetTextureSwapChainBufferVk( session, m_swapChain, i, &image );

                VkExtent3D extent{ (U32)width, (U32)height, 1 };
                vezImportVkImage( device, image, VK_FORMAT_B8G8R8A8_UNORM, extent, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );

                VezImageViewCreateInfo createInfo{};
                createInfo.format   = VK_FORMAT_B8G8R8A8_UNORM; // VK_FORMAT_B8G8R8A8_SRGB
                createInfo.image    = image;
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                createInfo.subresourceRange.layerCount = 1;
                createInfo.subresourceRange.levelCount = 1;
                VALIDATE( vezCreateImageView( device, &createInfo, &m_imageViews[i] ) );

                m_fbos[i].create( (U32)width, (U32)height, 1, &m_imageViews[i], VK_SAMPLE_COUNT_1_BIT );
            }
        }

        //----------------------------------------------------------------------
        ~OculusSwapchainVk()
        {
            for (auto& view : m_imageViews)
                vkDestroyImageView( g_vulkan.device, view, ALLOCATOR );
            for (auto& fbo : m_fbos)
                fbo.destroy();
        }

        //----------------------------------------------------------------------
        // OculusSwapchain Interface
        //----------------------------------------------------------------------
        void bindForRendering( ovrSession session ) override
        {
            int index = 0;
            ovr_GetTextureSwapChainCurrentIndex( session, m_swapChain, &index );
            g_vulkan.ctx.OMSetRenderTarget( m_fbos[index] );
        }

        //----------------------------------------------------------------------
        void clear( ovrSession session, Color color ) override
        {
            int index = 0;
            ovr_GetTextureSwapChainCurrentIndex( session, m_swapChain, &index );
            m_fbos[index].setClearColor( 0, color );
        }

    private:
        ArrayList<VkImageView>          m_imageViews;
        ArrayList<Vulkan::Framebuffer>  m_fbos;
    };

    //**********************************************************************
    class OculusRiftVk : public OculusRift
    {
    public:
        OculusRiftVk() = default;
        ~OculusRiftVk() = default;

        //----------------------------------------------------------------------
        ArrayList<String> getRequiredInstanceExtentions()
        {
            char instExtensionNames[4096];
            U32 instExtensionNamesSize = sizeof( instExtensionNames );
            ovr_GetInstanceExtensionsVk( getGraphicsLuid(), instExtensionNames, &instExtensionNamesSize );
            return ParseOVRExtentionsVk( instExtensionNames, instExtensionNamesSize );
        }

        //----------------------------------------------------------------------
        VkPhysicalDevice getPhysicalDevice( VkInstance instance )
        {
            VkPhysicalDevice physicalDevice;
            ovr_GetSessionPhysicalDeviceVk( getSession(), getGraphicsLuid(), instance, &physicalDevice );
            return physicalDevice;
        }

        //----------------------------------------------------------------------
        ArrayList<String> getRequiredDeviceExtentions()
        {
            char deviceExtensionNames[4096];
            U32 deviceExtensionNamesSize = sizeof( deviceExtensionNames );
            ovr_GetDeviceExtensionsVk( getGraphicsLuid(), deviceExtensionNames, &deviceExtensionNamesSize );
            return ParseOVRExtentionsVk( deviceExtensionNames, deviceExtensionNamesSize );
        }

        //----------------------------------------------------------------------
        void setSynchronizationQueueVk( VkQueue queue )
        {
            ovr_SetSynchronizationQueueVk( getSession(), queue );
        }

        //----------------------------------------------------------------------
        void createEyeBuffers( VkDevice device )
        {
            auto desc = getDescription();
            for (auto eye : { LeftEye, RightEye })
                m_eyeBuffers[eye] = new OculusSwapchainVk( getSession(), device, desc.idealResolution[eye].x, desc.idealResolution[eye].y );
        }

    private:
        NULL_COPY_AND_ASSIGN(OculusRiftVk)
    };

} } // End namespaces