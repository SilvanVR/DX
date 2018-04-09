#pragma once
/**********************************************************************
    class: ICubemap (i_cubemap.hpp)

    author: S. Hau
    date: April 1, 2018

    Interface class for a cubemap
**********************************************************************/

#include "i_texture.h"
#include "Utils/utils.h"

namespace Graphics
{
    //----------------------------------------------------------------------
    enum class CubemapFace
    {
        Unknown = -1,
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5
    };

    //**********************************************************************
    class ICubemap : public ITexture
    {
    public:
        ICubemap() : ITexture(TextureDimension::Cube) {}
        virtual ~ICubemap() {}

        //----------------------------------------------------------------------
        // Create a new cubemap texture on the gpu
        // @Params:
        //  "size": Width/Height of each face in pixels
        //  "format": Format for each pixel
        //  "generateMips": If true a complete mipmap-chain will be generated for each face
        //----------------------------------------------------------------------
        virtual void create(I32 size, TextureFormat format, bool generateMips) = 0;

        //----------------------------------------------------------------------
        // Apply all previous setPixel() changes
        // @Params:
        //  "updateMips": Mipmaps will be updated. Ignored if tex has no mips.
        //  "keepPixelsInRAM": If true, pixel data won't be deleted after uploading to the gpu.
        //----------------------------------------------------------------------
        virtual void apply(bool updateMips = true, bool keepPixelsInRAM = false) = 0;

        //----------------------------------------------------------------------
        // Set one pixel for the specified cubemap face. This function is only supported by the formats RGBA32 and BGRA32.
        // @Params:
        //  "face": Cubemap face to modify
        //  "x": X-Position of the specified face in pixel-coords.
        //  "y": Y-Position of the specified face in pixel-coords.
        //  "color": New color of the pixel.
        //----------------------------------------------------------------------
        void setPixel(CubemapFace face, U32 x, U32 y, Color color)
        {
            ASSERT( (m_format == TextureFormat::RGBA32 || m_format == TextureFormat::BGRA32)
                   && not m_facePixels[(I32)face].empty() 
                   && x < m_width && y < m_height );
            reinterpret_cast<Color*>( m_facePixels[(I32)face].data() )[x + y * m_width] = color;
        }

        //----------------------------------------------------------------------
        // Set pixels for a whole cubemap face.
        // @Params:
        //  "face": Cubemap face to modify.
        //  "pPixels": Pointer to pixel data. 
        //----------------------------------------------------------------------
        void setPixels(CubemapFace face, const void* pPixels)
        {
            ASSERT( not m_facePixels[(I32)face].empty() );
            Size sizeInBytes = m_width * m_height * ByteCountFromTextureFormat( m_format );
            ASSERT( m_facePixels[(I32)face].size() <= sizeInBytes );
            memcpy( m_facePixels[(I32)face].data(), pPixels, sizeInBytes );
        }

        //----------------------------------------------------------------------
        // Return the pixels for this texture. P.S. This might be empty after
        // the texture data was uploaded to the gpu.
        //----------------------------------------------------------------------
        const ArrayList<Byte>& getPixels(CubemapFace face) const { return m_facePixels[I32(face)]; }

    protected:
        // Heap allocated mem for each face. How large it is depends on width/height and the format
        ArrayList<Byte> m_facePixels[6];

    private:
        //----------------------------------------------------------------------
        ICubemap(const ICubemap& other)               = delete;
        ICubemap& operator = (const ICubemap& other)  = delete;
        ICubemap(ICubemap&& other)                    = delete;
        ICubemap& operator = (ICubemap&& other)       = delete;
    };

    using Cubemap = ICubemap;

}

using CubemapPtr = std::shared_ptr<Graphics::ICubemap>;