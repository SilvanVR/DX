#pragma once
/**********************************************************************
    class: Cubemap (D3D11Cubemap.h)

    author: S. Hau
    date: April 1, 2018
**********************************************************************/

#include "i_cubemap.hpp"
#include "../D3D11.hpp"
#include "D3D11IBindableTexture.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Cubemap : public Graphics::ICubemap, public D3D11::IBindableTexture
    {
    public:
        Cubemap() = default;
        ~Cubemap() = default;

        //----------------------------------------------------------------------
        // ICubemap Interface
        //----------------------------------------------------------------------
        void create(I32 size, TextureFormat format, Mips mips) override;
        void apply(bool updateMips, bool keepPixelsInRAM) override { IBindableTexture::apply(updateMips, keepPixelsInRAM); }

    private:
        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { SAFE_RELEASE( m_pSampleState ); _CreateSampler( m_anisoLevel, m_filter, m_clampMode ); }
        void bind(const ShaderResourceDeclaration& res) override { IBindableTexture::bind(res); }
        U64* getNativeTexturePtr() const override { return reinterpret_cast<U64*>(m_pTexture); }

        //----------------------------------------------------------------------
        void _CreateTexture(Mips mips);
        void _CreateShaderResourceView();

        //----------------------------------------------------------------------
        // IBindableTexture Interface
        //----------------------------------------------------------------------
        void _PushToGPU() override;

        //----------------------------------------------------------------------
        Cubemap(const Cubemap& other)               = delete;
        Cubemap& operator = (const Cubemap& other)  = delete;
        Cubemap(Cubemap&& other)                    = delete;
        Cubemap& operator = (Cubemap&& other)       = delete;
    };

} } // End namespaces