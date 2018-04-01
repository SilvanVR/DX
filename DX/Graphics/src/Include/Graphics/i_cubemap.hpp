#pragma once
/**********************************************************************
    class: ICubemap (i_cubemap.hpp)

    author: S. Hau
    date: April 1, 2018

    Interface class for a cubemap
**********************************************************************/

#include "i_texture.h"

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
        ICubemap() = default;
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
        // Set one pixel for the specified cubemap face. This function is only supported by the formats RGBA32 and BGRA32.
        // @Params:
        //  "face": Cubemap face to modify
        //  "x": X-Position of the specified face in pixel-coords.
        //  "y": Y-Position of the specified face in pixel-coords.
        //  "color": New color of the pixel.
        //----------------------------------------------------------------------
        virtual void setPixel(CubemapFace face, U32 x, U32 y, Color color) = 0;

        //----------------------------------------------------------------------
        // Set pixels for a whole cubemap face.
        // @Params:
        //  "face": Cubemap face to modify.
        //  "pPixels": Pointer to pixel data. 
        //----------------------------------------------------------------------
        virtual void setPixels(CubemapFace face, const void* pPixels) = 0;

        //----------------------------------------------------------------------
        // Apply all previous setPixel() changes
        // @Params:
        //  "updateMips": Mipmaps will be updated. Ignored if tex has no mips.
        //----------------------------------------------------------------------
        virtual void apply(bool updateMips = true) = 0;


    private:
        //----------------------------------------------------------------------
        ICubemap(const ICubemap& other)               = delete;
        ICubemap& operator = (const ICubemap& other)  = delete;
        ICubemap(ICubemap&& other)                    = delete;
        ICubemap& operator = (ICubemap&& other)       = delete;
    };

    using Cubemap = ICubemap;

}