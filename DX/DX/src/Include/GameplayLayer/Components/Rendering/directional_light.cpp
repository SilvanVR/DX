#include "directional_light.h"
/**********************************************************************
    class: DirectionalLight (directional_light.h)
    
    author: S. Hau
    date: May 14, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"

namespace Components {

    //----------------------------------------------------------------------
    DirectionalLight::DirectionalLight( const Math::Vec3& direction, F32 intensity, Color color )
        : m_direction( direction ), m_intensity( intensity ), m_color( color )
    {

    }

    //----------------------------------------------------------------------
    void DirectionalLight::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {


    }

    //----------------------------------------------------------------------
    bool DirectionalLight::cull( const Camera& camera )
    {

        return true;
    }



} // End namespaces