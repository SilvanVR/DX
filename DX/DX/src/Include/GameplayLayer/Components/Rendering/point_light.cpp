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

        m_pointLight.setPosition( transform->getWorldPosition() );

        cmd.drawLight( &m_pointLight );
    }

    //----------------------------------------------------------------------
    bool PointLight::cull( const Camera& camera ) 
    { 
        //@TODO:
        return true; 
    }

    //// Calculate the range this light can affect
    //void PointLight::calculateRange()
    //{
    //    static const float MIN_LIGHT_STRENGTH = 256.0f / 1.0f;

    //    Vec3f& attenuation = getAttenuation();

    //    // Calculate the max range the light can affect
    //    float exponent = getExponent() + 0.01f;
    //    float linear = getLinear();
    //    float constant = getConstant();

    //    float c = constant - MIN_LIGHT_STRENGTH * getIntensity() * getColor().getMax();

    //    range = static_cast<float>((-linear + sqrt(linear * linear - 4 * exponent * c) / (2 * exponent)));

    //    // Set scale for the light-volume
    //    getTransform().scale = Vec3f(range, range, range);
    //}


} // End namespaces