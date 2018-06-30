#include "directional_light.h"
/**********************************************************************
    class: DirectionalLight (directional_light.h)
    
    author: S. Hau
    date: May 14, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"
#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/i_scene.h"
#include "i_render_component.hpp"
#include "Graphics/camera.h"
#include "Core/locator.h"

namespace Components {

    #define DEPTH_STENCIL_FORMAT    Graphics::DepthFormat::D32
    #define ORTHO_SIZE              10
    #define Z_NEAR_OFFSET           10
    #define Z_FAR                   20

    //----------------------------------------------------------------------
    DirectionalLight::DirectionalLight( F32 intensity, Color color, bool shadowsEnabled )
        : ILightComponent( new Graphics::DirectionalLight( intensity, color ) )
    {
        m_dirLight = dynamic_cast<Graphics::DirectionalLight*>( m_light.get() );

        if (shadowsEnabled)
            _CreateShadowMap( CONFIG.getShadowMapQuality() );
    }

    //----------------------------------------------------------------------
    void DirectionalLight::_CreateShadowMap( Graphics::ShadowMapQuality quality )
    {
        U32 shadowMapSize = 0;
        switch (quality)
        {
            case Graphics::ShadowMapQuality::Low:     shadowMapSize = 512;  break;
            case Graphics::ShadowMapQuality::Medium:  shadowMapSize = 1024; break;
            case Graphics::ShadowMapQuality::High:    shadowMapSize = 2048; break;
            case Graphics::ShadowMapQuality::Insane:  shadowMapSize = 4096; break;
        }

        if (shadowMapSize > 0)
        {
            // Create shadowmap
            auto shadowMap = RESOURCES.createRenderBuffer();
            shadowMap->create( shadowMapSize, shadowMapSize, DEPTH_STENCIL_FORMAT );
            shadowMap->setAnisoLevel( 1 );
            shadowMap->setFilter( Graphics::TextureFilter::Point );
            shadowMap->setClampMode( Graphics::TextureAddressMode::Clamp );
            m_light->setShadowMap( shadowMap );

            // Create rendertexture
            auto rt = RESOURCES.createRenderTexture();
            rt->create( nullptr, m_light->getShadowMap() );

            // Configure camera
            m_camera.reset( new Graphics::Camera( -ORTHO_SIZE, ORTHO_SIZE, -ORTHO_SIZE, ORTHO_SIZE, -Z_NEAR_OFFSET, Z_FAR ) );
            m_camera->setRenderTarget( rt, false );
        }
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
        //@TODO: Adjust camera frustum

        ILightComponent::renderShadowMap( scene, lerp );
    }


} // End namespaces