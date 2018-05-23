#include "camera.h"
/**********************************************************************
    class: Camera (camera.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "i_render_component.hpp"
#include "Math/math_utils.h"
#include "Core/locator.h"
#include "Core/layer_defines.hpp"

namespace Components {

    //----------------------------------------------------------------------
    Camera::Camera( F32 fovAngleYInDegree, F32 zNear, F32 zFar )
        : m_camera( fovAngleYInDegree, zNear, zFar ), m_cullingMask( LAYER_ALL )
    {
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
        : m_camera( left, right, bottom, top, zNear, zFar ), m_cullingMask( LAYER_ALL )
    {
    }

    //----------------------------------------------------------------------
    Graphics::CommandBuffer& Camera::recordGraphicsCommands( F32 lerp, const ArrayList<IRenderComponent*>& rendererComponents )
    {
        // Set view matrix from the camera
        auto transform = getGameObject()->getTransform();
        auto modelMatrix = transform->getWorldMatrix( lerp );
        m_camera.setModelMatrix( modelMatrix );

        // Reset command buffer
        m_commandBuffer.reset();

        // Set camera
        m_commandBuffer.setCamera( &m_camera );

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

        for ( auto& additionalCmd : m_additionalCommandBuffers )
            tmpBuffer.merge( *additionalCmd );

        _SortRenderCommands( tmpBuffer, transform->position );

        return m_commandBuffer;
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

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
        // Sort rendering commands by material, which are sorted by their renderqueue
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
            case Graphics::GPUCommand::DRAW_LIGHT:
            {
                // Add command immediately
                m_commandBuffer.getGPUCommands().push_back( command );
                break;
            }
            default:
                ASSERT(false && "Unknown command, must be added here");
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