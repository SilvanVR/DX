#include "D3D11Texture2D.h"
/**********************************************************************
    class: D3D11Texture (D3D11Texture.cpp)

    author: S. Hau
    date: March 24, 2018
**********************************************************************/

#include "../D3D11Utility.h"
#include "Utils/utils.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Texture2D::~Texture2D()
    {
        SAFE_DELETE( m_pixels );
        SAFE_RELEASE( m_pSampleState );
        SAFE_RELEASE( m_pTexture );
        SAFE_RELEASE( m_pTextureView );
    }

    //----------------------------------------------------------------------
    void Texture2D::create( U32 width, U32 height, TextureFormat format, bool generateMips )
    {
        ASSERT( width > 0 && height > 0 );
        ITexture::_Init( width, height, format );

        m_isImmutable = false;
        if (m_generateMips)
            _CalculateMipCount();
        m_pixels = new Byte[m_width * m_height * ByteCountFromTextureFormat( format )];

        _CreateTexture();
        _CreateShaderResourveView();
        _CreateSampler();
    }

    //----------------------------------------------------------------------
    void Texture2D::create( U32 width, U32 height, TextureFormat format, const void* pData )
    {
        ASSERT( width > 0 && height > 0 && pData != nullptr );
        ITexture::_Init( width, height, format );

        m_isImmutable = true;
        _CreateTexture( pData );
        _CreateShaderResourveView();
        _CreateSampler();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Texture2D::bind( U32 slot )
    {
        if ( not m_gpuUpToDate )
        {
            _PushToGPU();
            m_gpuUpToDate = true;
        }

        if ( m_generateMips )
        {
            g_pImmediateContext->GenerateMips( m_pTextureView );
            m_generateMips = false;
        }

        g_pImmediateContext->PSSetSamplers( slot, 1, &m_pSampleState );
        g_pImmediateContext->PSSetShaderResources( slot, 1, &m_pTextureView );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

        //----------------------------------------------------------------------
    void Texture2D::_CreateTexture()
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
    }

    //----------------------------------------------------------------------
    void Texture2D::_CreateTexture( const void* pData )
    {
        SAFE_RELEASE( m_pTexture );
        SAFE_RELEASE( m_pTextureView );

        // Setup the description of the texture
        D3D11_TEXTURE2D_DESC textureDesc;
        textureDesc.Height              = getHeight();
        textureDesc.Width               = getWidth();
        textureDesc.MipLevels           = 1;
        textureDesc.ArraySize           = 1;
        textureDesc.Format              = Utility::TranslateTextureFormat( m_format );
        textureDesc.SampleDesc.Count    = 1;
        textureDesc.SampleDesc.Quality  = 0;
        textureDesc.Usage               = D3D11_USAGE_IMMUTABLE;
        textureDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags      = 0;
        textureDesc.MiscFlags           = 0;
        
        D3D11_SUBRESOURCE_DATA subResourceData = {};
        subResourceData.pSysMem = pData;
        subResourceData.SysMemPitch = m_width * ByteCountFromTextureFormat( m_format ) ;
        HR( g_pDevice->CreateTexture2D( &textureDesc, &subResourceData , &m_pTexture ) );
    }

    //----------------------------------------------------------------------
    void Texture2D::_CreateShaderResourveView()
    {
        // Setup the shader resource view description
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = Utility::TranslateTextureFormat( m_format );
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        // Create the shader resource view for the texture
        HR( g_pDevice->CreateShaderResourceView( m_pTexture, &srvDesc, &m_pTextureView ) );
    }

    //----------------------------------------------------------------------
    void Texture2D::_CreateSampler()
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
    void Texture2D::_PushToGPU()
    {
        ASSERT( m_pixels != nullptr );

        // Copy the data into the texture
        unsigned int rowPitch = (getWidth() * sizeof( Color ));
        g_pImmediateContext->UpdateSubresource( m_pTexture, 0, NULL, m_pixels, rowPitch, 0 );
    }

} } // End namespaces