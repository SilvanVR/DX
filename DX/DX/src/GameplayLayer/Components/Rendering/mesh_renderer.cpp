#include "mesh_renderer.h"
/**********************************************************************
    class: MeshRenderer (mesh_renderer.cpp)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "GameplayLayer/Components/transform.h"

namespace Components {

    //----------------------------------------------------------------------
    void MeshRenderer::AddedToGameObject( GameObject* go )
    {
        auto transform = go->getComponent<Transform>();
        if ( transform == nullptr )
            WARN( "MeshRenderer-Component requires a transform for a game-object, but the attached game-object has none!" );
    }

    //----------------------------------------------------------------------
    void MeshRenderer::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        if (m_mesh == nullptr)
            return;

        auto transform = getGameObject()->getComponent<Transform>();
        ASSERT( transform != nullptr );

        // 1. MeshID
        // 2. MaterialID
        // 3. Interpolated World Matrix
        auto modelMatrix = transform->getTransformationMatrix();
        cmd.drawMesh( modelMatrix, m_mesh );
    }


}