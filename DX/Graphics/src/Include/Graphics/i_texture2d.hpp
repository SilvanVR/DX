#pragma once
/**********************************************************************
    class: Texture2D (i_texture2d.hpp)

    author: S. Hau
    date: March 30, 2018
**********************************************************************/

#include "i_texture.h"
#include "Utils/utils.h"

namespace Graphics
{

    //**********************************************************************
    class ITexture2D : public ITexture
    {
    public:
        ITexture2D() = default;
        virtual ~ITexture2D() { SAFE_DELETE( m_pixels ); }

        //----------------------------------------------------------------------
        // Creates a new 2d-texture.
        // @Params:
        //  "width": Width in pixels.
        //  "height": Height in pixels.
        //  "format": The texture format.
        //  "generateMips": If true a complete mipchain will be generated.
        //----------------------------------------------------------------------
        virtual void create(U32 width, U32 height, TextureFormat format, bool generateMips = true) = 0;

        //----------------------------------------------------------------------
        // Creates a new immutable 2d-texture. (P.S. This texture does not support generating mipmaps on the gpu)
        // @Params:
        //  "width": Width in pixels.
        //  "height": Height in pixels.
        //  "format": The texture format.
        //  "pData": Pointer to initializing data. 
        //----------------------------------------------------------------------
        virtual void create(U32 width, U32 height, TextureFormat format, const void* pData = nullptr) = 0;

        //----------------------------------------------------------------------
        // Apply all previous pixels changes to the texture.
        //----------------------------------------------------------------------
        virtual void apply() = 0;

        //----------------------------------------------------------------------
        // Generates mipmaps before the next frame
        //----------------------------------------------------------------------
        void generateMips() { m_generateMips = true; _CalculateMipCount(); }

        //----------------------------------------------------------------------
        // @Return:
        //  Whether the texture can be dynamically updated or is immutable.
        //----------------------------------------------------------------------
        bool isImmutable() const { return m_isImmutable; }

        //----------------------------------------------------------------------
        // Change one pixel. This function is only supported by the formats RGBA32 and BGRA32.
        //----------------------------------------------------------------------
        void setPixel( U32 x, U32 y, Color color ) 
        { 
            ASSERT( not isImmutable() && (m_format == TextureFormat::RGBA32 || m_format == TextureFormat::BGRA32) );
            ((Color*)m_pixels)[x + y * m_width] = color; 
        }

        //----------------------------------------------------------------------
        // Copies width * height * byteWidth from the given pointer.
        // @Params:
        //  "pPixels": Pointer to the pixel-data.
        //----------------------------------------------------------------------
        void setPixels( const void* pPixels ) 
        {
            ASSERT( not isImmutable() ); 
            Size sizeInBytes = m_width * m_height * ByteCountFromTextureFormat( m_format );
            memcpy( m_pixels, pPixels, sizeInBytes );
        }

        //----------------------------------------------------------------------
        // @Params:
        //  "pPixels": Pointer to the pixel-data.
        //  "sizeInBytes": Size in bytes to copy from the pPixels pointer.
        //----------------------------------------------------------------------
        void setPixels( const void* pPixels, Size sizeInBytes ) 
        {
            ASSERT( not isImmutable() ); 
            memcpy( m_pixels, pPixels, sizeInBytes );
        }

    protected:
        bool                m_generateMips = true;
        bool                m_isImmutable = true;

        // Heap allocated mem for pixels. How large it is depends on width/height and the format
        void*               m_pixels = nullptr;

        //----------------------------------------------------------------------
        void _CalculateMipCount() { m_mipCount = (U32)floor( log2( std::min( m_width, m_height ) ) ) + 1; }

    private:
        //----------------------------------------------------------------------
        ITexture2D(const ITexture2D& other)               = delete;
        ITexture2D& operator = (const ITexture2D& other)  = delete;
        ITexture2D(ITexture2D&& other)                    = delete;
        ITexture2D& operator = (ITexture2D&& other)       = delete;
    };

    using Texture2D = ITexture2D;

}