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
    public:
        Camera(F32 fovAngleYInDegree = 45.0f, F32 zNear = 0.1f, F32 zFar = 1000.0f);
        Camera(F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        ~Camera() = default;

        //----------------------------------------------------------------------
        bool                            isRenderingToScreen()   const { return m_camera.isRenderingToScreen(); }
        const Graphics::CameraMode      getCameraMode()         const { return m_camera.getCameraMode(); }
        F32                             getZNear()              const { return m_camera.getZNear(); }
        F32                             getZFar()               const { return m_camera.getZFar(); }
        F32                             getFOV()                const { return m_camera.getFOV(); }
        bool                            isOrthographic()        const { return getCameraMode() == Graphics::CameraMode::Orthographic; }
        const Color&                    getClearColor()         const { return m_camera.getClearColor(); }
        const Graphics::CameraClearMode getClearMode()          const { return m_camera.getClearMode(); }
        F32                             getLeft()               const { return m_camera.getLeft(); }
        F32                             getRight()              const { return m_camera.getRight(); }
        F32                             getTop()                const { return m_camera.getTop(); }
        F32                             getBottom()             const { return m_camera.getBottom(); }
        F32                             getAspectRatio()        const { return m_camera.getAspectRatio(); }
        Common::BitMask                 getLayerMask()          const { return m_cullingMask; }

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

        //----------------------------------------------------------------------
        // @Return
        // Returns the viewport in which this camera renders.
        // Note that the viewport is in normalized coordinates [0-1].
        //----------------------------------------------------------------------
        const Graphics::ViewportRect&   getViewport()           const { return m_camera.getViewport(); }
        Graphics::ViewportRect&         getViewport()                 { return m_camera.getViewport(); }

        //----------------------------------------------------------------------
        // @Return:
        //  Target texture in which this camera renders. Nullptr if rendering to screen.
        //----------------------------------------------------------------------
        RenderTexturePtr                getRenderTarget() { return m_camera.getRenderTarget(); }

        //----------------------------------------------------------------------
        // Set the render target in which this camera renders. Nullptr means the camera should
        // render to the screen. The viewport will be adapted to cover the whole texture.
        //----------------------------------------------------------------------
        void setRenderTarget(RenderTexturePtr renderTarget) { m_camera.setRenderTarget(renderTarget); }

        //----------------------------------------------------------------------
        // Add additional command buffer to this camera
        //----------------------------------------------------------------------
        void addCommandBuffer(Graphics::CommandBuffer* cmd) { m_additionalCommandBuffers.push_back(cmd); }

        //----------------------------------------------------------------------
        // Remove a command buffer from this camera
        //----------------------------------------------------------------------
        void removeCommandBuffer(Graphics::CommandBuffer* cmd) { m_additionalCommandBuffers.erase( std::remove( m_additionalCommandBuffers.begin(), m_additionalCommandBuffers.end(), cmd ) ); }

        //----------------------------------------------------------------------
        DirectX::XMMATRIX getProjectionMatrix()       const { return m_camera.getProjectionMatrix(); }
        DirectX::XMMATRIX getViewProjectionMatrix()   const { return m_camera.getViewProjectionMatrix(); }


    private:
        Graphics::Camera        m_camera;

        // Which layer the camera should render
        Common::BitMask         m_cullingMask;

        // Contains commands to render one frame by one camera
        Graphics::CommandBuffer m_commandBuffer;

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
        Camera(const Camera& other)               = delete;
        Camera& operator = (const Camera& other)  = delete;
        Camera(Camera&& other)                    = delete;
        Camera& operator = (Camera&& other)       = delete;
    };

}