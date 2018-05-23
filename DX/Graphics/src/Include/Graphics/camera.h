#pragma once
/**********************************************************************
    class: Camera (camera.h)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "i_render_texture.hpp"
#include "structs.hpp"

namespace Graphics {

    //----------------------------------------------------------------------
    enum class CameraClearMode
    {
        None,
        Color,
        Depth
    };

    //----------------------------------------------------------------------
    enum class CameraMode
    {
        Perspective,
        Orthographic
    };

    //**********************************************************************
    class Camera
    {
    public:
        Camera(F32 fovAngleYInDegree = 45.0f, F32 zNear = 0.1f, F32 zFar = 1000.0f);
        Camera(F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        ~Camera() = default;

        //----------------------------------------------------------------------
        void setCameraMode          (CameraMode mode)        { m_cameraMode = mode; }
        void setZNear               (F32 zNear)                 { m_zNear = zNear; }
        void setZFar                (F32 zFar)                  { m_zFar = zFar; }
        void setFOV                 (F32 fovAngleYInDegree)     { m_fov = fovAngleYInDegree; }
        void setClearColor          (const Color& clearColor)   { m_clearColor = clearColor; }
        void setClearMode           (CameraClearMode clearMode)      { m_clearMode = clearMode; }
        void setOrthoParams         (F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        void setPerspectiveParams   (F32 fovAngleYInDegree, F32 zNear, F32 zFar);
        void setViewport            (const Graphics::ViewportRect& viewport) { m_viewport = viewport; }

        //----------------------------------------------------------------------
        inline bool                    isRenderingToScreen()   const { return m_renderTarget == nullptr; }
        inline const CameraMode&       getCameraMode()         const { return m_cameraMode; }
        inline F32                     getZNear()              const { return m_zNear; }
        inline F32                     getZFar()               const { return m_zFar; }
        inline F32                     getFOV()                const { return m_fov; }
        inline bool                    isOrthographic()        const { return m_cameraMode == CameraMode::Orthographic; }
        inline const Color&            getClearColor()         const { return m_clearColor; }
        inline F32                     getLeft()               const { return m_ortho.left; }
        inline F32                     getRight()              const { return m_ortho.right; }
        inline F32                     getTop()                const { return m_ortho.top; }
        inline F32                     getBottom()             const { return m_ortho.bottom; }
        inline CameraClearMode         getClearMode()          const { return m_clearMode; }
        inline F32                     getAspectRatio()        const;

        //----------------------------------------------------------------------
        // @Return
        // Returns the viewport in which this camera renders.
        // Note that the viewport is in normalized coordinates [0-1].
        //----------------------------------------------------------------------
        inline const Graphics::ViewportRect& getViewport()           const { return m_viewport; }
        inline Graphics::ViewportRect&       getViewport()                 { return m_viewport; }

        //----------------------------------------------------------------------
        // @Return:
        //  Target texture in which this camera renders. Nullptr if rendering to screen.
        //----------------------------------------------------------------------
        inline RenderTexturePtr getRenderTarget() { return m_renderTarget; }

        //----------------------------------------------------------------------
        // Set the render target in which this camera renders. Nullptr means the camera should
        // render to the screen. The viewport will be adapted to cover the whole texture.
        //----------------------------------------------------------------------
        void setRenderTarget(RenderTexturePtr renderTarget) { m_renderTarget = renderTarget; }

        //----------------------------------------------------------------------
        //const DirectX::XMMATRIX& getProjectionMatrix() const { return m_projection; }
        DirectX::XMMATRIX   getProjectionMatrix() const;
        DirectX::XMMATRIX   getViewProjectionMatrix() const { return m_viewMatrix * getProjectionMatrix(); }

        void setViewMatrix(const DirectX::XMMATRIX& view) { m_viewMatrix = view; }
        const DirectX::XMMATRIX& getViewMatrix() const { return m_viewMatrix; }

    private:
        CameraMode m_cameraMode = CameraMode::Perspective;

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
        CameraClearMode         m_clearMode     = CameraClearMode::Color;
        Color                   m_clearColor    = Color::BLACK;

        // Target render texture (nullptr means camera renders to screen)
        RenderTexturePtr        m_renderTarget = nullptr;

        // Matrices
        DirectX::XMMATRIX       m_viewMatrix;

        //----------------------------------------------------------------------
        //void _UpdateProjectionMatrix();

        //----------------------------------------------------------------------
        Camera(const Camera& other)               = delete;
        Camera& operator = (const Camera& other)  = delete;
        Camera(Camera&& other)                    = delete;
        Camera& operator = (Camera&& other)       = delete;
    };

}