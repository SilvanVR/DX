#pragma once
/**********************************************************************
    class: Camera (camera.h)

    author: S. Hau
    date: March 4, 2018

**********************************************************************/

#include "../i_component.h"
#include "Common/bit_mask.hpp"

namespace Graphics { class RenderTexture; }

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

        Camera(F32 fovAngleYInDegree = 45.0f, F32 zNear = 0.1f, F32 zFar = 1000.0f);
        Camera(F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        ~Camera();

        //----------------------------------------------------------------------
        void setCameraMode          (Camera::EMode mode)        { m_cameraMode = mode; }
        void setZNear               (F32 zNear)                 { m_zNear = zNear; }
        void setZFar                (F32 zFar)                  { m_zFar = zFar; }
        void setFOV                 (F32 fovAngleYInDegree)     { m_fov = fovAngleYInDegree; }
        void setClearColor          (const Color& clearColor)   { m_clearColor = clearColor; }
        void setOrthoParams         (F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        void setPerspectiveParams   (F32 fovAngleYInDegree, F32 zNear, F32 zFar);

        //----------------------------------------------------------------------
        const EMode&        getCameraMode()     const { return m_cameraMode; }
        F32                 getZNear()          const { return m_zNear; }
        F32                 getZFar()           const { return m_zFar; }
        F32                 getFOV()            const { return m_fov; }
        bool                isOrthographic()    const { return m_cameraMode == ORTHOGRAPHIC; }
        const Color&        getClearColor()     const { return m_clearColor; }
        F32                 getLeft()           const { return m_left; }
        F32                 getRight()          const { return m_right; }
        F32                 getTop()            const { return m_top; }
        F32                 getBottom()         const { return m_bottom; }
        //F32                 getAspecRatio()     const;


        //----------------------------------------------------------------------
        // @Return:
        //  Target texture in which this camera renders. Nullptr if rendering to screen.
        //----------------------------------------------------------------------
        Graphics::RenderTexture* getRenderTarget(){ return m_renderTarget; }

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;

        //----------------------------------------------------------------------
        // const DirectX::XMMATRIX& getProjectionMatrix() const { return m_projection; }

    private:
        EMode                           m_cameraMode;
        //DirectX::XMMATRIX               m_projection;

        // Z Clipping Plane
        F32 m_zNear       = 0.1f;
        F32 m_zFar        = 1000.0f;

        // Perspective Params
        F32 m_fov         = 45.0f;

        // Ortographics Params
        F32 m_left        = -1.0f;
        F32 m_right       = 1.0f;
        F32 m_bottom      = -1.0f;
        F32 m_top         = 1.0f;

        // Clear color for this camera
        Color m_clearColor = Color::BLACK;

        // Target render texture (nullptr means render to screen)
        Graphics::RenderTexture* m_renderTarget = nullptr;

        // Which layer this camera renders
        //Common::BitMask               m_layerMask; 

        //----------------------------------------------------------------------
        Camera(const Camera& other)               = delete;
        Camera& operator = (const Camera& other)  = delete;
        Camera(Camera&& other)                    = delete;
        Camera& operator = (Camera&& other)       = delete;
    };

}