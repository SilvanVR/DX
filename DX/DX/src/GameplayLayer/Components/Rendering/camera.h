#pragma once
/**********************************************************************
    class: Camera (camera.h)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "../i_component.h"
#include "Common/bit_mask.hpp"
#include "Graphics/structs.hpp"

namespace Graphics { class RenderTexture; }
namespace Core { class GraphicsCommandRecorder; }

namespace Components {

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
        Graphics::RenderTexture* getRenderTarget() { return m_renderTarget; }

        //----------------------------------------------------------------------
        // Set the render target in which this camera renders. Nullptr means the camera should
        // render to the screen. The viewport will be adapted to cover the whole texture.
        //----------------------------------------------------------------------
        void setRenderTarget(Graphics::RenderTexture* renderTarget) { m_renderTarget = renderTarget; }

        //----------------------------------------------------------------------
        // const DirectX::XMMATRIX& getProjectionMatrix() const { return m_projection; }

    private:
        EMode           m_cameraMode;
        //DirectX::XMMATRIX     m_projection;

        // Z Clipping Plane
        F32             m_zNear     = 0.1f;
        F32             m_zFar      = 1000.0f;

        // Perspective Params
        F32             m_fov       = 45.0f;

        // Ortographics Params
        struct {
            F32 left                = -1.0f;
            F32 right               = 1.0f;
            F32 bottom              = -1.0f;
            F32 top                 = 1.0f;
        } m_ortho;

        // Viewport Rect in normalized coordinates [0-1]
        Graphics::ViewportRect m_viewport;

        // Clear Mode + Clear Color for this camera
        EClearMode  m_clearMode     = EClearMode::COLOR;
        Color       m_clearColor    = Color::BLACK;

        // Target render texture (nullptr means camera renders to screen)
        Graphics::RenderTexture* m_renderTarget = nullptr;

        // Which layer this camera renders
        //Common::BitMask               m_layerMask; 

        //----------------------------------------------------------------------
        friend class Core::GraphicsCommandRecorder;
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp);

        //----------------------------------------------------------------------
        Camera(const Camera& other)               = delete;
        Camera& operator = (const Camera& other)  = delete;
        Camera(Camera&& other)                    = delete;
        Camera& operator = (Camera&& other)       = delete;
    };

}