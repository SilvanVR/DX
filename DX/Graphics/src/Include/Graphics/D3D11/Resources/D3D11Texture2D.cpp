#include "D3D11Texture2D.h"
/**********************************************************************
    class: Texture2D (D3D11Texture2D.cpp)

    author: S. Hau
    date: March 24, 2018
**********************************************************************/

#include "../D3D11Utility.h"
#include "Utils/utils.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Texture2D::~Texture2D()
    {
        SAFE_RELEASE( m_pTexture );
        SAFE_RELEASE( m_pTextureView );
    }

    //----------------------------------------------------------------------
    void Texture2D::create( U32 width, U32 height, TextureFormat format, bool generateMips )
    {
        ASSERT( width > 0 && height > 0 && m_width == 0 && "Invalid params or texture were already created" );
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );

        m_isImmutable = false;
        m_generateMips = generateMips;
        if (m_generateMips)
            _UpdateMipCount();
        m_pixels.resize( m_width * m_height * ByteCountFromTextureFormat( format ) );

        _CreateTexture();
        _CreateShaderResourveView();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //----------------------------------------------------------------------
    void Texture2D::create( U32 width, U32 height, TextureFormat format, const void* pData )
    {
        ASSERT( width > 0 && height > 0 && pData != nullptr && m_width == 0 && "Invalid params or texture were already created" );
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );

        m_generateMips = false;
        m_isImmutable = true;
        _CreateTexture( pData );
        _CreateShaderResourveView();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Texture2D::apply( bool updateMips, bool keepPixelsInRAM )
    { 
        m_keepPixelsInRAM = keepPixelsInRAM;
        m_gpuUpToDate = false; 
        if (m_mipCount > 1)
            m_generateMips = updateMips;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Texture2D::_CreateTexture()
    {
        D3D11_TEXTURE2D_DESC texDesc;
        texDesc.Height              = getHeight();
        texDesc.Width               = getWidth();
        texDesc.MipLevels           = m_generateMips ? 0 : 1;
        texDesc.ArraySize           = 1;
        texDesc.Format              = Utility::TranslateTextureFormat( m_format );
        texDesc.SampleDesc.Count    = 1;
        texDesc.SampleDesc.Quality  = 0;
        texDesc.Usage               = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags           = m_generateMips ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags      = 0;
        texDesc.MiscFlags           = m_generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

        HR( g_pDevice->CreateTexture2D( &texDesc, NULL, &m_pTexture ) );
    }

    //----------------------------------------------------------------------
    void Texture2D::_CreateTexture( const void* pData )
    {
        D3D11_TEXTURE2D_DESC texDesc;
        texDesc.Height              = getHeight();
        texDesc.Width               = getWidth();
        texDesc.MipLevels           = 1;
        texDesc.ArraySize           = 1;
        texDesc.Format              = Utility::TranslateTextureFormat( m_format );
        texDesc.SampleDesc.Count    = 1;
        texDesc.SampleDesc.Quality  = 0;
        texDesc.Usage               = D3D11_USAGE_IMMUTABLE;
        texDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags      = 0;
        texDesc.MiscFlags           = 0;
        
        D3D11_SUBRESOURCE_DATA subResourceData = {};
        subResourceData.pSysMem = pData;
        subResourceData.SysMemPitch = m_width * ByteCountFromTextureFormat( m_format ) ;
        HR( g_pDevice->CreateTexture2D( &texDesc, &subResourceData , &m_pTexture ) );
    }

    //----------------------------------------------------------------------
    void Texture2D::_CreateShaderResourveView()
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = Utility::TranslateTextureFormat( m_format );
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        HR( g_pDevice->CreateShaderResourceView( m_pTexture, &srvDesc, &m_pTextureView ) );
    }

    //----------------------------------------------------------------------
    void Texture2D::_PushToGPU()
    {
        ASSERT( not m_pixels.empty() );

        // Copy the data into the texture
        U32 rowPitch = ( getWidth() * ByteCountFromTextureFormat( m_format ) );
        g_pImmediateContext->UpdateSubresource( m_pTexture, 0, NULL, m_pixels.data(), rowPitch, 0 );

        if ( not m_keepPixelsInRAM )
            m_pixels.clear();
    }

} } // End namespaces