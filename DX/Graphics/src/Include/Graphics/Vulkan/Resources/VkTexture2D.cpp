#include "VkTexture2D.h"
/**********************************************************************
    class: Texture2D

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "Utils/utils.h"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    void Texture2D::create( U32 width, U32 height, TextureFormat format, bool generateMips )
    {
        ASSERT( width > 0 && height > 0 && m_width == 0 && "Invalid params or texture were already created" );
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );

        m_isImmutable = false;
        m_generateMips = generateMips;
        if (m_generateMips)
            _UpdateMipCount();

        _CreateTexture();
    }

    //----------------------------------------------------------------------
    void Texture2D::create( U32 width, U32 height, TextureFormat format, const void* pData )
    {
        ASSERT( width > 0 && height > 0 && pData != nullptr && m_width == 0 && "Invalid params or texture were already created" );
        ITexture::_Init( TextureDimension::Tex2D, width, height, format );

        m_generateMips = false;
        m_isImmutable = true;
        _CreateTexture( pData );
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

    }

    //----------------------------------------------------------------------
    void Texture2D::_CreateTexture( const void* pData )
    {

    }

    //----------------------------------------------------------------------
    void Texture2D::_PushToGPU()
    {
        ASSERT( not m_pixels.empty() );

        // Copy the data into the texture
        U32 rowPitch = ( getWidth() * ByteCountFromTextureFormat( m_format ) );
        //g_pImmediateContext->UpdateSubresource( m_pTexture, 0, NULL, m_pixels.data(), rowPitch, 0 );

        if ( not m_keepPixelsInRAM )
            m_pixels.clear();
    }

} } // End namespaces