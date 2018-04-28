#include "camera.h"
/**********************************************************************
    class: Camera (camera.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "GameplayLayer/Components/transform.h"
#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "i_render_component.hpp"
#include "Math/math_utils.h"
#include "locator.h"
#include "Core/layer_defines.hpp"

namespace Components {

    //----------------------------------------------------------------------
    Camera::Camera( F32 fovAngleYInDegree, F32 zNear, F32 zFar )
        : m_cullingMask( LAYER_ALL )
    {
        m_cameraMode = EMode::PERSPECTIVE;
        setPerspectiveParams( fovAngleYInDegree, zNear, zFar );
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
        : m_cullingMask( LAYER_ALL )
    {
        m_cameraMode = EMode::ORTHOGRAPHIC;
        setOrthoParams( left, right, bottom, top, zNear, zFar );
    }

    //----------------------------------------------------------------------
    Camera::~Camera()
    {
    }

    //----------------------------------------------------------------------
    Graphics::CommandBuffer& Camera::recordGraphicsCommands( F32 lerp, const ArrayList<IRenderComponent*>& rendererComponents )
    {
        // Reset command buffer
        m_commandBuffer.reset();

        // Set render target
        m_commandBuffer.setRenderTarget( getRenderTarget() );

        // Clear render target if desired
        switch (m_clearMode)
        {
        case EClearMode::NONE: break;
        case EClearMode::COLOR: m_commandBuffer.clearRenderTarget( getClearColor() ); break;
        default: LOG_WARN( "Unknown Clear-Mode in camera!" );
        }

        // Set viewport (Translate to pixel coordinates)
        Graphics::ViewportRect screenRect;
        if ( isRenderingToScreen() )
        {
            screenRect.topLeftX = m_viewport.topLeftX * Locator::getWindow().getWidth();
            screenRect.topLeftY = m_viewport.topLeftY * Locator::getWindow().getHeight();
            screenRect.width    = m_viewport.width    * Locator::getWindow().getWidth();
            screenRect.height   = m_viewport.height   * Locator::getWindow().getHeight();
        }
        else
        {
            screenRect.topLeftX = m_viewport.topLeftX * m_renderTarget->getWidth();
            screenRect.topLeftY = m_viewport.topLeftY * m_renderTarget->getHeight();
            screenRect.width    = m_viewport.width    * m_renderTarget->getWidth();
            screenRect.height   = m_viewport.height   * m_renderTarget->getHeight();
        }
        m_commandBuffer.setViewport( screenRect );

        // Set view / projection params
        Transform* transform = getGameObject()->getComponent<Components::Transform>();
        auto modelMatrix = transform->getTransformationMatrix( lerp );
        auto view = DirectX::XMMatrixInverse( nullptr, modelMatrix );
        auto projection = getProjectionMatrix();
        m_viewProjection = view * projection;

        switch ( m_cameraMode )
        {
        case Camera::PERSPECTIVE:
            m_commandBuffer.setCameraPerspective( view, getFOV(), getZNear(), getZFar() ); break;
        case Camera::ORTHOGRAPHIC:
            m_commandBuffer.setCameraOrtho( view, getLeft(), getRight(), getBottom(), getTop(), getZNear(), getZFar() ); break;
        default:
            LOG_WARN_RENDERING( "UNKNOWN CAMERA MODE" );
        }

        // Record commands for every rendering component
        for ( auto& renderer : rendererComponents )
        {
            if ( not renderer->isActive() )
                continue;

            // Check if component is visible and if layer matches
            bool isVisible = renderer->cull( *this );
            bool layerMatch = ( m_cullingMask & renderer->getGameObject()->getLayers() ).isAnyBitSet();
            if (isVisible && layerMatch)
                renderer->recordGraphicsCommands( m_commandBuffer, lerp );
        }

        return m_commandBuffer;
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
        return isRenderingToScreen() ? Locator::getWindow().getAspectRatio() : m_renderTarget->getAspectRatio();
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    DirectX::XMMATRIX Camera::getProjectionMatrix() const
    {
        switch (m_cameraMode)
        {
        case EMode::PERSPECTIVE:
            return DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians( m_fov ), getAspectRatio(), m_zNear, m_zFar );
        case EMode::ORTHOGRAPHIC:
            return DirectX::XMMatrixOrthographicOffCenterLH( m_ortho.left, m_ortho.right, m_ortho.bottom, m_ortho.top, m_zNear, m_zFar );
        }
    }

    ////----------------------------------------------------------------------
    //void Camera::_UpdateProjectionMatrix()
    //{
    //    switch (m_cameraMode)
    //    {
    //    case EMode::PERSPECTIVE:
    //        m_projection = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians( m_fov ), getAspectRatio(), m_zNear, m_zFar );
    //        break;
    //    case EMode::ORTHOGRAPHIC:
    //        m_projection = DirectX::XMMatrixOrthographicOffCenterLH( m_ortho.left, m_ortho.right, m_ortho.bottom, m_ortho.top, m_zNear, m_zFar );
    //        break;
    //    }
    //}


}