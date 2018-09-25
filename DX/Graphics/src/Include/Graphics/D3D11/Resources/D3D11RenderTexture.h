#pragma once
/**********************************************************************
    class: RenderTexture (D3D11RenderTexture.h)

    author: S. Hau
    date: March 24, 2018

    D3D11 implementation of a render-texture. It can consist of more than 
    one buffer, so it can be bound as a srv and to the output-merger
    at the same time.
**********************************************************************/

#include "i_render_texture.h"
#include "D3D11/D3D11.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class RenderTexture : public Graphics::IRenderTexture
    {
    public:
        RenderTexture() = default;
        ~RenderTexture() = default;

        //----------------------------------------------------------------------
        // IRenderTexture Interface
        //----------------------------------------------------------------------
        void bindForRendering(U64 frameIndex) override;
        U64* getNativeTexturePtr() const override { return nullptr; }

    private:
        NULL_COPY_AND_ASSIGN(RenderTexture)
    };

} } // End namespaces