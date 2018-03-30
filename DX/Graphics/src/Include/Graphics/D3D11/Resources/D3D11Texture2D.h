#pragma once
/**********************************************************************
    class: Texture (D3D11Texture.h)

    author: S. Hau
    date: March 24, 2018
**********************************************************************/

#include "i_texture2d.hpp"
#include "../D3D11.hpp"
#include "D3D11Texture.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Texture2D : public ITexture2D, public D3D11Texture
    {
    public:
        Texture2D(U32 width, U32 height, TextureFormat format, bool generateMips);
        ~Texture2D();

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void apply() override;

        void bind(U32 slot) override;

    private:
        ID3D11Texture2D*            m_pTexture       = nullptr;
        ID3D11ShaderResourceView*   m_pTextureView   = nullptr;
        ID3D11SamplerState*         m_pSampleState   = nullptr;

        bool m_gpuUpToDate = true;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { _CreateSampler(); }

        //----------------------------------------------------------------------
        void _CreateSampler();
        void _CreateTexture();
        void _PushToGPU();

        //----------------------------------------------------------------------
        Texture2D(const Texture2D& other)               = delete;
        Texture2D& operator = (const Texture2D& other)  = delete;
        Texture2D(Texture2D&& other)                    = delete;
        Texture2D& operator = (Texture2D&& other)       = delete;
    };

} } // End namespaces