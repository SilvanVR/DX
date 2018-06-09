#pragma once
/**********************************************************************
    class: RenderTexture (render_texture.h)

    author: S. Hau
    date: March 4, 2018

    Texture on the gpu, which can be set as the rendertarget for a camera.
**********************************************************************/

#include "i_texture.h"
#include "structs.hpp"

namespace Graphics
{

    //**********************************************************************
    class IRenderTexture : public ITexture
    {
    public:
        IRenderTexture() : ITexture(TextureDimension::Tex2D) {}
        virtual ~IRenderTexture() = default;

        //----------------------------------------------------------------------
        U32                         getWidth()                  const override  { return (U32)(m_width * m_scaleFactor); }
        U32                         getHeight()                 const override  { return (U32)(m_height * m_scaleFactor); }
        const SamplingDescription&  getSamplingDescription()    const           { return m_samplingDescription; }
        bool                        dynamicScales()             const           { return m_dynamicScale; }
        F32                         getDynamicScaleFactor()     const           { return m_scaleFactor; }
        bool                        hasDepthBuffer()            const           { return m_depthBuffer != nullptr; }

        //----------------------------------------------------------------------
        // @Return:
        //  Depth-Buffer if present, otherwise nullptr.
        //----------------------------------------------------------------------
        const TexturePtr& getDepthBuffer() const { return m_depthBuffer; }

        //----------------------------------------------------------------------
        // @Params:
        //  "shouldScale": If set to true, the texture will be automatically scale up/down whenever the window resizes.
        //  "scaleFactor": This is for super-/undersampling.
        //----------------------------------------------------------------------
        void setDynamicScreenScale(bool shouldScale, F32 scaleFactor = 1.0f) { m_dynamicScale = shouldScale; m_scaleFactor = scaleFactor; recreate(m_width, m_height); }

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
        //  "numBuffers": Amount of buffers to create. This should be greater than one if you plan 
        //                to use the rt as a rendertarget and in a material at the same time(e.g.as a mirror)
        //  "samplingDesc": Whether the buffer will be multisampled or not.
        //----------------------------------------------------------------------
        virtual void create(U32 width, U32 height, U32 depth, TextureFormat format, U32 numBuffers = 1, SamplingDescription samplingDesc = {}) = 0;

        //----------------------------------------------------------------------
        // Bind this texture to the output merger for rendering.
        //----------------------------------------------------------------------
        virtual void bindForRendering() = 0;

        //----------------------------------------------------------------------
        // Recreate all buffers in this render texture.
        //----------------------------------------------------------------------
        virtual void recreate(U32 w, U32 h) = 0;
        virtual void recreate(U32 w, U32 h, SamplingDescription) = 0;
        virtual void recreate(Graphics::TextureFormat format) = 0;

    protected:
        U32                 m_depth = 0; // Number of bits in the depth buffer
        SamplingDescription m_samplingDescription;
        F32                 m_scaleFactor = 1.0f;
        bool                m_dynamicScale = false; // If true it will be scaled automatically when the window resizes.
        TexturePtr          m_depthBuffer = nullptr;

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