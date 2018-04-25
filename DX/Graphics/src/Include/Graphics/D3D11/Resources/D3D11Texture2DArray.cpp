#include "D3D11Texture2DArray.h"
/**********************************************************************
    class: Texture2DArray (D3D11Texture2DArray.cpp)

    author: S. Hau
    date: April 3, 2018
**********************************************************************/

#include "../D3D11Utility.h"
#include "Utils/utils.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Texture2DArray::~Texture2DArray()
    {
        SAFE_RELEASE( m_pTexture );
        SAFE_RELEASE( m_pTextureView );
    }

    //----------------------------------------------------------------------
    void Texture2DArray::create( U32 width, U32 height, U32 depth, TextureFormat format, bool generateMips )
    {
        ASSERT( width > 0 && height > 0 && m_width == 0 && "Invalid params or texture were already created" );
        ITexture::_Init( width, height, format );

        m_depth = depth;
        m_generateMips = generateMips;
        if (m_generateMips)
            _UpdateMipCount();

        m_pixels.resize( depth );
        for (auto& slice : m_pixels)
            slice.resize( m_width * m_height * ByteCountFromTextureFormat( format ) );

        _CreateTextureArray();
        _CreateShaderResourveView();
        _CreateSampler( m_anisoLevel, m_filter, m_clampMode );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Texture2DArray::apply( bool updateMips, bool keepPixelsInRAM )
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
    void Texture2DArray::_CreateTextureArray()
    {
        D3D11_TEXTURE2D_DESC texDesc;
        texDesc.Height              = getHeight();
        texDesc.Width               = getWidth();
        texDesc.MipLevels           = m_generateMips ? 0 : 1;
        texDesc.ArraySize           = m_depth;
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
    void Texture2DArray::_CreateShaderResourveView()
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = Utility::TranslateTextureFormat( m_format );
        srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MostDetailedMip  = 0;
        srvDesc.Texture2DArray.MipLevels        = -1;
        srvDesc.Texture2DArray.FirstArraySlice  = 0;
        srvDesc.Texture2DArray.ArraySize        = m_depth;

        HR( g_pDevice->CreateShaderResourceView( m_pTexture, &srvDesc, &m_pTextureView ) );
    }

    //----------------------------------------------------------------------
    void Texture2DArray::_PushToGPU()
    {
        ASSERT( not m_pixels.empty() );

        // Upload data to gpu
        U32 rowPitch = ( getWidth() * ByteCountFromTextureFormat( m_format ) );
        for ( U32 slice = 0; slice < m_pixels.size(); slice++ )
        {
            U32 sliceLevel = D3D11CalcSubresource( 0, slice, m_mipCount );
            g_pImmediateContext->UpdateSubresource( m_pTexture, sliceLevel, NULL, m_pixels[slice].data(), rowPitch, 0 );
        }

        if ( not m_keepPixelsInRAM )
            m_pixels.clear();
    }

} } // End namespaces