#include "camera.h"
/**********************************************************************
    class: Camera (camera.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

namespace Components {

    //----------------------------------------------------------------------
    Camera::Camera( float fovAngleYInDegree, float zNear, float zFar )
    {
        m_cameraMode = EMode::PERSPECTIVE;
        setPerspectiveParams( fovAngleYInDegree, zNear, zFar );
    }

    //----------------------------------------------------------------------
    Camera::Camera( float left, float right, float bottom, float top, float zNear, float zFar )
    {
        m_cameraMode = EMode::ORTHOGRAPHIC;
        setOrthoParams( left, right, bottom, top, zNear, zFar );
    }

    //----------------------------------------------------------------------
    void Camera::Tick( Time::Seconds delta )
    {
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::setOrthoParams( float left, float right, float bottom, float top, float zNear, float zFar )
    {
        m_left = left; m_right = right; m_bottom = bottom; m_top = top; m_zNear = zNear; m_zFar = zFar;
        _UpdateProjectionMatrix();
    }

    //----------------------------------------------------------------------
    void Camera::setPerspectiveParams( float fovAngleYInDegree, float zNear, float zFar )
    {
        m_fov = fovAngleYInDegree; m_zNear = zNear; m_zFar = zFar;
        _UpdateProjectionMatrix();
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::_UpdateProjectionMatrix()
    {
        switch (m_cameraMode)
        {
            case EMode::PERSPECTIVE:
            {
                float ar = 1.0f;
                m_projection = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians( m_fov ), ar, m_zNear, m_zFar );
            }
            case EMode::ORTHOGRAPHIC:
            {
                m_projection = DirectX::XMMatrixOrthographicOffCenterLH( m_left, m_right, m_bottom, m_top, m_zNear, m_zFar );
            }
        }
    }

}