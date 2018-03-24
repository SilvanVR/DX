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

    class RenderTexture : public ITexture
    {

    private:
        //----------------------------------------------------------------------
        RenderTexture(const RenderTexture& other)               = delete;
        RenderTexture& operator = (const RenderTexture& other)  = delete;
        RenderTexture(RenderTexture&& other)                    = delete;
        RenderTexture& operator = (RenderTexture&& other)       = delete;
    };


}