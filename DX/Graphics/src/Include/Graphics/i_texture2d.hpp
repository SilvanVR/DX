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
        ITexture2D() : ITexture(TextureDimension::Tex2D) {}
        virtual ~ITexture2D() {}

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
        // @Params:
        //  "updateMips": If true, mipmaps will be generated. Ignored if not possible.
        //  "keepPixelsInRAM": If true, pixel data won't be deleted after uploading to the gpu.
        //----------------------------------------------------------------------
        virtual void apply(bool updateMips = true, bool keepPixelsInRAM = false) = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  Whether the texture can be dynamically updated or is immutable.
        //----------------------------------------------------------------------
        bool isImmutable() const { return m_isImmutable; }

        //----------------------------------------------------------------------
        // Change one pixel. This function is only supported for the formats RGBA32 and BGRA32.
        //----------------------------------------------------------------------
        void setPixel( U32 x, U32 y, Color color ) 
        { 
            ASSERT( not isImmutable() 
                    && (m_format == TextureFormat::RGBA32 || m_format == TextureFormat::BGRA32) 
                    && x < m_width && y < m_height );

            if ( m_pixels.empty() )
                m_pixels.resize( m_width * m_height * ByteCountFromTextureFormat( m_format ) );

            reinterpret_cast<Color*>( m_pixels.data() )[x + y * m_width] = color; 
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
            if ( m_pixels.empty() )
                m_pixels.resize( sizeInBytes );

            memcpy( m_pixels.data(), pPixels, sizeInBytes );
        }

        //----------------------------------------------------------------------
        // @Params:
        //  "pPixels": Pointer to the pixel-data.
        //  "sizeInBytes": Size in bytes to copy from the pPixels pointer.
        //----------------------------------------------------------------------
        void setPixels( const void* pPixels, Size sizeInBytes ) 
        {
            ASSERT( not isImmutable() && ( sizeInBytes <= m_pixels.size() ) );

            if ( m_pixels.empty() )
                m_pixels.resize( m_width * m_height * ByteCountFromTextureFormat( m_format ) );

            memcpy( m_pixels.data(), pPixels, sizeInBytes );
        }

        //----------------------------------------------------------------------
        // Return the pixels for this texture. P.S. This might be empty after
        // the texture data was uploaded to the gpu.
        //----------------------------------------------------------------------
        const ArrayList<Byte>& getPixels() const { return m_pixels; }

    protected:
        bool                m_isImmutable = true;

        // Heap allocated mem for pixels. How large it is depends on width/height and the format
        ArrayList<Byte>     m_pixels;

    private:
        //----------------------------------------------------------------------
        ITexture2D(const ITexture2D& other)               = delete;
        ITexture2D& operator = (const ITexture2D& other)  = delete;
        ITexture2D(ITexture2D&& other)                    = delete;
        ITexture2D& operator = (ITexture2D&& other)       = delete;
    };

    using Texture2D = ITexture2D;

}

using Texture2DPtr = std::shared_ptr<Graphics::ITexture2D>;