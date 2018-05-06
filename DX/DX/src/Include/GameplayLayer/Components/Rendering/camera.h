#pragma once
/**********************************************************************
    class: Camera (camera.h)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "../i_component.h"
#include "Common/bit_mask.hpp"
#include "Graphics/structs.hpp"
#include "Graphics/command_buffer.h"
#include "Graphics/i_render_texture.hpp"

namespace Core { class GraphicsCommandRecorder; }

namespace Components {

    class IRenderComponent;

    //**********************************************************************
    class Camera : public IComponent
    {
    public:
        //----------------------------------------------------------------------
        enum EMode
        {
            PERSPECTIVE,
            ORTHOGRAPHIC
        };

        enum class EClearMode
        {
            NONE,
            COLOR
        };

        Camera(F32 fovAngleYInDegree = 45.0f, F32 zNear = 0.1f, F32 zFar = 1000.0f);
        Camera(F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        ~Camera();

        //----------------------------------------------------------------------
        void setCameraMode          (Camera::EMode mode)        { m_cameraMode = mode; }
        void setZNear               (F32 zNear)                 { m_zNear = zNear; }
        void setZFar                (F32 zFar)                  { m_zFar = zFar; }
        void setFOV                 (F32 fovAngleYInDegree)     { m_fov = fovAngleYInDegree; }
        void setClearColor          (const Color& clearColor)   { m_clearColor = clearColor; }
        void setClearMode           (EClearMode clearMode)      { m_clearMode = clearMode; }
        void setOrthoParams         (F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        void setPerspectiveParams   (F32 fovAngleYInDegree, F32 zNear, F32 zFar);
        void setViewport            (const Graphics::ViewportRect& viewport) { m_viewport = viewport; }
        void setCullingMask         (U32 layers)                { m_cullingMask.unsetAnyBit(); m_cullingMask.setBits(layers); }

        //----------------------------------------------------------------------
        bool                            isRenderingToScreen()   const { return m_renderTarget == nullptr; }
        const EMode&                    getCameraMode()         const { return m_cameraMode; }
        F32                             getZNear()              const { return m_zNear; }
        F32                             getZFar()               const { return m_zFar; }
        F32                             getFOV()                const { return m_fov; }
        bool                            isOrthographic()        const { return m_cameraMode == ORTHOGRAPHIC; }
        const Color&                    getClearColor()         const { return m_clearColor; }
        F32                             getLeft()               const { return m_ortho.left; }
        F32                             getRight()              const { return m_ortho.right; }
        F32                             getTop()                const { return m_ortho.top; }
        F32                             getBottom()             const { return m_ortho.bottom; }
        EClearMode                      getClearMode()          const { return m_clearMode; }
        F32                             getAspectRatio()        const;
        Common::BitMask                 getLayerMask()          const { return m_cullingMask; }

        //----------------------------------------------------------------------
        // @Return
        // Returns the viewport in which this camera renders.
        // Note that the viewport is in normalized coordinates [0-1].
        //----------------------------------------------------------------------
        const Graphics::ViewportRect&   getViewport()           const { return m_viewport; }
        Graphics::ViewportRect&         getViewport()                 { return m_viewport; }

        //----------------------------------------------------------------------
        // @Return:
        //  Target texture in which this camera renders. Nullptr if rendering to screen.
        //----------------------------------------------------------------------
        RenderTexturePtr                getRenderTarget() { return m_renderTarget; }

        //----------------------------------------------------------------------
        // Set the render target in which this camera renders. Nullptr means the camera should
        // render to the screen. The viewport will be adapted to cover the whole texture.
        //----------------------------------------------------------------------
        void setRenderTarget(RenderTexturePtr renderTarget) { m_renderTarget = renderTarget; }

        //----------------------------------------------------------------------
        // Add additional command buffer to this camera
        //----------------------------------------------------------------------
        void addCommandBuffer(Graphics::CommandBuffer* cmd) { m_additionalCommandBuffers.push_back(cmd); }

        //----------------------------------------------------------------------
        // Remove a command buffer from this camera
        //----------------------------------------------------------------------
        void removeCommandBuffer(Graphics::CommandBuffer* cmd) { m_additionalCommandBuffers.erase( std::remove( m_additionalCommandBuffers.begin(), m_additionalCommandBuffers.end(), cmd ) ); }

        //----------------------------------------------------------------------
        //const DirectX::XMMATRIX& getProjectionMatrix() const { return m_projection; }
        DirectX::XMMATRIX getProjectionMatrix() const;
        const DirectX::XMMATRIX& getViewProjectionMatrix() const { return m_viewProjection; }


    private:
        EMode m_cameraMode = EMode::PERSPECTIVE;

        // Z Clipping Planes
        F32 m_zNear     = 0.1f;
        F32 m_zFar      = 1000.0f;

        // Perspective Params
        F32 m_fov       = 45.0f;

        // Ortographics Params
        struct {
            F32 left    = -1.0f;
            F32 right   =  1.0f;
            F32 bottom  = -1.0f;
            F32 top     =  1.0f;
        } m_ortho;

        // Viewport Rect in normalized coordinates [0-1]
        Graphics::ViewportRect  m_viewport;

        // Clear Mode + Clear Color for this camera
        EClearMode              m_clearMode     = EClearMode::COLOR;
        Color                   m_clearColor    = Color::BLACK;

        // Target render texture (nullptr means camera renders to screen)
        RenderTexturePtr        m_renderTarget = nullptr;

        // Which layer this camera renders
        Common::BitMask         m_cullingMask;

        // Projection + ViewProjection Matrix
        DirectX::XMMATRIX       m_viewProjection;

        // Contains commands to render one frame into the rendertarget
        Graphics::CommandBuffer m_commandBuffer;

        // Additional attached command buffer
        ArrayList<Graphics::CommandBuffer*> m_additionalCommandBuffers;

        //----------------------------------------------------------------------
        //void _UpdateProjectionMatrix();

        //----------------------------------------------------------------------
        friend class Core::GraphicsCommandRecorder;
        Graphics::CommandBuffer& recordGraphicsCommands(F32 lerp, const ArrayList<IRenderComponent*>& rendererComponents);
        ArrayList<Graphics::CommandBuffer*>& getAdditionalCommandBuffers() { return m_additionalCommandBuffers; }

        //----------------------------------------------------------------------
        // Sorts all rendering commands from the given command buffer by material, renderqueue and camera distance
        //----------------------------------------------------------------------
        inline void _SortRenderCommands(const Graphics::CommandBuffer& cmd, const Math::Vec3& position);

        //----------------------------------------------------------------------
        Camera(const Camera& other)               = delete;
        Camera& operator = (const Camera& other)  = delete;
        Camera(Camera&& other)                    = delete;
        Camera& operator = (Camera&& other)       = delete;
    };

}