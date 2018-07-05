#include "point_light.h"
/**********************************************************************
    class: PointLight (point_light.h)
    
    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"
#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/i_scene.h"
#include "i_render_component.hpp"
#include "Core/locator.h"
#include "camera.h"

namespace Components {

    #define DEPTH_STENCIL_FORMAT    Graphics::DepthFormat::D32      // Must be compatible with the format below
    #define TEMP_RT_FORMAT          Graphics::TextureFormat::RFloat // Must be compatible with the format above

    //----------------------------------------------------------------------
    PointLight::PointLight( F32 intensity, Color color, F32 range, bool shadowsEnabled )
        : ILightComponent( new Graphics::PointLight( intensity, color, {}, range ) )
    {
        m_pointLight = dynamic_cast<Graphics::PointLight*>( m_light.get() );
        if (shadowsEnabled)
            _CreateShadowMap( CONFIG.getShadowMapQuality() );
    }

    //----------------------------------------------------------------------
    void PointLight::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        m_pointLight->setPosition( transform->getWorldPosition() );

        cmd.drawLight( m_pointLight );
    }

    //----------------------------------------------------------------------
    bool PointLight::cull( const Graphics::Camera& camera )
    { 
        return camera.cull( getGameObject()->getTransform()->getWorldPosition(), getRange() );
    }

    //----------------------------------------------------------------------
    void PointLight::renderShadowMap( const IScene& scene, F32 lerp )
    {
        Graphics::CommandBuffer cmd;

        DirectX::XMVECTOR directions[] = {
            { 1, 0, 0, 0 }, { -1,  0,  0, 0 },
            { 0, 1, 0, 0 }, {  0, -1,  0, 0 },
            { 0, 0, 1, 0 }, {  0,  0, -1, 0 },
        };
        DirectX::XMVECTOR ups[] = {
            { 0, 1,  0, 0 }, { 0, 1, 0, 0 },
            { 0, 0, -1, 0 }, { 0, 0, 1, 0 },
            { 0, 1,  0, 0 }, { 0, 1, 0, 0 },
        };

        m_camera->setZFar( getRange() );

        auto transform = getGameObject()->getTransform();

        for (I32 face = 0; face < 6; face++)
        {
            auto worldPos = DirectX::XMLoadFloat3( &transform->getWorldPosition() );
            auto view = DirectX::XMMatrixLookToLH( worldPos, directions[face], ups[face] );
            m_camera->setViewMatrix( view );

            // Set camera
            cmd.setCamera( *m_camera );

            // Record commands for every rendering component
            for ( auto& renderer : scene.getComponentManager().getRenderer() )
            {
                if ( not renderer->isActive() || not renderer->isCastingShadows() )
                    continue;

                // Check if component is visible
                bool isVisible = renderer->cull( *m_camera );
                if (isVisible)
                    renderer->recordGraphicsCommands( cmd, lerp );
            }

            cmd.endCamera();

            cmd.copyTexture( m_camera->getRenderTarget()->getDepthBuffer(), 0, 0, m_light->getShadowMap(), face, 0 );
        }

        Locator::getRenderer().dispatch( cmd );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************


    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void PointLight::_CreateShadowMap( Graphics::ShadowMapQuality quality )
    {
        U32 shadowMapSize = 0;
        switch (quality)
        {
            case Graphics::ShadowMapQuality::Low:     shadowMapSize = 256;  break;
            case Graphics::ShadowMapQuality::Medium:  shadowMapSize = 512;  break;
            case Graphics::ShadowMapQuality::High:    shadowMapSize = 1024; break;
            case Graphics::ShadowMapQuality::Insane:  shadowMapSize = 2048; break;
        }

        if (shadowMapSize > 0)
        {
            // Create cubemap
            auto shadowMap = RESOURCES.createCubemap();
            shadowMap->create( shadowMapSize, TEMP_RT_FORMAT, Graphics::Mips::None );
            shadowMap->setAnisoLevel( 1 );
            shadowMap->setFilter( Graphics::TextureFilter::Point );
            shadowMap->setClampMode( Graphics::TextureAddressMode::Clamp );
            m_light->setShadowMap( shadowMap );

            // Create shadowmap (rendertarget, which gets copied to each face)
            auto shadowMapRender = RESOURCES.createRenderBuffer();
            shadowMapRender->create( shadowMapSize, shadowMapSize, DEPTH_STENCIL_FORMAT );

            // Create rendertexture
            auto rt = RESOURCES.createRenderTexture();
            rt->create( nullptr, shadowMapRender );

            // Configure camera
            m_camera.reset( new Graphics::Camera( 90.0f, 0.1f, getRange() ) );
            m_camera->setRenderTarget( rt, false );
            m_camera->setReplacementShader( ASSETS.getShadowMapShaderLinear(), TAG_SHADOW_PASS_LINEAR );
        }
    }



} // End namespaces