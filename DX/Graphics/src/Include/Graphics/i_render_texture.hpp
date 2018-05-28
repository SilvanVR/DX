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
        IRenderTexture() : ITexture(TextureDimension::Tex2D) {}
        virtual ~IRenderTexture() = default;

        //----------------------------------------------------------------------
        // Clears the texture with the given color, depth and stencil data.
        //----------------------------------------------------------------------
        virtual void clear(Color color, F32 depth, U8 stencil) = 0;

        //----------------------------------------------------------------------
        // Clears the texture with the given depth and stencil data.
        //----------------------------------------------------------------------
        virtual void clearDepthStencil(F32 depth, U8 stencil) = 0;

        //----------------------------------------------------------------------
        // Creates a new render texture.
        // @Params:
        //  "width": Width in pixels.
        //  "height": Height in pixels.
        //  "depth": Bit-depth of the depth-buffer. Allowed is only 0, 16, 24, 32
        //  "format": The texture format.
        //  "numBuffers": Amount of buffers to create. This should be greater than one 
        //  if you plan to use the rt as a rendertarget and in a material at the same time (e.g. as a mirror)
        //----------------------------------------------------------------------
        virtual void create(U32 width, U32 height, U32 depth, TextureFormat format, U32 numBuffers = 1) = 0;

        //----------------------------------------------------------------------
        // Bind this texture to the output merger for rendering.
        //----------------------------------------------------------------------
        virtual void bindForRendering() = 0;

    protected:
        U32 m_depth = 0; // Number of bits in the depth buffer

    private:
        //----------------------------------------------------------------------
        IRenderTexture(const IRenderTexture& other)               = delete;
        IRenderTexture& operator = (const IRenderTexture& other)  = delete;
        IRenderTexture(IRenderTexture&& other)                    = delete;
        IRenderTexture& operator = (IRenderTexture&& other)       = delete;
    };

    using RenderTexture = IRenderTexture;

}

using RenderTexturePtr = std::shared_ptr<Graphics::IRenderTexture>;