#include "c_model_renderer.h"
/**********************************************************************
    class: CModelRenderer (CModelRenderer.cpp)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "GameplayLayer/Components/c_transform.h"

namespace Components {

    //----------------------------------------------------------------------
    void CModelRenderer::addedToGameObject( GameObject* go )
    {
        auto transform = getGameObject()->getComponent<CTransform>();
        if ( transform == nullptr )
            WARN( "CModelRenderer-Component requires a transform for a game-object, but the attached game-object has none!" );
    }

    //----------------------------------------------------------------------
    void CModelRenderer::recordGraphicsCommands( Core::Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getComponent<CTransform>();
        ASSERT( transform != nullptr );

        // Need:
        // 1. MeshID
        // 2. MaterialID
        // 3. Interpolated World Matrix
        cmd.drawMesh();
    }


}