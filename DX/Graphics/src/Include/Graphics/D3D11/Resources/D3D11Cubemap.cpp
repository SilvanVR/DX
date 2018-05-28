#include "D3D11Cubemap.h"
/**********************************************************************
    class: Cubemap (D3D11Cubemap.cpp)

    author: S. Hau
    date: April 1, 2018
**********************************************************************/

#include "Utils/utils.h"

#define NUM_FACES 6

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Cubemap::~Cubemap()
    {
        SAFE_RELEASE( m_pTexture );
        SAFE_RELEASE( m_pTextureView );
    }

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
            _UpdateMipCount();

        // Reserve mem for faces
        Size bytesPerFace = size * size * ByteCountFromTextureFormat( m_format );
        for (U32 face = 0; face < NUM_FACES; face++)
            m_facePixels[face].resize( bytesPerFace );

        // Create D3D11 Resources
        _CreateTexture( mips );
        _CreateShaderResourceView();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //----------------------------------------------------------------------
    void Cubemap::apply( bool updateMips, bool keepPixelsInRAM )
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
    void Cubemap::_CreateTexture( Mips mips )
    {
        bool createMips = (mips != Mips::None);

        D3D11_TEXTURE2D_DESC texDesc;
        texDesc.Height              = getHeight();
        texDesc.Width               = getWidth();
        texDesc.MipLevels           = createMips ? 0 : 1;
        texDesc.ArraySize           = 6;
        texDesc.Format              = Utility::TranslateTextureFormat( m_format );
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
    void Cubemap::_CreateShaderResourceView()
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format                      = Utility::TranslateTextureFormat( m_format );
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