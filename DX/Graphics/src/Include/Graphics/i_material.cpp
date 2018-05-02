#include "i_material.h"
/**********************************************************************
    class: IMaterial (i_material.cpp)

    author: S. Hau
    date: March 22, 2018
**********************************************************************/

namespace Graphics {

    //----------------------------------------------------------------------
    void IMaterial::setShader( const ShaderPtr& shader )
    { 
        ASSERT( shader );
        if (m_shader == shader)
            return;

        m_shader = shader; 
        m_typeMap.clear();
        m_intMap.clear();
        m_floatMap.clear();
        m_vec4Map.clear();
        m_matrixMap.clear();
        m_textureMap.clear();
        _ChangedShader(); 
    }
    
    //**********************************************************************
    // MATERIAL PARAMETERS - GET
    //**********************************************************************

    //----------------------------------------------------------------------
    I32 IMaterial::getInt( StringID name ) const 
    { 
        if ( m_intMap.find( name ) != m_intMap.end() )
            return m_intMap.at( name );

        LOG_WARN_RENDERING( "Material::getInt(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
        return 0;
    }

    //----------------------------------------------------------------------
    F32 IMaterial::getFloat( StringID name ) const 
    { 
        if ( m_floatMap.find( name ) != m_floatMap.end() )
            return m_floatMap.at( name );

        LOG_WARN_RENDERING( "Material::getFloat(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
        return 0.0f;
    }

    //----------------------------------------------------------------------
    Math::Vec4 IMaterial::getVec4( StringID name ) const
    {
        if ( m_vec4Map.find( name ) != m_vec4Map.end() )
            return m_vec4Map.at( name );

        LOG_WARN_RENDERING( "Material::getVec4(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
        return Math::Vec4( 0.0f );
    }

    //----------------------------------------------------------------------
    DirectX::XMMATRIX IMaterial::getMatrix( StringID name ) const
    {
        if ( m_matrixMap.find( name ) != m_matrixMap.end() )
            return m_matrixMap.at( name );

        LOG_WARN_RENDERING( "Material::getMatrix(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
        return DirectX::XMMatrixIdentity();
    }

    //----------------------------------------------------------------------
    Color IMaterial::getColor( StringID name ) const
    {
        if ( m_vec4Map.find( name ) != m_vec4Map.end() )
        {
            Math::Vec4 colorAsVec = m_vec4Map.at( name );
            return Color( (Byte) (colorAsVec.x * 255.0f), (Byte) (colorAsVec.y * 255.0f), (Byte) (colorAsVec.z * 255.0f), (Byte) (colorAsVec.w * 255.0f) );
        }

        LOG_WARN_RENDERING( "Material::getColor(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
        return Color::BLACK;
    }

    //----------------------------------------------------------------------
    TexturePtr IMaterial::getTexture( StringID name ) const
    {
        if ( m_textureMap.find( name ) != m_textureMap.end() )
            return m_textureMap.at( name );

        LOG_WARN_RENDERING( "Material::getTexture(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
        return nullptr;
    }

    //----------------------------------------------------------------------
    DataType IMaterial::getDataType( StringID name ) const
    {
        if ( m_typeMap.find( name ) != m_typeMap.end() )
            return m_typeMap.at( name );

        return DataType::Unknown;
    }

    //**********************************************************************
    // MATERIAL PARAMETERS - SET
    //**********************************************************************
    
    //----------------------------------------------------------------------
    void IMaterial::setInt( StringID name, I32 val )
    { 
        if ( _SetInt( name, val ) )
            m_intMap[ name ] = val;
        else
            LOG_WARN_RENDERING( "Material::setInt(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::setFloat( StringID name, F32 val )
    { 
        if ( _SetFloat( name, val ) )
            m_floatMap[ name ] = val;
        else
            LOG_WARN_RENDERING( "Material::setFloat(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::setVec4( StringID name, const Math::Vec4& vec )
    { 
        if ( _SetVec4( name, vec ) )
            m_vec4Map[ name ] = vec; 
        else
            LOG_WARN_RENDERING( "Material::setVec4(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::setMatrix( StringID name, const DirectX::XMMATRIX& matrix )
    { 
        if ( _SetMatrix( name, matrix ) )
            m_matrixMap[ name ] = matrix;
        else
            LOG_WARN_RENDERING( "Material::setMatrix(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::setColor( StringID name, Color color )
    { 
        auto normalized = color.normalized();
        Math::Vec4 colorAsVec( normalized[0], normalized[1], normalized[2], normalized[3] );
        if ( _SetVec4( name, colorAsVec ) )
            m_vec4Map[ name ] = colorAsVec;
        else
            LOG_WARN_RENDERING( "Material::setColor(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IMaterial::setTexture( StringID name, const TexturePtr& texture )
    { 
        if ( _SetTexture( name, texture ) )
            m_textureMap[ name ] = texture;
        else
            LOG_WARN_RENDERING( "Material::setTexture(): Name '" + name.toString() + "' does not exist in shader '" + m_shader->getName() + "'" );
    }

} // End namespaces