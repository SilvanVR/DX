#include "D3D11Texture.h"
/**********************************************************************
    class: D3D11Texture (D3D11Texture.cpp)

    author: S. Hau
    date: March 24, 2018
**********************************************************************/

#include "../D3D11Utility.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Texture::Texture( U32 width, U32 height, TextureFormat format, bool generateMips )
        : ITexture( width, height, format, generateMips )
    {
        ASSERT( m_width > 0 && m_height > 0 );
        _CreateTexture();
        _CreateSampler();
    }

    //----------------------------------------------------------------------
    Texture::~Texture()
    {
        SAFE_RELEASE( m_pSampleState );
        SAFE_RELEASE( m_pTexture );
        SAFE_RELEASE( m_pTextureView );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Texture::bind( U32 slot )
    {
        if ( not m_gpuUpToDate )
        {
            _PushToGPU();
            m_gpuUpToDate = true;
        }

        if ( m_generateMips )
            g_pImmediateContext->GenerateMips( m_pTextureView );

        g_pImmediateContext->PSSetSamplers( slot, 1, &m_pSampleState );
        g_pImmediateContext->PSSetShaderResources( slot, 1, &m_pTextureView );
    }

    //----------------------------------------------------------------------
    void Texture::apply()
    {
        m_gpuUpToDate = false;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Texture::_CreateTexture()
    {
        SAFE_RELEASE( m_pTexture );
        SAFE_RELEASE( m_pTextureView );

        // Setup the description of the texture
        D3D11_TEXTURE2D_DESC textureDesc;
        textureDesc.Height              = getHeight();
        textureDesc.Width               = getWidth();
        textureDesc.MipLevels           = m_generateMips ? 0 : 1;
        textureDesc.ArraySize           = 1;
        textureDesc.Format              = Utility::TranslateTextureFormat( m_format );
        textureDesc.SampleDesc.Count    = 1;
        textureDesc.SampleDesc.Quality  = 0;
        textureDesc.Usage               = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags           = m_generateMips ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags      = 0;
        textureDesc.MiscFlags           = m_generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

        // Create texture
        HR( g_pDevice->CreateTexture2D( &textureDesc, NULL, &m_pTexture ) );

        // Setup the shader resource view description
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        // Create the shader resource view for the texture
        HR( g_pDevice->CreateShaderResourceView( m_pTexture, &srvDesc, &m_pTextureView ) );
    }

    //----------------------------------------------------------------------
    void Texture::_CreateSampler()
    {
        SAFE_RELEASE( m_pSampleState );

        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = (m_anisoLevel > 1 ? D3D11_FILTER_ANISOTROPIC : Utility::TranslateFilter( m_filter ) );

        auto clampMode = Utility::TranslateClampMode( m_clampMode );
        samplerDesc.AddressU        = clampMode;
        samplerDesc.AddressV        = clampMode;
        samplerDesc.AddressW        = clampMode;
        samplerDesc.MipLODBias      = 0.0f;
        samplerDesc.MaxAnisotropy   = m_anisoLevel;
        samplerDesc.ComparisonFunc  = D3D11_COMPARISON_NEVER;
        samplerDesc.BorderColor[0]  = 0.0f;
        samplerDesc.BorderColor[1]  = 0.0f;
        samplerDesc.BorderColor[2]  = 0.0f;
        samplerDesc.BorderColor[3]  = 0.0f;
        samplerDesc.MinLOD          = 0;
        samplerDesc.MaxLOD          = D3D11_FLOAT32_MAX;

        HR( g_pDevice->CreateSamplerState( &samplerDesc, &m_pSampleState ) );
    }

    //----------------------------------------------------------------------
    void Texture::_PushToGPU()
    {
        ASSERT( m_pixels != nullptr );

        // Copy the data into the texture
        unsigned int rowPitch = (getWidth() * 4) * sizeof(unsigned char);
        g_pImmediateContext->UpdateSubresource( m_pTexture, 0, NULL, m_pixels, rowPitch, 0 );

        SAFE_DELETE( m_pixels );
    }

} } // End namespaces