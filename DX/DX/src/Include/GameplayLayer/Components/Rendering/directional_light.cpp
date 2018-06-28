#include "directional_light.h"
/**********************************************************************
    class: DirectionalLight (directional_light.h)
    
    author: S. Hau
    date: May 14, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"
#include "GameplayLayer/gameobject.h"
#include "Core/locator.h"
#include "GameplayLayer/i_scene.h"
#include "i_render_component.hpp"

namespace Components {

    #define SHADOW_MAP_SIZE         2048
    #define DEPTH_STENCIL_FORMAT    Graphics::DepthFormat::D32
    #define ORTHO_SIZE              10
    #define Z_NEAR_OFFSET           10
    #define Z_FAR                   20

    //----------------------------------------------------------------------
    DirectionalLight::DirectionalLight( F32 intensity, Color color )
        : ILightComponent( new Graphics::DirectionalLight( intensity, color ) )
    {
        m_dirLight = dynamic_cast<Graphics::DirectionalLight*>( m_light.get() );

        // Create shadowmap
        auto shadowMap = RESOURCES.createRenderBuffer();
        shadowMap->create( SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, DEPTH_STENCIL_FORMAT );
        shadowMap->setFilter( Graphics::TextureFilter::Point );
        shadowMap->setClampMode( Graphics::TextureAddressMode::Clamp );
        m_dirLight->setShadowMap( shadowMap );

        // Create rendertexture
        auto rt = RESOURCES.createRenderTexture();
        rt->create( nullptr, m_dirLight->getShadowMap() );

        // Configure camera
        m_camera.setRenderTarget( rt, false );
        m_camera.setCameraMode( Graphics::CameraMode::Orthographic );
        m_camera.setOrthoParams( -ORTHO_SIZE, ORTHO_SIZE, -ORTHO_SIZE, ORTHO_SIZE, -Z_NEAR_OFFSET, Z_FAR);
    }

    //----------------------------------------------------------------------
    void DirectionalLight::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        m_dirLight->setDirection( transform->rotation.getForward() );

        cmd.drawLight( m_dirLight );
    }

    //----------------------------------------------------------------------
    void DirectionalLight::renderShadowMap( const IScene& scene, F32 lerp )
    {
        Graphics::CommandBuffer cmd;

        // Update camera 
        auto transform = getGameObject()->getTransform();
        auto modelMatrix = transform->getWorldMatrix(lerp);
        m_camera.setModelMatrix( modelMatrix );

        m_dirLight->setShadowViewProjection( m_camera.getViewProjectionMatrix() );

        // Set camera
        cmd.setCamera( &m_camera );

        // Record commands for every rendering component
        for ( auto& renderer : scene.getComponentManager().getRenderer() )
        {
            if ( not renderer->isActive() || not renderer->castShadows() )
                continue;

            // Check if component is visible
            bool isVisible = renderer->cull( m_camera );
            if (isVisible)
                renderer->recordGraphicsCommandsShadows( cmd, lerp );
        }

        cmd.endCamera( &m_camera );

        Locator::getRenderer().dispatch( cmd );
    }


} // End namespaces