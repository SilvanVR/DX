#pragma once
/**********************************************************************
    class: Camera (camera.h)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "../i_component.h"
#include "Graphics/command_buffer.h"
#include "Graphics/camera.h"
#include "GameplayLayer/layers.hpp"

namespace Core { class RenderSystem; }
class IScene;

namespace Components {

    //----------------------------------------------------------------------
    enum class CameraEvent
    {
        Geometry = 0, // Commands corresponding to this event will be sorted with every render command in scene
        PostProcess,
        Overlay,
        MAX_EVENTS
    };

    //**********************************************************************
    class Camera : public IComponent
    {
        static const Graphics::MSAASamples DEFAULT_MSAA_SAMPLES = Graphics::MSAASamples::Four;
    public:
        Camera(const RenderTexturePtr& rt, F32 fovAngleYInDegree = 45.0f, F32 zNear = 0.1f, F32 zFar = 1000.0f );
        Camera(F32 fovAngleYInDegree = 45.0f, F32 zNear = 0.1f, F32 zFar = 1000.0f, Graphics::MSAASamples numSamples = DEFAULT_MSAA_SAMPLES, bool hdr = false);
        Camera(F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar, Graphics::MSAASamples numSamples = DEFAULT_MSAA_SAMPLES, bool hdr = false);
        ~Camera() = default;

        //----------------------------------------------------------------------
        bool                            isBlittingToScreen()        const { return m_camera.isBlittingToScreen(); }
        const Graphics::CameraMode      getCameraMode()             const { return m_camera.getCameraMode(); }
        F32                             getZNear()                  const { return m_camera.getZNear(); }
        F32                             getZFar()                   const { return m_camera.getZFar(); }
        F32                             getFOV()                    const { return m_camera.getFOV(); }
        bool                            isOrthographic()            const { return getCameraMode() == Graphics::CameraMode::Orthographic; }
        const Color&                    getClearColor()             const { return m_camera.getClearColor(); }
        const Graphics::CameraClearMode getClearMode()              const { return m_camera.getClearMode(); }
        F32                             getAspectRatio()            const { return m_camera.getAspectRatio(); }
        const LayerMask                 getCullingMask()            const { return m_cullingMask; }
        const DirectX::XMMATRIX&        getProjectionMatrix()       const { return m_camera.getProjectionMatrix(); }
        const DirectX::XMMATRIX&        getViewProjectionMatrix()   const { return m_camera.getViewProjectionMatrix(); }
        const Graphics::Camera&         getNativeCamera()           const { return m_camera; }
        F32                             getLeft()                   const { return m_camera.getLeft(); }
        F32                             getRight()                  const { return m_camera.getRight(); }
        F32                             getTop()                    const { return m_camera.getTop(); }
        F32                             getBottom()                 const { return m_camera.getBottom(); }
        const Graphics::FrameInfo&      getFrameInfo()              const { return m_camera.getFrameInfo(); }
        bool                            isBlittingToHMD()           const { return m_camera.isBlittingToHMD(); }
        Graphics::VR::Eye               getHMDEye()                 const { return m_camera.getHMDEye(); }
        bool                            isHDR()                     const;

        //----------------------------------------------------------------------
        void setCameraMode          (Graphics::CameraMode mode)                                     { m_camera.setCameraMode(mode); }
        void setZNear               (F32 zNear)                                                     { m_camera.setZNear(zNear); }
        void setZFar                (F32 zFar)                                                      { m_camera.setZFar(zFar); }
        void setFOV                 (F32 fovAngleYInDegree)                                         { m_camera.setFOV(fovAngleYInDegree); }
        void setClearColor          (const Color& clearColor)                                       { m_camera.setClearColor(clearColor); }
        void setClearMode           (Graphics::CameraClearMode clearMode)                           { m_camera.setClearMode(clearMode); }
        void setViewport            (const Graphics::ViewportRect& viewport)                        { m_camera.setViewport(viewport); }
        void setCullingMask         (LayerMask cullingMask)                                         { m_cullingMask = cullingMask; }
        void setOrthoParams         (F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar) { m_camera.setOrthoParams(left, right, bottom, top, zNear, zFar); }
        void setPerspectiveParams   (F32 fovAngleYInDegree, F32 zNear, F32 zFar)                    { m_camera.setPerspectiveParams(fovAngleYInDegree, zNear, zFar); }
        void setMultiSamples        (Graphics::MSAASamples sampleCount)                             { m_camera.getRenderTarget()->recreate(sampleCount); }
        void setCameraFlags         (Graphics::CameraFlags flags)                                   { m_camera.setCameraFlags(flags); }
        void setProjection          (const DirectX::XMMATRIX& projection)                           { m_camera.setProjection(projection); }
        void setSuperSampling       (F32 screenResMod);
        void setHDRRendering        (bool enabled);

        //----------------------------------------------------------------------
        // @Return
        // Returns the viewport in which this camera renders.
        // Note that the viewport is in normalized coordinates [0-1].
        //----------------------------------------------------------------------
        const Graphics::ViewportRect&   getViewport() const { return m_camera.getViewport(); }
        Graphics::ViewportRect&         getViewport()       { return m_camera.getViewport(); }

        //----------------------------------------------------------------------
        // @Return:
        //  Target texture in which this camera renders.
        //----------------------------------------------------------------------
        const RenderTexturePtr& getRenderTarget() { return m_camera.getRenderTarget(); }

        //----------------------------------------------------------------------
        // Set the render target in which this camera renders.
        // @Params:
        //  "renderTarget": The target in which this camera renders.
        //  "flags": Flags which specify what should be done at the end with the render-target.
        //----------------------------------------------------------------------
        void setRenderTarget(RenderTexturePtr renderTarget, Graphics::CameraFlags flags = Graphics::CameraFlags::None) { m_camera.setRenderTarget(renderTarget, flags); }

        //----------------------------------------------------------------------
        // Add an additional command buffer to this camera
        //----------------------------------------------------------------------
        void addCommandBuffer(Graphics::CommandBuffer* cmd, CameraEvent evt = CameraEvent::Geometry);
        void removeCommandBuffer(Graphics::CommandBuffer* cmd);

    private:
        Graphics::Camera            m_camera;

        // Which layer the camera should render
        LayerMask                   m_cullingMask;

        // Additional attached command buffer
        HashMap<CameraEvent, ArrayList<Graphics::CommandBuffer*>> m_additionalCommandBuffers;

        friend class Core::RenderSystem;

        //----------------------------------------------------------------------
        void _CreateRenderTarget(Graphics::MSAASamples sampleCount, bool hdr);

        NULL_COPY_AND_ASSIGN(Camera)
    };

}