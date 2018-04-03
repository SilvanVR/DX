#pragma once
/**********************************************************************
    class: Texture2DArray (i_texture2d.hpp)

    author: S. Hau
    date: April 3, 2018
**********************************************************************/

#include "i_texture.h"
#include "Utils/utils.h"

namespace Graphics
{

    //**********************************************************************
    class ITexture2DArray : public ITexture
    {
    public:
        ITexture2DArray() : ITexture(TextureDimension::Tex2DArray) {}
        virtual ~ITexture2DArray() {}

        //----------------------------------------------------------------------
        U32 getDepthCount() const { return m_depth; }

        //----------------------------------------------------------------------
        // Creates a new 2d-texture array.
        // @Params:
        //  "width": Width in pixels.
        //  "height": Height in pixels.
        //  "depth": Number of elements in the texture array.
        //  "format": The texture format.
        //  "generateMips": If true a complete mipchain will be generated for each texture.
        //----------------------------------------------------------------------
        virtual void create(U32 width, U32 height, U32 depth, TextureFormat format, bool generateMips = true) = 0;

        //----------------------------------------------------------------------
        // Apply all previous pixels changes to the texture.
        // @Params:
        //  "updateMips": If true, mipmaps will be generated. Ignored if not possible.
        //  "keepPixelsInRAM": If true, pixel data won't be deleted after uploading to the gpu.
        //----------------------------------------------------------------------
        virtual void apply(bool updateMips = true, bool keepPixelsInRAM = false) = 0;

        //----------------------------------------------------------------------
        // Set a pixel for a single array slice. (Copies width * height * byteWidth from the given pointer)
        // @Params:
        //  "slice": Texture index.
        //  "pPixels": Pointer to the pixel-data.
        //----------------------------------------------------------------------
        void setPixels(I32 slice, const void* pPixels)
        {
            Size sizeInBytes = m_width * m_height * ByteCountFromTextureFormat( m_format );
            ASSERT( m_pixels[slice].size() <= sizeInBytes );
            memcpy( m_pixels[slice].data(), pPixels, sizeInBytes );
        }

        //----------------------------------------------------------------------
        // Return the pixels for this texture. P.S. This might be empty after
        // the texture data was uploaded to the gpu.
        // @Params:
        //  "slice": Pixels from the array slice.
        //----------------------------------------------------------------------
        const ArrayList<Byte>& getPixels(I32 slice) const { return m_pixels[slice]; }

    protected:
        // One array of pixels for each texture.
        ArrayList<ArrayList<Byte>>  m_pixels;
        U32                         m_depth = 0;

    private:
        //----------------------------------------------------------------------
        ITexture2DArray(const ITexture2DArray& other)               = delete;
        ITexture2DArray& operator = (const ITexture2DArray& other)  = delete;
        ITexture2DArray(ITexture2DArray&& other)                    = delete;
        ITexture2DArray& operator = (ITexture2DArray&& other)       = delete;
    };

    using Texture2DArray = ITexture2DArray;

}