#include "render_system.h"
/**********************************************************************
    class: RenderSystem (render_system.cpp)

    author: S. Hau
    date: June 30, 2018
**********************************************************************/

#include "Core/locator.h"
#include "GameplayLayer/i_scene.h"
#include "GameplayLayer/Components/Rendering/camera.h"
#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/Components/Rendering/i_light_component.h"
#include "GameplayLayer/Components/Rendering/i_render_component.hpp"
#include <unordered_set>

namespace Core {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderSystem::execute( F32 lerp )
    {
        auto& renderer = Locator::getRenderer();

        // List of lights which rendered a shadowmap this frame. This is needed in order to prevent
        // a shadowmap rendered from a light multiple times (because more than one camera renders the same light)
        std::unordered_set<Components::ILightComponent*> shadowMapsRendered;

        // Render each camera
        auto& scene = Locator::getSceneManager().getCurrentScene();
        for (auto& cam : scene.getComponentManager().getCameras())
        {
            // Update camera 
            auto transform = cam->getGameObject()->getTransform();
            auto modelMatrix = transform->getWorldMatrix( lerp );
            auto camWorldPos = transform->getWorldPosition();
            cam->m_camera.setModelMatrix( modelMatrix );

            // Set camera
            Graphics::CommandBuffer cmd;
            cmd.setCamera( &cam->m_camera );

            // Lights
            {
                // Record commands for every light component
                ArrayList<Components::ILightComponent*> visibleLights;
                for ( auto& light : scene.getComponentManager().getLights() )
                {
                    if ( not light->isActive() )
                        continue;

                    // Check if layer matches
                    bool layerMatch = cam->m_cullingMask & light->getGameObject()->getLayerMask();
                    if ( not layerMatch )
                        continue;

                    // Check if light is visible
                    bool isVisible = light->cull( cam->m_camera );
                    if (isVisible)
                        visibleLights.push_back( light );
                }

                // Sort lights by distance, so lights nearest to camera will be drawn first (or not culled due to light limit)
                std::sort( visibleLights.begin(), visibleLights.end(), [camWorldPos](Components::ILightComponent*& l1, Components::ILightComponent*& l2) {
                    auto pos = l1->getGameObject()->getTransform()->getWorldPosition();
                    auto pos2 = l2->getGameObject()->getTransform()->getWorldPosition();
                    return camWorldPos.distance( pos ) < camWorldPos.distance( pos2 );
                } );

                // Record commands for a light
                U32 lightsDrawn = 0;
                for (auto& light : visibleLights)
                {
                    // Draw light
                    light->recordGraphicsCommands( cmd, lerp );

                    // Draw shadowmap if enabled and we are still under the limit
                    if ( light->shadowsEnabled() && (shadowMapsRendered.size() < renderer.getLimits().maxShadowmaps) )
                    {
                        // This prevents rendering of a shadowmap multiple times per frame (because the light is rendered by >1 cameras)
                        if ( shadowMapsRendered.find( light ) == shadowMapsRendered.end() )
                        {
                            light->renderShadowMap( scene, lerp );
                            shadowMapsRendered.insert( light );
                        }
                    }

                    lightsDrawn++;
                    if (lightsDrawn == renderer.getLimits().maxLights)
                        break;
                }
            }

            // Rendering components (e.g. mesh-renderer)
            {
                for ( auto& renderer : scene.getComponentManager().getRenderer() )
                {
                    if ( not renderer->isActive() )
                        continue;

                    // Check if layer matches
                    bool layerMatch = cam->m_cullingMask & renderer->getGameObject()->getLayerMask();
                    if ( not layerMatch )
                        continue;

                    // Check if component is visible
                    bool isVisible = renderer->cull( cam->m_camera );
                    if (isVisible)
                        renderer->recordGraphicsCommands( cmd, lerp );
                }
            }

            // Merge all geometry commands
            for (auto& additionalCmd : cam->m_additionalCommandBuffers[Components::CameraEvent::Geometry])
                cmd.merge( *additionalCmd );

            // Sort all draw commands
            cmd.sortDrawCommands( camWorldPos );

            // Merge all post process commands
            for (auto& additionalCmd : cam->m_additionalCommandBuffers[Components::CameraEvent::PostProcess])
                cmd.merge( *additionalCmd );

            // Merge all gui commands
            for (auto& additionalCmd : cam->m_additionalCommandBuffers[Components::CameraEvent::Overlay])
                cmd.merge( *additionalCmd );

            // Inject an command which blits last rendered buffer to the screen/render target if we
            // have at least one post processing command buffer attached or we are rendering to the screen.
            if( cam->m_camera.isRenderingToScreen() )
                cmd.blit( PREVIOUS_BUFFER, SCREEN_BUFFER, ASSETS.getPostProcessMaterial() );
            else if (cam->m_additionalCommandBuffers[Components::CameraEvent::PostProcess].size() > 0)
                cmd.blit( PREVIOUS_BUFFER, cam->getRenderTarget(), ASSETS.getPostProcessMaterial() );

            // Add an end camera command
            cmd.endCamera( &cam->m_camera );

            // Submit command buffer to render engine
            renderer.dispatch( std::move( cmd ) );
        }
    }

}