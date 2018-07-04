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

    //----------------------------------------------------------------------
    bool DirectionalLight::supportsShadowType( ShadowType shadowType ) 
    { 
        switch (shadowType)
        {
        case ShadowType::None:
        case ShadowType::Hard:
        case ShadowType::Soft:
        case ShadowType::CSM:
            return true;
        }
        return false; 
    }

    //----------------------------------------------------------------------
    void DirectionalLight::setCSMSplitRanges( const ArrayList<F32>& ranges ) 
    { 
        m_csmSplits.resize( ranges.size() );
        for (auto i = 0; i < ranges.size(); ++i)
            m_csmSplits[i].range = ranges[i];
    }

    //----------------------------------------------------------------------
    void DirectionalLight::setCSMShadowViewProjection( I32 cascade, const DirectX::XMMATRIX& vp )
    {
        ASSERT( cascade < m_csmSplits.size() && "Not enough cascades! Forgot to set the split ranges first?" );
        m_csmSplits[cascade].vp = vp;
    }

    //**********************************************************************
    // POINT-LIGHT
    //**********************************************************************

    //----------------------------------------------------------------------
    bool PointLight::supportsShadowType( ShadowType shadowType ) 
    { 
        switch (shadowType)
        {
        case ShadowType::None:
        case ShadowType::Hard:
        case ShadowType::Soft:
            return true;
        }
        return false; 
    }

    //**********************************************************************
    // SPOT-LIGHT
    //**********************************************************************

    //----------------------------------------------------------------------
    SpotLight::SpotLight( F32 intensity, Color color, const Math::Vec3& position, F32 angleInDeg, const Math::Vec3& direction, F32 range )
        : PointLight( LightType::Spot, intensity, color, position, range ), m_angle( Math::Deg2Rad( angleInDeg ) ), m_direction( direction ) {}

    //----------------------------------------------------------------------
    bool SpotLight::supportsShadowType( ShadowType shadowType )
    { 
        switch (shadowType)
        {
        case ShadowType::None:
        case ShadowType::Hard:
        case ShadowType::Soft:
            return true;
        }
        return false; 
    }

    //----------------------------------------------------------------------
    void SpotLight::setAngle( F32 angleInDegree )
    { 
        m_angle = Math::Deg2Rad( angleInDegree ); 
    }

} // End namespaces