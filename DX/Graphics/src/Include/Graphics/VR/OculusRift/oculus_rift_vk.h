#pragma once
/**********************************************************************
    class: OculusRiftVk

    author: S. Hau
    date: August 18, 2018
**********************************************************************/

#include "oculus_rift.h"

// Include API dependant header
#include "LibOVR/OVR_CAPI_Vk.h"

namespace Graphics { namespace VR {

    //----------------------------------------------------------------------
    // Splits given extentions (splitted by whitespace) into an array-list
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
        OculusSwapchainVk( ovrSession session, VkDevice device, I32 sizeW, I32 sizeH )
        {
            ovrTextureSwapChainDesc colorDesc = {};
            colorDesc.Type = ovrTexture_2D;
            colorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM;
            colorDesc.ArraySize = 1;
            colorDesc.Width = sizeW;
            colorDesc.Height = sizeH;
            colorDesc.MipLevels = 1;
            colorDesc.SampleCount = 1;
            colorDesc.StaticImage = ovrFalse;
            colorDesc.MiscFlags = ovrTextureMisc_DX_Typeless;
            colorDesc.BindFlags = ovrTextureBind_DX_RenderTarget;

            ovr_CreateTextureSwapChainVk( session, device, &colorDesc, &m_swapChain );
            I32 count = 0;
            ovr_GetTextureSwapChainLength( session, m_swapChain, &count );
            for (I32 i = 0; i < count; ++i)
            {
                VkImage image;
                ovr_GetTextureSwapChainBufferVk( session, m_swapChain, i, &image );
                VkImageViewCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
                createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
                createInfo.image = image;
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                createInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
                createInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
                vkCreateImageView( device, &createInfo, ALLOCATOR, &m_imageViews[i] );
            }
        }

        //----------------------------------------------------------------------
        // OculusSwapchain Interface
        //----------------------------------------------------------------------
        void bindForRendering(ovrSession session) override
        {
        }

        void clear(ovrSession session, Color color) override
        {
        }

    private:
        VkImageView m_imageViews[3];
        VkImageView _GetRTV( ovrSession session )
        {
            int index = 0;
            ovr_GetTextureSwapChainCurrentIndex( session, m_swapChain, &index );
            return m_imageViews[index];
        }
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