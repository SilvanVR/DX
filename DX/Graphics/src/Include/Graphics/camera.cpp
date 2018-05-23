#include "camera.h"
/**********************************************************************
    class: Camera (camera.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "command_buffer.h"
#include "i_renderer.h"

namespace Graphics {

    //----------------------------------------------------------------------
    Camera::Camera( F32 fovAngleYInDegree, F32 zNear, F32 zFar )
    {
        m_cameraMode = CameraMode::Perspective;
        setPerspectiveParams( fovAngleYInDegree, zNear, zFar );
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
    {
        m_cameraMode = CameraMode::Orthographic;
        setOrthoParams( left, right, bottom, top, zNear, zFar );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::setOrthoParams( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
    {
        m_ortho.left = left; m_ortho.right = right; m_ortho.bottom = bottom; m_ortho.top = top; m_zNear = zNear; m_zFar = zFar;
    }

    //----------------------------------------------------------------------
    void Camera::setPerspectiveParams( F32 fovAngleYInDegree, F32 zNear, F32 zFar )
    {
        m_fov = fovAngleYInDegree; m_zNear = zNear; m_zFar = zFar;
    }

    //----------------------------------------------------------------------
    F32 Camera::getAspectRatio() const
    {
        return isRenderingToScreen() ? IRenderer::GetWindow()->getAspectRatio() : m_renderTarget->getAspectRatio();
    }

    //----------------------------------------------------------------------
    void Camera::setModelMatrix( const DirectX::XMMATRIX& model )
    { 
        m_model = model; 
        m_view = DirectX::XMMatrixInverse( nullptr, m_model );
        _UpdateProjectionMatrix();
        m_viewProjection = m_view * m_projection;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::_UpdateProjectionMatrix()
    {
        switch (m_cameraMode)
        {
        case CameraMode::Perspective:
            m_projection = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians( m_fov ), getAspectRatio(), m_zNear, m_zFar );
            break;
        case CameraMode::Orthographic:
            m_projection = DirectX::XMMatrixOrthographicOffCenterLH( m_ortho.left, m_ortho.right, m_ortho.bottom, m_ortho.top, m_zNear, m_zFar );
            break;
        default:
            ASSERT( false && "Invalid camera mode" );
        }
    }

}