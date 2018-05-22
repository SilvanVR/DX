#include "point_light.h"
/**********************************************************************
    class: PointLight (point_light.h)
    
    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"
#include "GameplayLayer/gameobject.h"

namespace Components {

    //----------------------------------------------------------------------
    PointLight::PointLight( F32 intensity, Color color )
        : m_pointLight( intensity, color )
    {
    }

    //----------------------------------------------------------------------
    void PointLight::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        m_pointLight.setPosition( transform->position );

        cmd.drawLight( &m_pointLight );
    }

    //----------------------------------------------------------------------
    bool PointLight::cull( const Camera& camera ) 
    { 
        //@TODO:
        return true; 
    }


} // End namespaces