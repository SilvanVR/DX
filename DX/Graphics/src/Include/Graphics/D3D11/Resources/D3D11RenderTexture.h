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
#include "D3D11IBindableTexture.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class RenderTexture : public Graphics::IRenderTexture, public D3D11::IBindableTexture
    {
    public:
        RenderTexture() = default;
        ~RenderTexture() {}

        //----------------------------------------------------------------------
        // IRenderTexture Interface
        //----------------------------------------------------------------------
        void bindForRendering() override;

        //----------------------------------------------------------------------
        // D3D11ITexture Interface
        //----------------------------------------------------------------------
        void bind(ShaderType shaderType, U32 slot) override;
        ID3D11Texture2D* getD3D11Texture() override;

    private:
        //----------------------------------------------------------------------
        RenderTexture(const RenderTexture& other)               = delete;
        RenderTexture& operator = (const RenderTexture& other)  = delete;
        RenderTexture(RenderTexture&& other)                    = delete;
        RenderTexture& operator = (RenderTexture&& other)       = delete;
    };

} } // End namespaces