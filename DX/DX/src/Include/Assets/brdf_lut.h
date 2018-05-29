#pragma once
/**********************************************************************
    class: BRDFLut (brdf_lut.h)

    author: S. Hau
    date: May 29, 2018

    2D BRDF loop-up-texture for Physically based rendering.
**********************************************************************/

#include "Graphics/i_render_texture.hpp"

namespace Assets { 

    //*********************************************************************
    class BRDFLut
    {
    public:
        BRDFLut();
        ~BRDFLut() = default;

        //----------------------------------------------------------------------
        const RenderTexturePtr& getTexture() const { return m_brdfLut; }

    private:
        RenderTexturePtr m_brdfLut;

        //----------------------------------------------------------------------
        BRDFLut(const BRDFLut& other)               = delete;
        BRDFLut& operator = (const BRDFLut& other)  = delete;
        BRDFLut(BRDFLut&& other)                    = delete;
        BRDFLut& operator = (BRDFLut&& other)       = delete;
    };

} // End namespaces