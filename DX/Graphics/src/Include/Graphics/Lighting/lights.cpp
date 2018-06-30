#include "lights.h"
/**********************************************************************
    class: Several
    
    author: S. Hau
    date: June 30, 2018
**********************************************************************/

#include "Math/math_utils.h"

namespace Graphics {

    //**********************************************************************
    // DIRECTIONAL-LIGHT
    //**********************************************************************

    //**********************************************************************
    // POINT-LIGHT
    //**********************************************************************

    //**********************************************************************
    // SPOT-LIGHT
    //**********************************************************************

    //----------------------------------------------------------------------
    SpotLight::SpotLight( F32 intensity, Color color, const Math::Vec3& position, F32 angleInDeg, const Math::Vec3& direction, F32 range )
        : PointLight( LightType::Spot, intensity, color, position, range ), m_angle( Math::Deg2Rad( angleInDeg ) ), m_direction( direction ) {}

    //----------------------------------------------------------------------
    void SpotLight::setAngle( F32 angleInDegree )
    { 
        m_angle = Math::Deg2Rad( angleInDegree ); 
    }

} // End namespaces