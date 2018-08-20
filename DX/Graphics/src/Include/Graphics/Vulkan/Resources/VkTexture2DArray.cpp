#include "VkTexture2DArray.h"
/**********************************************************************
    class: Texture2DArray

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "Utils/utils.h"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    void Texture2DArray::create( U32 width, U32 height, U32 depth, TextureFormat format, bool generateMips )
    {
        ASSERT( width > 0 && height > 0 && m_width == 0 && "Invalid params or texture were already created" );
        ITexture::_Init( TextureDimension::Tex2DArray, width, height, format );

        m_depth = depth;
        m_generateMips = generateMips;
        if (m_generateMips)
            _UpdateMipCount();

        _CreateTextureArray();
        _CreateShaderResourveView();
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

    }

    //----------------------------------------------------------------------
    void Texture2DArray::_CreateShaderResourveView()
    {

    }

    //----------------------------------------------------------------------
    void Texture2DArray::_PushToGPU()
    {
        ASSERT( not m_pixels.empty() );

        // Upload data to gpu
        U32 rowPitch = ( getWidth() * ByteCountFromTextureFormat( m_format ) );
        for ( U32 slice = 0; slice < m_pixels.size(); slice++ )
        {
            //U32 sliceLevel = D3D11CalcSubresource( 0, slice, m_mipCount );
            //g_pImmediateContext->UpdateSubresource( m_pTexture, sliceLevel, NULL, m_pixels[slice].data(), rowPitch, 0 );
        }

        if ( not m_keepPixelsInRAM )
            m_pixels.clear();
    }

} } // End namespaces