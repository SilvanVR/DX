#include "VkCubemap.h"
/**********************************************************************
    class: Cubemap

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "Utils/utils.h"
#include "Vulkan/VkUtility.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Cubemap::create( I32 size, TextureFormat format, Mips mips )
    {
        ASSERT( size > 0 );
        ITexture::_Init( TextureDimension::Cube, size, size, format );

        if (mips == Mips::Generate || mips == Mips::Create)
        {
            _UpdateMipCount();
            _GenerateMips();
        }

        _CreateTexture();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Cubemap::_CreateTexture()
    {
        VezImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.imageType   = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format      = Utility::TranslateTextureFormat( m_format );
        imageCreateInfo.extent      = { m_width, m_height, 1 };
        imageCreateInfo.mipLevels   = m_mipCount;
        imageCreateInfo.arrayLayers = 6;
        imageCreateInfo.samples     = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling      = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage       = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.flags       = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        if (m_mipCount > 1)
            imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        VALIDATE( vezCreateImage( g_vulkan.device, VEZ_MEMORY_GPU_ONLY, &imageCreateInfo, &m_image.img ) );

        // Create the image view for binding the texture as a resource.
        VezImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.image    = m_image.img;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        imageViewCreateInfo.format   = imageCreateInfo.format;
        imageViewCreateInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        imageViewCreateInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        VALIDATE( vezCreateImageView( g_vulkan.device, &imageViewCreateInfo, &m_image.view ) );
    }

    //----------------------------------------------------------------------
    void Cubemap::_PushToGPU()
    {
        // Copy the data into the texture
        U32 rowPitch = ( getWidth() * ByteCountFromTextureFormat( m_format ) );
        for (U32 face = 0; face < NUM_FACES; face++)
        {
            // Upload data to gpu
            VezImageSubDataInfo subDataInfo = {};
            subDataInfo.imageSubresource.mipLevel       = 0;
            subDataInfo.imageSubresource.baseArrayLayer = face;
            subDataInfo.imageSubresource.layerCount     = 1;
            subDataInfo.imageExtent                     = { m_width, m_height, 1 };
            vezImageSubData( g_vulkan.device, m_image.img, &subDataInfo, m_facePixels[(I32)face].data() );

            // Free mem in RAM if desired
            if ( not keepPixelsInRAM() )
                m_facePixels[(I32)face].clear();
        }
    }

    //----------------------------------------------------------------------
    void Cubemap::_GenerateMips()
    {
        g_vulkan.ctx.GenerateMips( m_image.img, m_width, m_height, m_mipCount, 6 );
    }

} } // End namespaces