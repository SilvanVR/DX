#pragma once
/**********************************************************************
    class: Cubemap (D3D11Cubemap.h)

    author: S. Hau
    date: April 1, 2018
**********************************************************************/

#include "i_cubemap.hpp"
#include "../D3D11.hpp"
#include "D3D11IBindableTexture.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Cubemap : public Graphics::ICubemap, public D3D11::IBindableTexture
    {
    public:
        Cubemap() = default;
        ~Cubemap();

        //----------------------------------------------------------------------
        // ICubemap Interface
        //----------------------------------------------------------------------
        void create(I32 size, TextureFormat format, bool generateMips) override;
        void apply(bool updateMips, bool keepPixelsInRAM) override;

        //----------------------------------------------------------------------
        // IBindableTexture Interface
        //----------------------------------------------------------------------
        void bind(U32 slot) override;

    private:
        ID3D11Texture2D*            m_pTexture       = nullptr;
        ID3D11ShaderResourceView*   m_pTextureView   = nullptr;

        bool m_gpuUpToDate      = true;
        bool m_generateMips     = false;
        bool m_keepPixelsInRAM  = false;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { SAFE_RELEASE( m_pSampleState ); _CreateSampler( m_anisoLevel, m_filter, m_clampMode ); }

        //----------------------------------------------------------------------
        void _CreateTexture();
        void _CreateShaderResourceView();
        void _PushToGPU();

        //----------------------------------------------------------------------
        Cubemap(const Cubemap& other)               = delete;
        Cubemap& operator = (const Cubemap& other)  = delete;
        Cubemap(Cubemap&& other)                    = delete;
        Cubemap& operator = (Cubemap&& other)       = delete;
    };

} } // End namespaces