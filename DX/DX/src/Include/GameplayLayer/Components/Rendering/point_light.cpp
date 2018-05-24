#include "point_light.h"
/**********************************************************************
    class: PointLight (point_light.h)
    
    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"
#include "GameplayLayer/gameobject.h"
#include "camera.h"

namespace Components {

    //----------------------------------------------------------------------
    PointLight::PointLight( F32 intensity, Color color )
        : m_pointLight( intensity, color )
    {
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void PointLight::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        m_pointLight.setPosition( transform->getWorldPosition() );

        cmd.drawLight( &m_pointLight );
    }

    //----------------------------------------------------------------------
    bool PointLight::cull( const Camera& camera ) 
    { 
        return camera.cull( getGameObject()->getTransform()->getWorldPosition(), getRange() );
    }


} // End namespaces