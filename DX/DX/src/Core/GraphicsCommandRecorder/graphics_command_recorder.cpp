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

        // Fetch all renderer components e.g. model-renderer
        auto& renderers = scene.getComponentManager().getRenderer();

        // Create rendering commands for each camera and submit them to the graphics-engine
        auto& cameras = scene.getComponentManager().getCameras();

        for (auto& cam : cameras)
        {
            auto& cmd = cam->getCommandBuffer();

            cmd.reset();

            // Record where to render, view- & projection, viewport
            cam->recordGraphicsCommands( cmd, lerp );

            // Add optionally attached command buffer aswell
            //foreach commandBuffer in camera->commandBuffers
                //graphicsEngine->dispatch(commandBuffer)

            // Do viewfrustum culling with every renderer component
            for (auto& renderer : renderers)
            {
                if ( not renderer->isActive() )
                    continue;
                //bool isVisible = renderer->Cull(camera);
                //bool layerMatch = camera->layerMask & renderer->getLayerMask();
                //if (isVisible && layerMatch)
                     renderer->recordGraphicsCommands( cmd, lerp );
            }

            // Execute rendering commands
            graphicsEngine.dispatch( cmd );
        }
    }

}