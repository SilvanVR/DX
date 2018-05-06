#include "camera.h"
/**********************************************************************
    class: Camera (camera.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "i_render_component.hpp"
#include "Math/math_utils.h"
#include "Core/locator.h"
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
        Transform* transform = getGameObject()->getTransform();
        auto modelMatrix = transform->getWorldMatrix( lerp );
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
        Graphics::CommandBuffer tmpBuffer;
        for ( auto& renderer : rendererComponents )
        {
            if ( not renderer->isActive() )
                continue;

            // Check if component is visible and if layer matches
            bool isVisible = renderer->cull( *this );
            bool layerMatch = ( m_cullingMask & renderer->getGameObject()->getLayers() ).isAnyBitSet();
            if (isVisible && layerMatch)
                renderer->recordGraphicsCommands( tmpBuffer, lerp );
        }

        for (auto& additionalCmd : m_additionalCommandBuffers)
            tmpBuffer.merge(*additionalCmd);

        _SortRenderCommands( tmpBuffer, transform->position );

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
        return DirectX::XMMatrixIdentity();
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

    struct RenderQueueSort
    {
        bool operator()(const MaterialPtr& lhs, const MaterialPtr& rhs) const
        {
            return lhs->getShader()->getRenderQueue() < rhs->getShader()->getRenderQueue();
        }
    };

    //----------------------------------------------------------------------
    void Camera::_SortRenderCommands( const Graphics::CommandBuffer& cmd, const Math::Vec3& position )
    {
        // Sort rendering commands by material
        std::map<MaterialPtr, ArrayList<std::shared_ptr<Graphics::GPUC_DrawMesh>>, RenderQueueSort> sortByMaterials;
        for (auto& command : cmd.getGPUCommands())
        {
            switch (command->getType())
            {
            case Graphics::GPUCommand::DRAW_MESH:
            {
                auto c = std::dynamic_pointer_cast<Graphics::GPUC_DrawMesh>( command );
                sortByMaterials[c->material].push_back( c );
                break;
            }
            }
        }

        // Now sort drawcalls by camera distance
        ArrayList<std::shared_ptr<Graphics::GPUC_DrawMesh>> transparentDrawcalls;
        for (auto& pair : sortByMaterials)
        {
            auto& material = pair.first;
            auto& drawCalls = pair.second;

            // Sort by back-to-front if renderqueue > 3000
            if (material->getShader()->getRenderQueue() >= (I32)Graphics::RenderQueue::BackToFrontBoundary)
            {
                // Must be inserted in a separate list, because ALL transparent drawcalls have to be sorted
                transparentDrawcalls.insert( transparentDrawcalls.end(), drawCalls.begin(), drawCalls.end() );
            }
            else
            {
                // Front-To-Back if desired.

                // Now add drawcall to real command buffer
                for (auto& drawCall : drawCalls)
                    m_commandBuffer.getGPUCommands().push_back( drawCall );
            }
        }

        // Sort all transparent draw-calls back-to-front
        std::sort( transparentDrawcalls.begin(), transparentDrawcalls.end(), [=](const std::shared_ptr<Graphics::GPUC_DrawMesh>& d1, const std::shared_ptr<Graphics::GPUC_DrawMesh>& d2) {
            auto pos = d1->modelMatrix.r[3];
            auto pos2 = d2->modelMatrix.r[3];

            auto camPos = DirectX::XMLoadFloat3( &position );
            auto distance1 = DirectX::XMVector4LengthSq( DirectX::XMVectorSubtract( camPos, pos ) );
            auto distance2 = DirectX::XMVector4LengthSq( DirectX::XMVectorSubtract( camPos, pos2 ) );

            return DirectX::XMVector4Greater( distance1, distance2 );
        } );

        // Add drawcalls to command buffer
        for (auto& drawCall : transparentDrawcalls)
            m_commandBuffer.getGPUCommands().push_back( drawCall );
    }

}