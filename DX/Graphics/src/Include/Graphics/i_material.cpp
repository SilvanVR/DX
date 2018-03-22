#include "i_material.h"
/**********************************************************************
    class: IMaterial (i_material.cpp)

    author: S. Hau
    date: March 22, 2018
**********************************************************************/

namespace Graphics {

    //----------------------------------------------------------------------
    F32 IMaterial::getFloat( StringID name ) const 
    { 
        if ( m_floatMap.count( name ) > 0)
            return m_floatMap.at( name );

        WARN_RENDERING( "Material::getFloat(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
        return 0.0f;
    }

    //----------------------------------------------------------------------
    Math::Vec4 IMaterial::getVec4( StringID name ) const
    {
        if ( m_vec4Map.count( name ) > 0 )
            return m_vec4Map.at( name );

        WARN_RENDERING( "Material::getVec4(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
        return Math::Vec4( 0.0f );
    }

    //**********************************************************************
    // MATERIAL PARAMETERS
    //**********************************************************************

    //----------------------------------------------------------------------
    void IMaterial::setFloat( StringID name, F32 val )
    { 
        if ( _SetFloat( name, val ) )
            m_floatMap[ name ] = val;
        else
            WARN_RENDERING( "Material::setFloat(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::setVec4( StringID name, const Math::Vec4& vec )
    { 
        if ( _SetVec4( name, vec ) )
            m_vec4Map[ name ] = vec; 
        else
            WARN_RENDERING( "Material::setVec4(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
    }


} // End namespaces