#include "graphics_command_recorder.h"
/**********************************************************************
    class: GraphicsRecorder (graphics_recorder.cpp)

    author: S. Hau
    date: December 24, 2017
**********************************************************************/

#include "locator.h"
#include "GameplayLayer/i_scene.h"
#include "GameplayLayer/gameobject.h"
#include "Core/Graphics/command_buffer.h"

namespace Core {

    //----------------------------------------------------------------------
    void GraphicsCommandRecorder::init()
    {

    }

    //----------------------------------------------------------------------
    void GraphicsCommandRecorder::shutdown()
    {

    }


    //----------------------------------------------------------------------
    void GraphicsCommandRecorder::render( IScene* scene, F32 lerp )
    {
        auto& gameObjects = scene->getGameObjects();
        for ( auto go : gameObjects )
        {
            if ( go->isActive() )
            {
                go->recordGraphicsCommands( *m_CommandBuffers[0], lerp );
            }
        }

        Locator::getRenderer().dispatch( *m_CommandBuffers[0] );
    }

}