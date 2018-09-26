#include "D3D11Cubemap.h"
/**********************************************************************
    class: Cubemap (D3D11Cubemap.cpp)

    author: S. Hau
    date: April 1, 2018
**********************************************************************/

#include "Utils/utils.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Cubemap::create( I32 size, TextureFormat format, Mips mips )
    {
        ASSERT( size > 0 );
        ITexture::_Init( TextureDimension::Cube, size, size, format );

        m_generateMips = (mips == Mips::Generate);
        if (mips == Mips::Generate || mips == Mips::Create)
        {
            _UpdateMipCount();
            m_hasMips = true;
        }

        bool isDepthBuffer = IsDepthFormat( m_format );
        _CreateTexture( mips, isDepthBuffer );
        _CreateShaderResourceView( isDepthBuffer );
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Cubemap::_CreateTexture( Mips mips, bool isDepthBuffer )
    {
        bool createMips = (mips != Mips::None);

        D3D11_TEXTURE2D_DESC texDesc;
        texDesc.Height              = getHeight();
        texDesc.Width               = getWidth();
        texDesc.MipLevels           = createMips ? 0 : 1;
        texDesc.ArraySize           = 6;
        texDesc.Format              = isDepthBuffer ? Utility::TranslateDepthFormatBindable( m_format ) : Utility::TranslateTextureFormat( m_format );
        texDesc.SampleDesc.Count    = 1;
        texDesc.SampleDesc.Quality  = 0;
        texDesc.Usage               = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags           = m_generateMips ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags      = 0;
        texDesc.MiscFlags           = D3D11_RESOURCE_MISC_TEXTURECUBE;
        if (m_generateMips)
            texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

        // Create texture
        HR( g_pDevice->CreateTexture2D( &texDesc, NULL, &m_pTexture ) );
    }

    //----------------------------------------------------------------------
    void Cubemap::_CreateShaderResourceView( bool isDepthBuffer )
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format                      = isDepthBuffer ? Utility::TranslateDepthFormatSRV( m_format ) : Utility::TranslateTextureFormat( m_format );
        srvDesc.ViewDimension               = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels       = -1;
        srvDesc.TextureCube.MostDetailedMip = 0;

        HR( g_pDevice->CreateShaderResourceView( m_pTexture, &srvDesc, &m_pTextureView ) );
    }

    //----------------------------------------------------------------------
    void Cubemap::_PushToGPU()
    {
        // Copy the data into the texture
        U32 rowPitch = ( getWidth() * ByteCountFromTextureFormat( m_format ) );
        for (U32 face = 0; face < NUM_FACES; face++)
        {
            // Upload data to gpu
            U32 faceLevel = D3D11CalcSubresource( 0, face, m_mipCount );
            g_pImmediateContext->UpdateSubresource( m_pTexture, faceLevel, NULL, m_facePixels[(I32)face].data(), rowPitch, 0 );

            // Free mem in RAM if desired
            if ( not m_keepPixelsInRAM )
                m_facePixels[(I32)face].clear();
        }
    }

} } // End namespaces