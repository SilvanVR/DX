#include "point_light.h"
/**********************************************************************
    class: PointLight (point_light.h)
    
    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"
#include "GameplayLayer/gameobject.h"
#include "Core/locator.h"
#include "camera.h"

namespace Components {

    #define DEPTH_STENCIL_FORMAT Graphics::DepthFormat::D32

    //----------------------------------------------------------------------
    PointLight::PointLight( F32 intensity, Color color, F32 range )
        : ILightComponent( new Graphics::PointLight( intensity, color, {}, range ) )
    {
        m_pointLight = dynamic_cast<Graphics::PointLight*>( m_light.get() );
    }

    //----------------------------------------------------------------------
    PointLight::PointLight( F32 intensity, Color color, F32 range, Graphics::ShadowMapQuality quality )
        : ILightComponent( new Graphics::PointLight( intensity, color, {}, range ), quality )
    {
        m_pointLight = dynamic_cast<Graphics::PointLight*>( m_light.get() );
        _CreateShadowMap( quality );
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
            shadowMap->setFilter( Graphics::TextureFilter::Point );
            shadowMap->setClampMode( Graphics::TextureAddressMode::Clamp );
            m_light->setShadowMap( shadowMap );

            // Create rendertexture
            auto rt = RESOURCES.createRenderTexture();
            rt->create( nullptr, m_light->getShadowMap() );

            // Configure camera
            m_camera.reset( new Graphics::Camera( 90.0f, 0.1f, getRange() ) );
            m_camera->setRenderTarget( rt, false );
        }
    }



} // End namespaces