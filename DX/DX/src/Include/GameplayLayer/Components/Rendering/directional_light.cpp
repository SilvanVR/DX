#include "directional_light.h"
/**********************************************************************
    class: DirectionalLight (directional_light.h)
    
    author: S. Hau
    date: May 14, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"
#include "GameplayLayer/gameobject.h"

namespace Components {

    //----------------------------------------------------------------------
    DirectionalLight::DirectionalLight( F32 intensity, Color color )
        : m_dirLight( intensity, color )
    {
    }

    //----------------------------------------------------------------------
    void DirectionalLight::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        m_dirLight.setDirection( transform->rotation.getForward() );

        cmd.drawLight( &m_dirLight );
    }


} // End namespaces