#pragma once
/**********************************************************************
    class: RenderTexture (render_texture.h)

    author: S. Hau
    date: March 4, 2018

    Texture on the gpu, which can be set as the rendertarget for a camera.
**********************************************************************/

#include "i_texture.h"

namespace Graphics
{

    //**********************************************************************
    class IRenderTexture : public ITexture
    {
    public:
        IRenderTexture() = default;
        virtual ~IRenderTexture() = default;

        //----------------------------------------------------------------------
        // Clears the texture with the given color, depth and stencil data.
        //----------------------------------------------------------------------
        virtual void clear(Color color, F32 depth, U8 stencil) = 0;

        //----------------------------------------------------------------------
        // Creates a new render texture.
        // @Params:
        //  "width": Width in pixels.
        //  "height": Height in pixels.
        //  "depth": Bit-depth of the depth-buffer. Allowed is only 0, 16, 24, 32
        //  "format": The texture format.
        //----------------------------------------------------------------------
        virtual void create(U32 width, U32 height, U32 depth, TextureFormat format) = 0;

    protected:
        U32 m_depth = 0;

    private:
        //----------------------------------------------------------------------
        IRenderTexture(const IRenderTexture& other)               = delete;
        IRenderTexture& operator = (const IRenderTexture& other)  = delete;
        IRenderTexture(IRenderTexture&& other)                    = delete;
        IRenderTexture& operator = (IRenderTexture&& other)       = delete;
    };

    using RenderTexture = IRenderTexture;

}