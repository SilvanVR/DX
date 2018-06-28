#include "spot_light.h"
/**********************************************************************
    class: SpotLight (spot_light.h)
    
    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"
#include "GameplayLayer/gameobject.h"
#include "camera.h"

namespace Components {

    //----------------------------------------------------------------------
    SpotLight::SpotLight( F32 intensity, Color color, F32 spotAngleInDegrees, F32 range )
        : ILightComponent( new Graphics::SpotLight( intensity, color, { 0, 1, 0 }, spotAngleInDegrees, { 0, 0, 1 }, range ) )
    {
        m_spotLight = dynamic_cast<Graphics::SpotLight*>( m_light.get() );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

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


} // End namespaces