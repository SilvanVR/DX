#include "i_light_component.h"
/**********************************************************************
    class: ILightComponent

    author: S. Hau
    date: June 30, 2018
**********************************************************************/

#include "Graphics/camera.h"
#include "Graphics/command_buffer.h"
#include "GameplayLayer/i_scene.h"
#include "i_render_component.hpp"
#include "Core/locator.h"
#include "GameplayLayer/gameobject.h"

namespace Components {

    //----------------------------------------------------------------------
    ILightComponent::ILightComponent( Graphics::Light* light )
        : m_light{ light }, m_shadowMapQuality{ CONFIG.getShadowMapQuality() }
    {
    }

    //----------------------------------------------------------------------
    ILightComponent::ILightComponent(Graphics::Light* light, Graphics::ShadowMapQuality quality)
            : m_light{ light }, m_shadowMapQuality( quality )
    {
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void ILightComponent::setShadowMapQuality( Graphics::ShadowMapQuality quality )
    { 
        if ( not shadowsEnabled() )
            return;

        m_shadowMapQuality = quality;
        if ( shadowsEnabled() )
            _CreateShadowMap( m_shadowMapQuality );
    }

    //----------------------------------------------------------------------
    void ILightComponent::setShadowType( Graphics::ShadowType shadowType )
    { 
        if ( m_light->supportsShadowType( shadowType ) )
        {
            m_light->setShadowType( shadowType );
            if ( shadowsEnabled() )
                _CreateShadowMap( m_shadowMapQuality );
        }
    }

    //----------------------------------------------------------------------
    void ILightComponent::setShadowTypeAndQuality( Graphics::ShadowType shadowType, Graphics::ShadowMapQuality quality )
    {
        m_shadowMapQuality = quality;
        if ( m_light->supportsShadowType( shadowType ) )
            m_light->setShadowType( shadowType );
        if ( shadowsEnabled() )
            _CreateShadowMap( m_shadowMapQuality );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void ILightComponent::renderShadowMap( const IScene& scene, F32 lerp )
    {
        Graphics::CommandBuffer cmd;

        // Update camera 
        auto transform = getGameObject()->getTransform();
        auto modelMatrix = transform->getWorldMatrix( lerp );
        m_camera->setModelMatrix( modelMatrix );

        m_light->setShadowViewProjection( m_camera->getViewProjectionMatrix() );

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

        Locator::getRenderer().dispatch( cmd );
    }

}