#pragma once
/**********************************************************************
    class: Texture2D

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_texture2d.hpp"
#include "Vulkan/Vulkan.hpp"
#include "VkBindableTexture.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Texture2D : public ITexture2D, public IBindableTexture
    {
    public:
        Texture2D() = default;
        ~Texture2D() = default;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void create(U32 width, U32 height, TextureFormat format, bool generateMips) override;
        void create(U32 width, U32 height, TextureFormat format, const void* pData) override;
        void apply(bool updateMips, bool keepPixelsInRAM) override;

    private:
        struct
        {
            VkImage     img;
            VkImageView view;
        } m_image;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { IBindableTexture::_RecreateSampler( m_anisoLevel, m_filter, m_clampMode ); }
        void bind(const ShaderResourceDeclaration& res) override { IBindableTexture::bind( m_image.view, res ); }

        //----------------------------------------------------------------------
        void _CreateTexture();
        void _CreateTexture(const void* pData);
        void _PushToGPU();

        //----------------------------------------------------------------------
        NULL_COPY_AND_ASSIGN(Texture2D)
    };

} } // End namespaces