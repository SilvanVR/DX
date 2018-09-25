#pragma once
/**********************************************************************
    class: Texture2D (D3D11Texture2D.h)

    author: S. Hau
    date: March 24, 2018
**********************************************************************/

#include "i_texture2d.hpp"
#include "../D3D11.hpp"
#include "D3D11IBindableTexture.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Texture2D : public Graphics::ITexture2D, public D3D11::IBindableTexture
    {
    public:
        Texture2D() = default;
        ~Texture2D() = default;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void create(U32 width, U32 height, TextureFormat format, bool generateMips) override;
        void create(U32 width, U32 height, TextureFormat format, const void* pData) override;
        void apply(bool updateMips, bool keepPixelsInRAM) override { IBindableTexture::apply(updateMips, keepPixelsInRAM); }
        U64* getNativeTexturePtr() const override { return reinterpret_cast<U64*>(m_pTexture); }

    private:
        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { SAFE_RELEASE( m_pSampleState ); _CreateSampler( m_anisoLevel, m_filter, m_clampMode ); }
        void bind(const ShaderResourceDeclaration& res) override { IBindableTexture::bind(res); }

        //----------------------------------------------------------------------
        void _CreateTexture();
        void _CreateTexture(const void* pData);
        void _CreateShaderResourveView();

        //----------------------------------------------------------------------
        // IBindableTexture Interface
        //----------------------------------------------------------------------
        void _PushToGPU() override;

        //----------------------------------------------------------------------
        Texture2D(const Texture2D& other)               = delete;
        Texture2D& operator = (const Texture2D& other)  = delete;
        Texture2D(Texture2D&& other)                    = delete;
        Texture2D& operator = (Texture2D&& other)       = delete;
    };

} } // End namespaces