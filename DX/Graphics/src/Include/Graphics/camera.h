#pragma once
/**********************************************************************
    class: Camera (camera.h)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "forward_declarations.hpp"
#include "i_render_texture.h"
#include "structs.hpp"
#include "Math/aabb.h"

namespace Graphics {

    #define TAG_SHADOW_PASS         SID("_ShadowPass")
    #define TAG_SHADOW_PASS_LINEAR  SID("_ShadowPassLinear")

    //----------------------------------------------------------------------
    enum class CameraClearMode
    {
        None,  // Clear nothing (load prev contents)
        Color, // Clear color + depth
        Depth  // Clear only color
    };

    //----------------------------------------------------------------------
    enum class CameraMode
    {
        Custom,
        Perspective,
        Orthographic
    };

    //----------------------------------------------------------------------
    enum class CameraFlags
    {
        None                    = 0,        // Do nothing
        BlitToScreen            = 1 << 0,   // Final rt from camera will be blit to screen
        BlitToLeftEye           = 1 << 1,   // Final rt from camera will be blit to left eye
        BlitToRightEye          = 1 << 2,   // Final rt from camera will be blit to right eye
        BlitToScreenAndLeftEye  = 1 << 3,   // Blit to screen and left eye
        BlitToScreenAndRightEye = 1 << 4,   // Blit to screen and right eye
    };

    //----------------------------------------------------------------------
    enum class CameraMember // Possible members in the camera-buffer on the gpu.
    {
        Unknown = 0,
        View,
        Projection,
        Position,
        zNear,
        zFar,
        ProjectionInv,
        ViewInv,
        ViewProjection
    };

    //**********************************************************************
    class Camera
    {
    public:
        Camera(F32 fovAngleYInDegree = 45.0f, F32 zNear = 0.1f, F32 zFar = 1000.0f);
        Camera(F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        ~Camera() = default;

        //----------------------------------------------------------------------
        inline void setCameraMode           (CameraMode mode)                        { m_cameraMode = mode; }
        inline void setZNear                (F32 zNear)                              { m_zNear = zNear; }
        inline void setZFar                 (F32 zFar)                               { m_zFar = zFar; }
        inline void setFOV                  (F32 fovAngleYInDegree)                  { m_fov = fovAngleYInDegree; }
        inline void setClearColor           (const Color& clearColor)                { m_clearColor = clearColor; }
        inline void setClearMode            (CameraClearMode clearMode)              { m_clearMode = clearMode; }
        inline void setViewport             (const Graphics::ViewportRect& viewport) { m_viewport = viewport; }
        inline void setOrthoParams          (F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        inline void setPerspectiveParams    (F32 fovAngleYInDegree, F32 zNear, F32 zFar);
        inline void setCameraFlags          (CameraFlags flags)                      { m_cameraFlags = flags; }
        inline void setProjection           (const DirectX::XMMATRIX& projection)    { m_cameraMode = CameraMode::Custom; m_projection = projection; }

        //----------------------------------------------------------------------
        inline const CameraMode&   getCameraMode()              const { return m_cameraMode; }
        inline F32                 getZNear()                   const { return m_zNear; }
        inline F32                 getZFar()                    const { return m_zFar; }
        inline F32                 getFOV()                     const { return m_fov; }
        inline bool                isOrthographic()             const { return m_cameraMode == CameraMode::Orthographic; }
        inline const Color&        getClearColor()              const { return m_clearColor; }
        inline F32                 getLeft()                    const { return m_ortho.left; }
        inline F32                 getRight()                   const { return m_ortho.right; }
        inline F32                 getTop()                     const { return m_ortho.top; }
        inline F32                 getBottom()                  const { return m_ortho.bottom; }
        inline CameraClearMode     getClearMode()               const { return m_clearMode; }
        inline bool                isBlittingToScreen()         const { return m_cameraFlags == CameraFlags::BlitToScreen || m_cameraFlags == CameraFlags::BlitToScreenAndLeftEye || m_cameraFlags == CameraFlags::BlitToScreenAndRightEye; }
        inline bool                isBlittingToHMD()            const { return m_cameraFlags == CameraFlags::BlitToLeftEye || m_cameraFlags == CameraFlags::BlitToRightEye || m_cameraFlags == CameraFlags::BlitToScreenAndLeftEye || m_cameraFlags == CameraFlags::BlitToScreenAndRightEye; }
        inline const ShaderPtr&    getReplacementShader()       const { return m_replacementShader; }
        inline StringID            getReplacementShaderTag()    const { return m_replacementShaderTag; }
        inline bool                hasReplacementShader()       const { return m_replacementShader != nullptr; }
        inline const FrameInfo&    getFrameInfo()               const { return *m_frameInfo.get(); }
        inline FrameInfo&          getFrameInfo()                     { return *m_frameInfo.get(); }
        inline CameraFlags         getFlags()                   const { return m_cameraFlags; }
        inline F32                 getAspectRatio()             const;
        VR::Eye                    getHMDEye()                  const;

        //----------------------------------------------------------------------
        // @Return
        // Returns the viewport in which this camera renders.
        // Note that the viewport is in normalized coordinates [0-1].
        //----------------------------------------------------------------------
        inline const Graphics::ViewportRect& getViewport()           const { return m_viewport; }
        inline Graphics::ViewportRect&       getViewport()                 { return m_viewport; }

        //----------------------------------------------------------------------
        // @Return:
        //  Target texture in which this camera renders.
        //----------------------------------------------------------------------
        inline const RenderTexturePtr& getRenderTarget() const { return m_renderTarget; }

        //----------------------------------------------------------------------
        // Set the render target in which this camera renders.
        // @Params:
        //  "renderTarget": The target in which this camera renders.
        //  "cameraFlags": Camera flags. See the enum for a description.
        //----------------------------------------------------------------------
        void setRenderTarget(RenderTexturePtr renderTarget, CameraFlags cameraFlags = Graphics::CameraFlags::None) { m_renderTarget = renderTarget; m_cameraFlags = cameraFlags; }

        //----------------------------------------------------------------------
        inline const DirectX::XMMATRIX& getProjectionMatrix()      const { return m_projection; }
        inline const DirectX::XMMATRIX& getViewProjectionMatrix()  const { return m_viewProjection; }
        inline const DirectX::XMMATRIX& getViewMatrix()            const { return m_view; }
        inline const DirectX::XMMATRIX& getModelMatrix()           const { return m_model; }

        //----------------------------------------------------------------------
        // Set the model matrix for this camera.
        // This function also calculates the view + projection and view-projection matrix.
        // This function should be called once per frame.
        //----------------------------------------------------------------------
        void setModelMatrix(const DirectX::XMMATRIX& model);

        //----------------------------------------------------------------------
        // Set the view matrix for this camera.
        // This function also calculates the model + projection and view-projection matrix.
        // This function should be called once per frame.
        //----------------------------------------------------------------------
        void setViewMatrix(const DirectX::XMMATRIX& model);

        //----------------------------------------------------------------------
        // Cull the aabb with the given world matrix against this camera frustum.
        // @Return:
        //  True, when visible.
        //----------------------------------------------------------------------
        bool cull(const Math::AABB& aabb, const DirectX::XMMATRIX& modelMatrix) const;

        //----------------------------------------------------------------------
        // Cull the given sphere against this camera frustum.
        // @Return:
        //  True, when visible.
        //----------------------------------------------------------------------
        bool cull(const Math::Vec3& pos, F32 radius) const;

        //----------------------------------------------------------------------
        // Set the replacement shader with a given tag
        //----------------------------------------------------------------------
        void setReplacementShader(const ShaderPtr& shader, StringID tag){ m_replacementShader = shader; m_replacementShaderTag = tag; }

    private:
        // Matrices
        DirectX::XMMATRIX       m_model;
        DirectX::XMMATRIX       m_view;
        DirectX::XMMATRIX       m_projection;
        DirectX::XMMATRIX       m_viewProjection;

        std::shared_ptr<FrameInfo> m_frameInfo;

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

        // Target render texture
        RenderTexturePtr        m_renderTarget = nullptr;

        // Culling planes
        enum side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };
        std::array<Math::Vec4, 6> m_planes;

        // If not null, materials will use this shader if not overriden
        ShaderPtr m_replacementShader = nullptr;
        StringID m_replacementShaderTag;

        // Camera flags
        CameraFlags m_cameraFlags = CameraFlags::None;

        //----------------------------------------------------------------------
        void _UpdateProjectionMatrix();
        void _UpdateCullingPlanes(const DirectX::XMMATRIX& viewProjection);
    };

}