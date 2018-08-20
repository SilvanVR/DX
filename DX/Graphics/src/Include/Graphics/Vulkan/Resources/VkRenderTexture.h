#pragma once
/**********************************************************************
    class: RenderTexture

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_render_texture.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class RenderTexture : public IRenderTexture
    {
    public:
        RenderTexture() = default;
        ~RenderTexture() = default;

        //----------------------------------------------------------------------
        // IRenderTexture Interface
        //----------------------------------------------------------------------
        void bindForRendering(U64 frameIndex) override;

    private:
        NULL_COPY_AND_ASSIGN(RenderTexture)
    };

} } // End namespaces