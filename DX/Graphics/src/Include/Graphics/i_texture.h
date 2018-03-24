#pragma once
/**********************************************************************
    class: Texture (texture.h)

    author: S. Hau
    date: March 4, 2018

**********************************************************************/

namespace Graphics
{

    class ITexture
    {
    public:
        F32 getAspectRatio() const { return 1.0f; }
        F32 getWidth() const { return 1.0f; }
        F32 getHeight() const { return 1.0f; }

    private:
        //----------------------------------------------------------------------
        ITexture(const ITexture& other)               = delete;
        ITexture& operator = (const ITexture& other)  = delete;
        ITexture(ITexture&& other)                    = delete;
        ITexture& operator = (ITexture&& other)       = delete;
    };


    using Texture = ITexture;

}