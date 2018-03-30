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
        IRenderTexture(U32 width, U32 height, U32 depth, TextureFormat format) 
            : ITexture(width, height, format), m_depth(depth) {}
        virtual ~IRenderTexture() = default;

        //----------------------------------------------------------------------
        // Clears the texture with the given color, depth and stencil data.
        //----------------------------------------------------------------------
        virtual void clear(Color color, F32 depth, U8 stencil) = 0;

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