#pragma once
/**********************************************************************
    class: Camera (camera.h)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "../i_component.h"
#include "Common/bit_mask.hpp"
#include "Graphics/command_buffer.h"
#include "Graphics/camera.h"

namespace Core { class CoreEngine; }

namespace Components {

    class IRenderComponent;

    //**********************************************************************
    class Camera : public IComponent
    {
        static const Graphics::MSAASamples DEFAULT_MSAA_SAMPLES = Graphics::MSAASamples::Eight;
    public:
        Camera(const RenderTexturePtr& rt, F32 fovAngleYInDegree = 45.0f, F32 zNear = 0.1f, F32 zFar = 1000.0f );
        Camera(F32 fovAngleYInDegree = 45.0f, F32 zNear = 0.1f, F32 zFar = 1000.0f, Graphics::MSAASamples numSamples = DEFAULT_MSAA_SAMPLES, bool hdr = false);
        Camera(F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar, Graphics::MSAASamples numSamples = DEFAULT_MSAA_SAMPLES, bool hdr = false);
        ~Camera() = default;

        //----------------------------------------------------------------------
        bool                            isRenderingToScreen()       const { return m_camera.isRenderingToScreen(); }
        const Graphics::CameraMode      getCameraMode()             const { return m_camera.getCameraMode(); }
        F32                             getZNear()                  const { return m_camera.getZNear(); }
        F32                             getZFar()                   const { return m_camera.getZFar(); }
        F32                             getFOV()                    const { return m_camera.getFOV(); }
        bool                            isOrthographic()            const { return getCameraMode() == Graphics::CameraMode::Orthographic; }
        const Color&                    getClearColor()             const { return m_camera.getClearColor(); }
        const Graphics::CameraClearMode getClearMode()              const { return m_camera.getClearMode(); }
        F32                             getAspectRatio()            const { return m_camera.getAspectRatio(); }
        Common::BitMask                 getCullingMask()            const { return m_cullingMask; }
        const DirectX::XMMATRIX&        getProjectionMatrix()       const { return m_camera.getProjectionMatrix(); }
        const DirectX::XMMATRIX&        getViewProjectionMatrix()   const { return m_camera.getViewProjectionMatrix(); }

        //----------------------------------------------------------------------
        void setCameraMode          (Graphics::CameraMode mode)                                     { m_camera.setCameraMode(mode); }
        void setZNear               (F32 zNear)                                                     { m_camera.setZNear(zNear); }
        void setZFar                (F32 zFar)                                                      { m_camera.setZFar(zFar); }
        void setFOV                 (F32 fovAngleYInDegree)                                         { m_camera.setFOV(fovAngleYInDegree); }
        void setClearColor          (const Color& clearColor)                                       { m_camera.setClearColor(clearColor); }
        void setClearMode           (Graphics::CameraClearMode clearMode)                           { m_camera.setClearMode(clearMode); }
        void setViewport            (const Graphics::ViewportRect& viewport)                        { m_camera.setViewport(viewport); }
        void setCullingMask         (U32 layers)                                                    { m_cullingMask.unsetAnyBit(); m_cullingMask.setBits(layers); }
        void setOrthoParams         (F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar) { m_camera.setOrthoParams(left, right, bottom, top, zNear, zFar); }
        void setPerspectiveParams   (F32 fovAngleYInDegree, F32 zNear, F32 zFar)                    { m_camera.setPerspectiveParams(fovAngleYInDegree, zNear, zFar); }
        void setRenderingToScreen   (bool renderToScreen)                                           { m_camera.setRenderingToScreen(renderToScreen); }
        void setMultiSamples        (Graphics::MSAASamples sampleCount)                             { m_camera.getRenderTarget()->recreate({(U32)sampleCount}); }
        void setSuperSampling       (F32 screenResMod);
        void setHDRRendering        (bool enabled);

        //----------------------------------------------------------------------
        // @Return
        // Returns the viewport in which this camera renders.
        // Note that the viewport is in normalized coordinates [0-1].
        //----------------------------------------------------------------------
        const Graphics::ViewportRect&   getViewport()           const { return m_camera.getViewport(); }
        Graphics::ViewportRect&         getViewport()                 { return m_camera.getViewport(); }

        //----------------------------------------------------------------------
        // @Return:
        //  Target texture in which this camera renders.
        //----------------------------------------------------------------------
        RenderTexturePtr                getRenderTarget() { return m_camera.getRenderTarget(); }

        //----------------------------------------------------------------------
        // Set the render target in which this camera renders.
        // @Params:
        //  "renderTarget": The target in which this camera renders.
        //  "renderToScreen": If true the rendertarget will be copied to the screen afterwards.
        //----------------------------------------------------------------------
        void setRenderTarget(RenderTexturePtr renderTarget, bool renderToScreen = false) { m_camera.setRenderTarget(renderTarget, renderToScreen); }

        //----------------------------------------------------------------------
        // Add an additional command buffer to this camera
        //----------------------------------------------------------------------
        void addCommandBuffer(Graphics::CommandBuffer* cmd) { m_additionalCommandBuffers.push_back(cmd); }

        //----------------------------------------------------------------------
        void removeCommandBuffer(Graphics::CommandBuffer* cmd) { m_additionalCommandBuffers.erase(std::remove(m_additionalCommandBuffers.begin(), m_additionalCommandBuffers.end(), cmd)); }

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

    private:
        Graphics::Camera            m_camera;

        // Which layer the camera should render
        Common::BitMask             m_cullingMask;

        // Whether this camera renders into a floating point buffer for HDR rendering or not
        bool                        m_hdr = false;

        // Culling planes
        enum side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };
        std::array<Math::Vec4, 6>   m_planes;

        // Contains commands to render one frame by one camera
        Graphics::CommandBuffer     m_commandBuffer;

        // Additional attached command buffer
        ArrayList<Graphics::CommandBuffer*> m_additionalCommandBuffers;

        //----------------------------------------------------------------------
        friend class Core::CoreEngine;
        Graphics::CommandBuffer& recordGraphicsCommands(F32 lerp, const ArrayList<IRenderComponent*>& rendererComponents);

        //----------------------------------------------------------------------
        // Sorts all rendering commands from the given command buffer by material, renderqueue and camera distance
        //----------------------------------------------------------------------
        inline void _SortRenderCommands(const Graphics::CommandBuffer& cmd, const Math::Vec3& position);

        //----------------------------------------------------------------------
        void _UpdateCullingPlanes(const DirectX::XMMATRIX& viewProjection);
        void _CreateRenderTarget(Graphics::MSAASamples sampleCount);

        NULL_COPY_AND_ASSIGN(Camera)
    };

}