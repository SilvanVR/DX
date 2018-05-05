#include "graphics_command_recorder.h"
/**********************************************************************
    class: GraphicsRecorder (graphics_recorder.cpp)

    author: S. Hau
    date: December 24, 2017
**********************************************************************/

#include "Core/locator.h"
#include "GameplayLayer/i_scene.h"
#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "GameplayLayer/Components/Rendering/camera.h"
#include "GameplayLayer/Components/Rendering/i_render_component.hpp"

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
    void GraphicsCommandRecorder::dispatch( IScene& scene, F32 lerp )
    {
        auto& graphicsEngine = Locator::getRenderer();

        // Fetch all renderer components
        auto& renderers = scene.getComponentManager().getRenderer();

        // Create rendering commands for each camera and submit them to the graphics-engine
        for (auto& cam : scene.getComponentManager().getCameras())
        {
            auto& cmd = cam->recordGraphicsCommands( lerp, renderers );

            // Execute rendering commands from this camera
            graphicsEngine.dispatch( cmd );

            // Execute additional attached command buffers
            for ( auto& camCMD : cam->getAdditionalCommandBuffers() )
                graphicsEngine.dispatch( *camCMD );
        }
    }

}