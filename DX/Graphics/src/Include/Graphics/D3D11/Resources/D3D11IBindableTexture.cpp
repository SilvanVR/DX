#include "D3D11IBindableTexture.h"
/**********************************************************************
    class: IBindableTexture (D3D11IBindableTexture.cpp)

    author: S. Hau
    date: March 30, 2018
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    IBindableTexture::~IBindableTexture()
    { 
        SAFE_RELEASE( m_pSampleState );
        SAFE_RELEASE( m_pTexture );
        SAFE_RELEASE( m_pTextureView );
    }

    //**********************************************************************
    // PROTECTED
    //**********************************************************************

    //----------------------------------------------------------------------
    void IBindableTexture::bind( const ShaderResourceDeclaration& res )
    {
        if (not m_gpuUpToDate)
        {
            _PushToGPU();
            m_gpuUpToDate = true;
        }

        if (m_generateMips)
        {
            g_pImmediateContext->GenerateMips( m_pTextureView );
            m_generateMips = false;
        }

        if ((I32)(res.getShaderStages() & ShaderType::Vertex))
        {
            g_pImmediateContext->VSSetSamplers( res.getBindingSlot(), 1, &m_pSampleState );
            g_pImmediateContext->VSSetShaderResources( res.getBindingSlot(), 1, &m_pTextureView );
        }
        if ((I32)(res.getShaderStages() & ShaderType::Fragment))
        {
            g_pImmediateContext->PSSetSamplers( res.getBindingSlot(), 1, &m_pSampleState );
            g_pImmediateContext->PSSetShaderResources( res.getBindingSlot(), 1, &m_pTextureView );
        }
    }

    //----------------------------------------------------------------------
    void IBindableTexture::apply( bool updateMips, bool keepPixelsInRAM )
    {
        m_keepPixelsInRAM = keepPixelsInRAM;
        m_gpuUpToDate = false;
        m_generateMips = updateMips;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void IBindableTexture::_CreateSampler( U32 anisoLevel, TextureFilter filter, TextureAddressMode addressMode )
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

} } // End namespaces