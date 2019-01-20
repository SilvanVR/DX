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
    void RenderSystem::execute()
    {
        static const StringID CAMERA_NAMES[] = {
          SID("Camera#0"), SID("Camera#1"), SID("Camera#2"), SID("Camera#3"), SID("Camera#4"),
          SID("Camera#5"), SID("Camera#6"), SID("Camera#7"), SID("Camera#8"),
        };

        // List of lights which rendered a shadowmap this frame. This is needed in order to prevent
        // a shadowmap rendered from a light multiple times (because more than one camera renders the same light)
        static std::unordered_set<Components::ILightComponent*> shadowMapsRendered;
        static ArrayList<Components::ILightComponent*> visibleLights;

        // Clear data structures (memory is still allocated for them)
        shadowMapsRendered.clear();
        visibleLights.clear();

        Graphics::Limits limits = Locator::getRenderer().getLimits();

        // Render scene for each camera
        IScene& scene = Locator::getSceneManager().getCurrentScene();
        const ArrayList<Components::Camera*>           cameras            = scene.getComponentManager().getCameras();
        const ArrayList<Components::ILightComponent*>  lights             = scene.getComponentManager().getLights();
        const ArrayList<Components::IRenderComponent*> rendererComponents = scene.getComponentManager().getRenderer();
        for (U32 i = 0; i < cameras.size(); i++)
        {
            if ( i >= countof(CAMERA_NAMES) ) LOG_ERROR( "Too many cameras in the scene." );

            Components::Camera* cam = cameras[i];
            if ( not cam->isActive() )
                continue;

            // Update camera 
            auto transform = cam->getGameObject()->getTransform();
            auto modelMatrix = transform->getWorldMatrix();
            Math::Vec3 camWorldPos;
            DirectX::XMStoreFloat3( &camWorldPos, modelMatrix.r[3] );
            cam->m_camera.setModelMatrix( modelMatrix );

            // Set camera
            Graphics::CommandBuffer cmd;

            cmd.beginTimeQuery( CAMERA_NAMES[i] );
            cmd.setCamera( cam->m_camera );

            // Lights
            {
                // Record commands for every light component
                for ( Components::ILightComponent* light : lights )
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

                // Sort lights by distance, so lights nearest to camera will be drawn first (or even not culled due to light limit)
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
                    light->recordGraphicsCommands( cmd );

                    // Draw shadowmap if enabled and we are still under the limit
                    if ( light->shadowsEnabled() && (shadowMapsRendered.size() < limits.maxShadowmaps) )
                    {
                        // This prevents rendering of a shadowmap multiple times per frame (because the light is rendered by >1 cameras)
                        if ( shadowMapsRendered.find( light ) == shadowMapsRendered.end() )
                        {
                            light->renderShadowMap( scene );
                            shadowMapsRendered.insert( light );
                        }
                    }

                    lightsDrawn++;
                    if (lightsDrawn == limits.maxLights)
                        break;
                }
            }

            // Rendering components (e.g. mesh-renderer)
            {
                for ( Components::IRenderComponent* rc : rendererComponents )
                {
                    if ( not rc->isActive() )
                        continue;

                    // Check if layer matches
                    bool layerMatch = cam->m_cullingMask & rc->getGameObject()->getLayerMask();
                    if ( not layerMatch )
                        continue;

                    // Check if component is visible
                    bool isVisible = rc->cull( cam->m_camera );
                    if (isVisible)
                        rc->recordGraphicsCommands( cmd );
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
            if ( cam->isBlittingToScreen() || cam->isBlittingToHMD() )
                cmd.blit( PREVIOUS_BUFFER, SCREEN_BUFFER, ASSETS.getPostProcessMaterial() );
            else if (cam->m_additionalCommandBuffers[Components::CameraEvent::PostProcess].size() > 0)
                cmd.blit( PREVIOUS_BUFFER, cam->getRenderTarget(), ASSETS.getPostProcessMaterial() );

            // Add an end camera command
            cmd.endCamera();

            cmd.endTimeQuery( CAMERA_NAMES[i] );

            // Submit command buffer to render engine
            Locator::getRenderer().dispatch( std::move(cmd) );
        }
    }

}