#pragma once
/**********************************************************************
    class: RenderTexture (render_texture.h)

    author: S. Hau
    date: March 4, 2018

    Texture on the gpu, which can be set as the rendertarget for a camera.
**********************************************************************/

namespace Graphics
{

    class RenderTexture
    {
    public:

        F32 getAspectRatio() const { return 1.0f; }
        F32 getWidth() const { return 1.0f; }
        F32 getHeight() const { return 1.0f; }
    };


}