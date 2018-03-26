#pragma once
/**********************************************************************
    class: Texture (D3D11Texture.h)

    author: S. Hau
    date: March 24, 2018
**********************************************************************/

#include "i_texture.h"
#include "../D3D11.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Texture : public ITexture
    {
    public:
        Texture(U32 width, U32 height, TextureFormat format, bool generateMips);
        ~Texture();

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void apply() override;

        void bind(U32 slot);

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
        Texture(const Texture& other)               = delete;
        Texture& operator = (const Texture& other)  = delete;
        Texture(Texture&& other)                    = delete;
        Texture& operator = (Texture&& other)       = delete;
    };

} } // End namespaces