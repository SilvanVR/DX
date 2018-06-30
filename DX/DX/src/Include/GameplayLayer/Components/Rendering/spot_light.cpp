#include "spot_light.h"
/**********************************************************************
    class: SpotLight (spot_light.h)
    
    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Math/math_utils.h"
#include "Core/locator.h"
#include "camera.h"

namespace Components {

    #define DEPTH_STENCIL_FORMAT    Graphics::DepthFormat::D32

    //----------------------------------------------------------------------
    SpotLight::SpotLight( F32 intensity, Color color, F32 spotAngleInDegrees, F32 range )
        : ILightComponent( new Graphics::SpotLight( intensity, color, { 0, 1, 0 }, spotAngleInDegrees, { 0, 0, 1 }, range ) )
    {
        m_spotLight = dynamic_cast<Graphics::SpotLight*>( m_light.get() );
    }

    //----------------------------------------------------------------------
    SpotLight::SpotLight( F32 intensity, Color color, F32 spotAngleInDegrees, F32 range, Graphics::ShadowMapQuality quality )
        : ILightComponent( new Graphics::SpotLight( intensity, color, { 0, 1, 0 }, spotAngleInDegrees, { 0, 0, 1 }, range ), quality )
    {
        m_spotLight = dynamic_cast<Graphics::SpotLight*>( m_light.get() );
        _CreateShadowMap( quality );
    }

    //----------------------------------------------------------------------
    void SpotLight::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        m_spotLight->setPosition( transform->getWorldPosition());
        m_spotLight->setDirection( transform->getWorldRotation().getForward() );

        cmd.drawLight( m_spotLight );
    }

    //----------------------------------------------------------------------
    bool SpotLight::cull( const Graphics::Camera& camera )
    { 
        return camera.cull( getGameObject()->getTransform()->getWorldPosition(), getRange() );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    F32 SpotLight::getAngle() const
    {
        return Math::Rad2Deg( m_spotLight->getAngle() ); 
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void SpotLight::_CreateShadowMap( Graphics::ShadowMapQuality quality )
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
            m_camera.reset( new Graphics::Camera( getAngle() * 2.0f, 0.1f, getRange() ) );
            m_camera->setRenderTarget( rt, false );
        }
    }



} // End namespaces