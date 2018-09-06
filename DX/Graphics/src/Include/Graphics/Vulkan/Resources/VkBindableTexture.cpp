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
        vezDeviceWaitIdle( g_vulkan.device );
        vezDestroyImage( g_vulkan.device, m_image.img );
        vezDestroyImageView( g_vulkan.device, m_image.view );
        _DestroySampler();
    }

    //**********************************************************************
    // PROTECTED
    //**********************************************************************

    //----------------------------------------------------------------------
    void IBindableTexture::bind( const ShaderResourceDeclaration& res )
    {
        if (not m_gpuUpToDate)
        {
            _PushToGPU();
            m_gpuUpToDate = true;
        }

        g_vulkan.ctx.SetImage( m_image.view, m_sampler, res.getBindingSet(), res.getBindingSlot() );
    }

    //----------------------------------------------------------------------
    void IBindableTexture::apply( bool updateMips, bool keepPixelsInRAM )
    {
        m_keepPixelsInRAM = keepPixelsInRAM;
        m_gpuUpToDate = false;
        if (updateMips)
            _GenerateMips();
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void IBindableTexture::_CreateSampler( U32 anisoLevel, TextureFilter filter, TextureAddressMode addressMode )
    {
        // Destroy old sampler if any
        _DestroySampler();

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
        m_sampler = VK_NULL_HANDLE;
    }

} } // End namespaces