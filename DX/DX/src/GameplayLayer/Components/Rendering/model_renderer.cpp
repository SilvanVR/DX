#include "model_renderer.h"
/**********************************************************************
    class: ModelRenderer (model_renderer.cpp)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "GameplayLayer/Components/transform.h"

namespace Components {

    //----------------------------------------------------------------------
    void ModelRenderer::addedToGameObject( GameObject* go )
    {
        auto transform = getGameObject()->getComponent<Transform>();
        if ( transform == nullptr )
            WARN( "ModelRenderer-Component requires a transform for a game-object, but the attached game-object has none!" );
    }

    //----------------------------------------------------------------------
    void ModelRenderer::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getComponent<Transform>();
        ASSERT( transform != nullptr );

        // Need:
        // 1. MeshID
        // 2. MaterialID
        // 3. Interpolated World Matrix
        cmd.drawMesh();
    }


}