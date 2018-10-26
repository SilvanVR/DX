#include "i_material.h"
/**********************************************************************
    class: IMaterial (i_material.cpp)

    author: S. Hau
    date: March 22, 2018
**********************************************************************/

#include "Logging/logging.h"

namespace Graphics {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void IMaterial::setShader( const ShaderPtr& shader )
    {
        ASSERT( shader );

        m_shader = shader;
        ICachedShaderMaps::_ClearAllMaps();
        _ChangedShader();
    }

    //----------------------------------------------------------------------
    DataType IMaterial::getDataType( StringID name ) const
    {
        if (not m_shader)
            return DataType::Unknown;

        return m_shader->getDataTypeOfMaterialPropertyOrResource( name );
    }

    //**********************************************************************
    // PROTECTED
    //**********************************************************************

    //----------------------------------------------------------------------
    void IMaterial::_WarnMissingInt( StringID name ) const
    {
        LOG_WARN_RENDERING( "IMaterial::getInt(): Name '" + name.toString() + "' does not exist in material '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::_WarnMissingFloat( StringID name ) const
    {
        LOG_WARN_RENDERING( "IMaterial::getFloat(): Name '" + name.toString() + "' does not exist in material '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::_WarnMissingColor( StringID name ) const
    {
        LOG_WARN_RENDERING( "IMaterial::getColor(): Name '" + name.toString() + "' does not exist in material '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::_WarnMissingVec4( StringID name ) const
    {
        LOG_WARN_RENDERING( "IMaterial::getVec4(): Name '" + name.toString() + "' does not exist in material '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::_WarnMissingMatrix( StringID name ) const
    {
        LOG_WARN_RENDERING( "IMaterial::getMatrix(): Name '" + name.toString() + "' does not exist in material '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::_WarnMissingTexture( StringID name ) const
    {
        LOG_WARN_RENDERING( "IMaterial::getTexture(): Name '" + name.toString() + "' does not exist in material '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    bool IMaterial::_HasShaderInt( StringID name ) const
    {
        if ( not m_shader->hasMaterialProperty( name ) )
        {
            LOG_WARN_RENDERING( "IMaterial::setInt(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool IMaterial::_HasShaderFloat( StringID name ) const
    {
        if ( not m_shader->hasMaterialProperty( name ) )
        {
            LOG_WARN_RENDERING( "IMaterial::setFloat(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool IMaterial::_HasShaderColor( StringID name ) const
    {
        if ( not m_shader->hasMaterialProperty( name ) )
        {
            LOG_WARN_RENDERING( "IMaterial::setColor(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool IMaterial::_HasShaderVec4( StringID name ) const
    {
        if ( not m_shader->hasMaterialProperty( name ) )
        {
            LOG_WARN_RENDERING( "IMaterial::setVec4(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool IMaterial::_HasShaderMatrix( StringID name ) const
    {
        if ( not m_shader->hasMaterialProperty( name ) )
        {
            LOG_WARN_RENDERING( "IMaterial::setMatrix(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool IMaterial::_HasShaderTexture( StringID name ) const
    {
        if ( not m_shader->getShaderResource( name ) )
        {
            LOG_WARN_RENDERING( "IMaterial::setTexture(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    void IMaterial::_BindTextures()
    {
        for (auto& pair : m_textureMap)
        {
            auto shaderRes = m_shader->getShaderResource( pair.first );
            if (shaderRes) // shader res can be null when the shader was reloaded but the res no longer exists in it
                pair.second->bind( *shaderRes );
        }
    }


} // End namespaces