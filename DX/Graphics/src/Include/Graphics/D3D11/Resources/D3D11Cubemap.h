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
        void setPixel(CubemapFace face, I32 x, I32 y, Color color) override;
        void setPixels(CubemapFace face, const void* pPixels) override;
        void apply(bool updateMips = true) override;

        //----------------------------------------------------------------------
        // IBindableTexture Interface
        //----------------------------------------------------------------------
        void bind(U32 slot) override;

    private:
        ID3D11Texture2D*            m_pTexture       = nullptr;
        ID3D11ShaderResourceView*   m_pTextureView   = nullptr;

        bool m_gpuUpToDate  = true;
        bool m_generateMips = false;


        // Heap allocated mem for each face. How large it is depends on width/height and the format
        void* m_pixels[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { SAFE_RELEASE( m_pSampleState ); _CreateSampler( m_anisoLevel, m_filter, m_clampMode ); }


        void _PushToGPU();

        //----------------------------------------------------------------------
        Cubemap(const Cubemap& other)               = delete;
        Cubemap& operator = (const Cubemap& other)  = delete;
        Cubemap(Cubemap&& other)                    = delete;
        Cubemap& operator = (Cubemap&& other)       = delete;
    };

} } // End namespaces