#include "VkTexture2D.h"
/**********************************************************************
    class: Texture2D

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "Utils/utils.h"
#include "Vulkan/VkUtility.h"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    void Texture2D::create( U32 width, U32 height, TextureFormat format, bool generateMips )
    {
        ASSERT( width > 0 && height > 0 && m_width == 0 && "Invalid params or texture were already created" );
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );

        m_isImmutable = false;
        _SetGenerateMips( generateMips );
        if (generateMips)
            _UpdateMipCount();

        _CreateTexture();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //----------------------------------------------------------------------
    void Texture2D::create( U32 width, U32 height, TextureFormat format, const void* pData )
    {
        ASSERT( width > 0 && height > 0 && pData != nullptr && m_width == 0 && "Invalid params or texture were already created" );
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );

        m_isImmutable = true;
        _CreateTexture( pData );
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Texture2D::_CreateTexture()
    {
        VezImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.imageType   = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format      = Utility::TranslateTextureFormat( m_format );
        imageCreateInfo.extent      = { m_width, m_height, 1 };
        imageCreateInfo.mipLevels   = m_mipCount;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples     = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling      = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage       = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        if (m_mipCount > 1)
            imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

        VALIDATE( vezCreateImage( g_vulkan.device, VEZ_MEMORY_GPU_ONLY, &imageCreateInfo, &m_image.img ) );

        // Create the image view for binding the texture as a resource.
        VezImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.image    = m_image.img;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format   = imageCreateInfo.format;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        imageViewCreateInfo.subresourceRange.levelCount = m_mipCount;
        VALIDATE( vezCreateImageView( g_vulkan.device, &imageViewCreateInfo, &m_image.view ) );
    }

    //----------------------------------------------------------------------
    void Texture2D::_CreateTexture( const void* pData )
    {
        _CreateTexture();

        VezImageSubDataInfo subDataInfo = {};
        subDataInfo.imageSubresource.mipLevel = 0;
        subDataInfo.imageSubresource.layerCount = 1;
        subDataInfo.imageExtent = { m_width, m_height, 1 };
        vezImageSubData( g_vulkan.device, m_image.img, &subDataInfo, pData );
    }

    //----------------------------------------------------------------------
    void Texture2D::_PushToGPU()
    {
        ASSERT( not m_pixels.empty() );

        // Upload the host side data
        VezImageSubDataInfo subDataInfo{};
        subDataInfo.imageSubresource.mipLevel = 0;
        subDataInfo.imageSubresource.layerCount = 1;
        subDataInfo.imageExtent = { m_width, m_height, 1 };
        vezImageSubData( g_vulkan.device, m_image.img, &subDataInfo, m_pixels.data() );

        if ( not keepPixelsInRAM() )
            m_pixels.clear();
    }

    //----------------------------------------------------------------------
    void Texture2D::_GenerateMips()
    {
        g_vulkan.ctx.GenerateMips( m_image.img, m_width, m_height, m_mipCount );
    }

} } // End namespaces