#include "graphics_command_recorder.h"
/**********************************************************************
    class: GraphicsRecorder (graphics_recorder.cpp)

    author: S. Hau
    date: December 24, 2017
**********************************************************************/

#include "locator.h"
#include "GameplayLayer/i_scene.h"
#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"

namespace Core {

    //----------------------------------------------------------------------
    void GraphicsCommandRecorder::init()
    {
        for(I32 i = 0; i < COMMAND_BUFFER_COUNT; i++)
            m_CommandBuffers[i] = new Graphics::CommandBuffer();
    }

    //----------------------------------------------------------------------
    void GraphicsCommandRecorder::shutdown()
    {
        for (I32 i = 0; i < COMMAND_BUFFER_COUNT; i++)
            SAFE_DELETE( m_CommandBuffers[i] );
    }

    //----------------------------------------------------------------------
    void GraphicsCommandRecorder::render( IScene& scene, F32 lerp )
    {
        auto& gameObjects = scene.getGameObjects();
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