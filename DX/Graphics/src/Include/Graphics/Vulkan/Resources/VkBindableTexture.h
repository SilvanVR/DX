#pragma once
/**********************************************************************
    class: IBindableTexture

    author: S. Hau
    date: August 29, 2018
**********************************************************************/

#include "enums.hpp"
#include "Vulkan/Vulkan.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class IBindableTexture
    {
    public:
        IBindableTexture() = default;
        virtual ~IBindableTexture();

        //----------------------------------------------------------------------
        // This functions binds this texture to the given shader at the given set + binding.
        // Default implementation is provided, but can be overriden if desired.
        //----------------------------------------------------------------------
        virtual void bind(U32 set, U32 binding);

    protected:
        bool m_gpuUpToDate      = true;
        bool m_generateMips     = true;
        bool m_keepPixelsInRAM  = false;

        VkSampler m_sampler = VK_NULL_HANDLE;

        //----------------------------------------------------------------------
        void _RecreateSampler(U32 anisoLevel, TextureFilter filter, TextureAddressMode addressMode) { _DestroySampler(); _CreateSampler(anisoLevel, filter, addressMode); }
        void _CreateSampler(U32 anisoLevel, TextureFilter filter, TextureAddressMode addressMode);
        void _DestroySampler();

        //----------------------------------------------------------------------
        // Pushes the pixel data to the GPU before binding if gpu is not up to date.
        //----------------------------------------------------------------------
        virtual void _PushToGPU() {}

        NULL_COPY_AND_ASSIGN(IBindableTexture)
    };

} } // End namespaces