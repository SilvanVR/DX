#include "VkCubemap.h"
/**********************************************************************
    class: Cubemap

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "Utils/utils.h"

namespace Graphics { namespace Vulkan {

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

        // Create D3D11 Resources
        _CreateTexture( mips );
        _CreateShaderResourceView();
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

    }

    //----------------------------------------------------------------------
    void Cubemap::_CreateShaderResourceView()
    {

    }

    //----------------------------------------------------------------------
    void Cubemap::_PushToGPU()
    {
        // Copy the data into the texture
        U32 rowPitch = ( getWidth() * ByteCountFromTextureFormat( m_format ) );
        for (U32 face = 0; face < NUM_FACES; face++)
        {
            // Upload data to gpu
            //U32 faceLevel = D3D11CalcSubresource( 0, face, m_mipCount );
            //g_pImmediateContext->UpdateSubresource( m_pTexture, faceLevel, NULL, m_facePixels[(I32)face].data(), rowPitch, 0 );

            // Free mem in RAM if desired
            if ( not m_keepPixelsInRAM )
                m_facePixels[(I32)face].clear();
        }
    }

} } // End namespaces