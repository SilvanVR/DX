#pragma once
/**********************************************************************
    class: IBindableTexture

    author: S. Hau
    date: August 29, 2018
**********************************************************************/

#include "enums.hpp"
#include "Vulkan/Vulkan.hpp"
#include "shader_resources.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class IBindableTexture
    {
    public:
        IBindableTexture() = default;
        virtual ~IBindableTexture();

    protected:
        //----------------------------------------------------------------------
        VkSampler   getSampler()        const { return m_sampler; }
        bool        keepPixelsInRAM()   const { return m_keepPixelsInRAM; }

        void        setGenerateMips(bool generateMips) { m_generateMips = generateMips; }

        //----------------------------------------------------------------------
        // This functions binds this texture to the given set + binding.
        // Default implementation is provided, but can be overriden if desired.
        //----------------------------------------------------------------------
        virtual void bind(const ShaderResourceDeclaration& res);

        //----------------------------------------------------------------------
        // Apply changes. Next time this texture will be binded, pixel data will
        // be uploaded first and/or mips generated.
        //----------------------------------------------------------------------
        void apply(bool updateMips, bool keepPixelsInRAM);

        struct
        {
            VkImage     img = VK_NULL_HANDLE;
            VkImageView view = VK_NULL_HANDLE;
        } m_image;

        //----------------------------------------------------------------------
        void _RecreateSampler(U32 anisoLevel, TextureFilter filter, TextureAddressMode addressMode) { _DestroySampler(); _CreateSampler(anisoLevel, filter, addressMode); }
        void _CreateSampler(U32 anisoLevel, TextureFilter filter, TextureAddressMode addressMode);
        void _DestroySampler();

    private:
        bool m_keepPixelsInRAM  = false;
        bool m_gpuUpToDate      = true;
        bool m_generateMips     = true;

        VkSampler m_sampler = VK_NULL_HANDLE;

        //----------------------------------------------------------------------
        // Pushes the pixel data to the GPU before binding if gpu is not up to date.
        //----------------------------------------------------------------------
        virtual void _PushToGPU() = 0;

        //----------------------------------------------------------------------
        // Generate mips for whole texture
        //----------------------------------------------------------------------
        virtual void _GenerateMips() = 0;

        NULL_COPY_AND_ASSIGN(IBindableTexture)
    };

} } // End namespaces