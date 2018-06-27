#pragma once
/**********************************************************************
    class: RenderTexture (i_render_texture.h)

    author: S. Hau
    date: March 4, 2018

    Represents a set of renderbuffers which can be set as the rendertarget for a camera.
    Note that all renderbuffers in this class MUST have the same dimension.
    The reason for a set of color- and/or depth-buffer is that so the
    same render texture can practically render itself (e.g. using it as a mirror).
**********************************************************************/

#include "i_renderbuffer.hpp"

namespace Graphics
{

    //**********************************************************************
    class IRenderTexture
    {
    public:
        IRenderTexture() {}
        virtual ~IRenderTexture() = default;

        //----------------------------------------------------------------------
        inline U32                         getWidth()                  const { return (U32)(getBuffer()->getWidth()); }
        inline U32                         getHeight()                 const { return (U32)(getBuffer()->getHeight()); }
        inline bool                        dynamicScales()             const { return m_dynamicScale; }
        inline F32                         getDynamicScaleFactor()     const { return m_scaleFactor; }
        inline bool                        hasColorBuffer()            const { return getColorBuffer() != nullptr; }
        inline bool                        hasDepthBuffer()            const { return getDepthBuffer() != nullptr; }
        inline F32                         getAspectRatio()            const { return getBuffer()->getAspectRatio(); }
        inline SamplingDescription         getSamplingDescription()    const { return getBuffer()->getSamplingDescription(); }

        //----------------------------------------------------------------------
        // These set functions will be forwarded to all renderbuffers within this texture.
        //----------------------------------------------------------------------
        void setFilter(TextureFilter filter);
        void setClampMode(TextureAddressMode clampMode);
        void setAnisoLevel(U32 level);

        //----------------------------------------------------------------------
        // @Return:
        //  Color-Buffer if present, otherwise nullptr.
        //----------------------------------------------------------------------
        inline const RenderBufferPtr& getColorBuffer() const { return m_renderBuffers[m_bufferIndex].m_colorBuffer; }

        //----------------------------------------------------------------------
        // @Return:
        //  Depth-Buffer if present, otherwise nullptr.
        //----------------------------------------------------------------------
        inline const RenderBufferPtr& getDepthBuffer() const { return m_renderBuffers[m_bufferIndex].m_depthBuffer; }

        //----------------------------------------------------------------------
        // @Return:
        //  First buffer which exists, i.e. colorbuffer if existent, otherwise depthbuffer.
        //----------------------------------------------------------------------
        inline const RenderBufferPtr& getBuffer() const { return hasColorBuffer() ? getColorBuffer() : getDepthBuffer(); }

        //----------------------------------------------------------------------
        // @Params:
        //  "shouldScale": If set to true, the texture will be automatically scale up/down whenever the window resizes.
        //  "scaleFactor": This is for super-/undersampling.
        //----------------------------------------------------------------------
        void setDynamicScreenScale(bool shouldScale, F32 scaleFactor = 1.0f);

        //----------------------------------------------------------------------
        // Clears the color/depth buffer with the given color, depth and stencil data.
        //----------------------------------------------------------------------
        void clear(Color color, F32 depth, U8 stencil);

        //----------------------------------------------------------------------
        // Clears the depth buffer with the given depth and stencil data.
        //----------------------------------------------------------------------
        void clearDepthStencil(F32 depth, U8 stencil);

        //----------------------------------------------------------------------
        void create(const RenderBufferPtr& colorBuffer, const RenderBufferPtr& depthBuffer = nullptr);
        void create(const ArrayList<RenderBufferPtr>& colorBuffers, const ArrayList<RenderBufferPtr>& depthBuffers);

        //----------------------------------------------------------------------
        // Recreate all buffers in this render texture.
        //----------------------------------------------------------------------
        void recreate(U32 w, U32 h) { recreate(w, h, getSamplingDescription()); }
        void recreate(SamplingDescription samplingDesc) { recreate(getWidth(), getHeight(), samplingDesc); }
        void recreate(U32 w, U32 h, SamplingDescription samplingDesc);
        void recreate(Graphics::TextureFormat format);

    protected:
        F32  m_scaleFactor = 1.0f;
        bool m_dynamicScale = false; // If true it will be scaled automatically when the window resizes.

        struct RenderBuffers
        {
            RenderBufferPtr m_colorBuffer;
            RenderBufferPtr m_depthBuffer;
        };

        ArrayList<RenderBuffers> m_renderBuffers;

        // Framebuffer index. Only used if this render texture contains a SET of renderbuffers.
        I32 m_bufferIndex = 0;

        I32 _PreviousBufferIndex();

    private:
        //----------------------------------------------------------------------
        // Bind this color and/or depth buffer to the output merger for rendering.
        //----------------------------------------------------------------------
        friend class D3D11Renderer;
        virtual void bindForRendering() = 0;

        NULL_COPY_AND_ASSIGN(IRenderTexture)
    };

    using RenderTexture = IRenderTexture;

}

using RenderTexturePtr = std::shared_ptr<Graphics::IRenderTexture>;