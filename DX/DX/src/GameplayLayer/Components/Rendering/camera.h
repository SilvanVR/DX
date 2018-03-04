#pragma once
/**********************************************************************
    class: Camera (camera.h)

    author: S. Hau
    date: March 4, 2018

**********************************************************************/

#include "../i_component.h"
#include "Common/bit_mask.hpp"

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

        Camera(float fovAngleYInDegree = 45.0f, float zNear = 0.1f, float zFar = 1000.0f);
        Camera(float left, float right, float bottom, float top, float zNear, float zFar);

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void Tick(Time::Seconds delta) override;

        //----------------------------------------------------------------------
        void setCameraMode          (Camera::EMode mode)        { m_cameraMode = mode; _UpdateProjectionMatrix(); }
        void setZNear               (float zNear)               { m_zNear = zNear; _UpdateProjectionMatrix(); }
        void setZFar                (float zFar)                { m_zFar = zFar; _UpdateProjectionMatrix(); }
        void setFOV                 (float fovAngleYInDegree)   { m_fov = fovAngleYInDegree; _UpdateProjectionMatrix(); }
        void setClearColor          (const Color& clearColor)   { m_clearColor = clearColor; }
        void setOrthoParams         (float left, float right, float bottom, float top, float zNear, float zFar);
        void setPerspectiveParams   (float fovAngleYInDegree, float zNear, float zFar);

        //----------------------------------------------------------------------
        const EMode&        getCameraMode()     const { return m_cameraMode; }
        float               getZNear()          const { return m_zNear; }
        float               getZFar()           const { return m_zFar; }
        float               getFOV()            const { return m_fov; }
        bool                isOrthographic()    const { return m_cameraMode == ORTHOGRAPHIC; }
        const Color&        getClearColor()     const { return m_clearColor; }

        //----------------------------------------------------------------------
        const DirectX::XMMATRIX& getProjectionMatrix() const { return m_projection; }

    private:
        EMode                           m_cameraMode;
        DirectX::XMMATRIX               m_projection;

        // Z Clipping Plane
        float m_zNear       = 0.1f;
        float m_zFar        = 1000.0f;

        // Perspective Params
        float m_fov         = 45.0f;

        // Ortographics Params
        float m_left        = -1.0f;
        float m_right       = 1.0f;
        float m_bottom      = -1.0f;
        float m_top         = 1.0f;

        // Clear color for this camera
        Color m_clearColor = Color::BLACK;

        // Target render texture (nullptr means render to screen)
        // Graphics::RenderTexture* m_renderTarget = nullptr;

        // Which layer this camera renders
        //Common::BitMask               m_layerMask; 


        //----------------------------------------------------------------------
        void _UpdateProjectionMatrix();

        //----------------------------------------------------------------------
        Camera(const Camera& other)               = delete;
        Camera& operator = (const Camera& other)  = delete;
        Camera(Camera&& other)                    = delete;
        Camera& operator = (Camera&& other)       = delete;
    };

}