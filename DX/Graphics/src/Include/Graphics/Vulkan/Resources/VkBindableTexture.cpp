#include "VkBindableTexture.h"
/**********************************************************************
    class: IBindableTexture

    author: S. Hau
    date: August 29, 2018
**********************************************************************/

#include "Vulkan/VkUtility.h"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    IBindableTexture::~IBindableTexture()
    {
        _DestroySampler();
    }

    //----------------------------------------------------------------------
    void IBindableTexture::bind( VkImageView view, const ShaderResourceDeclaration& res )
    {
        if (not m_gpuUpToDate)
        {
            _PushToGPU();
            m_gpuUpToDate = true;
        }

        if (m_generateMips)
        {
            //g_pImmediateContext->GenerateMips( m_pTextureView );
            //g_vulkan.ctx.GenerateMips(img, mipLevels);
            m_generateMips = false;
        }

        g_vulkan.ctx.SetImage( view, m_sampler, res.getBindingSet(), res.getBindingSlot() );
    }

    //----------------------------------------------------------------------
    void IBindableTexture::_CreateSampler( U32 anisoLevel, TextureFilter filter, TextureAddressMode addressMode )
    {
        ASSERT( m_sampler == VK_NULL_HANDLE );
        auto vkFilter = Utility::TranslateFilter( filter );
        auto vkClampMode = Utility::TranslateClampMode( addressMode );

        VkSamplerCreateInfo createInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        createInfo.magFilter        = vkFilter.first;
        createInfo.minFilter        = vkFilter.first;
        createInfo.mipmapMode       = vkFilter.second;
        createInfo.addressModeU     = vkClampMode;
        createInfo.addressModeV     = vkClampMode;
        createInfo.addressModeW     = vkClampMode;
        createInfo.mipLodBias       = 0.0f;
        createInfo.anisotropyEnable = anisoLevel > 1 ? VK_TRUE : VK_FALSE;
        createInfo.maxAnisotropy    = (F32)anisoLevel;
        createInfo.minLod           = 0.0f;
        createInfo.maxLod           = std::numeric_limits<F32>::max();

        vkCreateSampler( g_vulkan.device, &createInfo, ALLOCATOR, &m_sampler );
    }

    //----------------------------------------------------------------------
    void IBindableTexture::_DestroySampler()
    {
        vkDestroySampler( g_vulkan.device, m_sampler, ALLOCATOR );
    }

} } // End namespaces