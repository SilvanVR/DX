#pragma once
/**********************************************************************
    class: IBindableTexture (D3D11IBindableTexture.hpp)

    author: S. Hau
    date: March 30, 2018

    Interface for all textures in d3d11, which can be bound to a shader.
**********************************************************************/

#include "enums.hpp"
#include "../D3D11Utility.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class IBindableTexture
    {
    public:
        IBindableTexture() = default;
        virtual ~IBindableTexture() { SAFE_RELEASE( m_pSampleState ); }

        //----------------------------------------------------------------------
        virtual void bind(U32 slot) = 0;

        // Virtual because the render texture has more than one texture
        virtual ID3D11Texture2D* getD3D11Texture() { return m_pTexture; }

    protected:
        ID3D11SamplerState*         m_pSampleState      = nullptr;
        ID3D11Texture2D*            m_pTexture          = nullptr;
        ID3D11ShaderResourceView*   m_pTextureView      = nullptr;

        bool                        m_gpuUpToDate      = true;
        bool                        m_generateMips     = true;
        bool                        m_keepPixelsInRAM  = false;

        //----------------------------------------------------------------------
        void _CreateSampler( U32 anisoLevel, TextureFilter filter, TextureAddressMode addressMode )
        {
            SAFE_RELEASE( m_pSampleState );

            D3D11_SAMPLER_DESC samplerDesc;
            samplerDesc.Filter = (anisoLevel > 1 ? D3D11_FILTER_ANISOTROPIC : Utility::TranslateFilter( filter ) );

            auto clampMode = Utility::TranslateClampMode( addressMode );
            samplerDesc.AddressU        = clampMode;
            samplerDesc.AddressV        = clampMode;
            samplerDesc.AddressW        = clampMode;
            samplerDesc.MipLODBias      = 0.0f;
            samplerDesc.MaxAnisotropy   = anisoLevel;
            samplerDesc.ComparisonFunc  = D3D11_COMPARISON_NEVER;
            samplerDesc.BorderColor[0]  = 0.0f;
            samplerDesc.BorderColor[1]  = 0.0f;
            samplerDesc.BorderColor[2]  = 0.0f;
            samplerDesc.BorderColor[3]  = 0.0f;
            samplerDesc.MinLOD          = 0;
            samplerDesc.MaxLOD          = D3D11_FLOAT32_MAX;

            HR( g_pDevice->CreateSamplerState( &samplerDesc, &m_pSampleState ) );
        }

    private:
        //----------------------------------------------------------------------
        IBindableTexture(const IBindableTexture& other)               = delete;
        IBindableTexture& operator = (const IBindableTexture& other)  = delete;
        IBindableTexture(IBindableTexture&& other)                    = delete;
        IBindableTexture& operator = (IBindableTexture&& other)       = delete;
    };

} } // End namespaces