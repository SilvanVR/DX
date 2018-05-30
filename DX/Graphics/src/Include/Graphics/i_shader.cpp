#include "i_shader.h"
/**********************************************************************
    class: IShader (i_shader.cpp)

    author: S. Hau
    date: May 29, 2018
**********************************************************************/

#include "Logging/logging.h"

namespace Graphics {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    DataType IShader::getDataTypeOfMaterialProperty( StringID name )
    {
        auto typeVS = DataType::Unknown;
        if ( auto VSBuffer = getVSUniformMaterialBuffer() )
            if ( auto member = VSBuffer->getMember( name ) )
                typeVS = member->getDataType();

        auto typeFS = DataType::Unknown;
        if ( auto FSBuffer = getFSUniformMaterialBuffer() )
            if ( auto member = FSBuffer->getMember( name ) )
                typeFS = member->getDataType();

        if (typeVS != DataType::Unknown && typeFS != DataType::Unknown)
            LOG_WARN_RENDERING( "IShader::getDataTypeOfMaterialProperty(): A IShader property exists in multiple shader stages. "
                                "This might cause issues. Consider renaming one of the properties with name: " + name.toString() );

        if (typeVS != DataType::Unknown)
            return typeVS;
        else if (typeFS != DataType::Unknown)
            return typeFS;

        return DataType::Unknown;
    }

    //----------------------------------------------------------------------
    DataType IShader::getDataTypeOfMaterialPropertyOrResource( StringID name )
    {
        auto typeMaterial = getDataTypeOfMaterialProperty( name );

        auto typeResource = DataType::Unknown;
        if ( auto shaderResource = getShaderResource(name) )
            typeResource = shaderResource->getDataType();

        if ( typeMaterial != DataType::Unknown && typeResource != DataType::Unknown )
            LOG_WARN_RENDERING( "IShader::getDataTypeOfMaterialPropertyOrResource(): A IShader property exists in a IShader buffer and as a resource. "
                                "This might cause issues. Consider renaming one of them with name: " + name.toString());

        if (typeMaterial != DataType::Unknown)
            return typeMaterial;
        else if (typeResource != DataType::Unknown)
            return typeResource;

        return DataType::Unknown;
    }

    //----------------------------------------------------------------------
    DataType IShader::getDataTypeOfShaderProperty( StringID name )
    {
        auto typeVS = DataType::Unknown;
        if ( auto VSBuffer = getVSUniformShaderBuffer() )
            if ( auto member = VSBuffer->getMember( name ) )
                typeVS = member->getDataType();

        auto typeFS = DataType::Unknown;
        if ( auto FSBuffer = getFSUniformShaderBuffer() )
            if ( auto member = FSBuffer->getMember( name ) )
                typeFS = member->getDataType();

        if (typeVS != DataType::Unknown && typeFS != DataType::Unknown)
            LOG_WARN_RENDERING( "IShader::getDataTypeOfMaterialProperty(): A IShader property exists in multiple shader stages. "
                                "This might cause issues. Consider renaming one of the properties with name: " + name.toString() );

        if (typeVS != DataType::Unknown)
            return typeVS;
        else if (typeFS != DataType::Unknown)
            return typeFS;

        return DataType::Unknown;
    }

    //**********************************************************************
    // SHADER PARAMETERS - GET
    //**********************************************************************

    //----------------------------------------------------------------------
    I32 IShader::getInt( StringID name ) const
    { 
        if ( m_intMap.find( name ) != m_intMap.end() )
            return m_intMap.at( name );

        LOG_WARN_RENDERING( "IShader::getInt(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
        return 0;
    }

    //----------------------------------------------------------------------
    F32 IShader::getFloat( StringID name ) const
    { 
        if ( m_floatMap.find( name ) != m_floatMap.end() )
            return m_floatMap.at( name );

        LOG_WARN_RENDERING( "IShader::getFloat(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
        return 0.0f;
    }

    //----------------------------------------------------------------------
    Math::Vec4 IShader::getVec4( StringID name ) const
    {
        if ( m_vec4Map.find( name ) != m_vec4Map.end() )
            return m_vec4Map.at( name );

        LOG_WARN_RENDERING( "IShader::getVec4(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
        return Math::Vec4( 0.0f );
    }

    //----------------------------------------------------------------------
    DirectX::XMMATRIX IShader::getMatrix( StringID name ) const
    {
        if ( m_matrixMap.find( name ) != m_matrixMap.end() )
            return m_matrixMap.at( name );

        LOG_WARN_RENDERING( "IShader::getMatrix(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
        return DirectX::XMMatrixIdentity();
    }

    //----------------------------------------------------------------------
    Color IShader::getColor( StringID name ) const
    {
        if ( m_vec4Map.find( name ) != m_vec4Map.end() )
        {
            Math::Vec4 colorAsVec = m_vec4Map.at( name );
            return Color( (Byte) (colorAsVec.x * 255.0f), (Byte) (colorAsVec.y * 255.0f), (Byte) (colorAsVec.z * 255.0f), (Byte) (colorAsVec.w * 255.0f) );
        }

        LOG_WARN_RENDERING( "IShader::getColor(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
        return Color::BLACK;
    }

    //----------------------------------------------------------------------
    TexturePtr IShader::getTexture( StringID name ) const
    {
        if ( m_textureMap.find( name ) != m_textureMap.end() )
            return m_textureMap.at( name );

        LOG_WARN_RENDERING( "IShader::getTexture(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
        return nullptr;
    }

    //**********************************************************************
    // SHADER PARAMETERS - SET
    //**********************************************************************

    //----------------------------------------------------------------------
    void IShader::setInt( StringID name, I32 val )
    { 
        if ( not hasShaderProperty( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setInt(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return;
        }

        m_intMap[ name ] = val;
        _SetInt( name, val ); 
    }

    //----------------------------------------------------------------------
    void IShader::setFloat( StringID name, F32 val )
    { 
        if ( not hasShaderProperty( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setFloat(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return;
        }

        m_floatMap[ name ] = val;
        _SetFloat( name, val );
    }

    //----------------------------------------------------------------------
    void IShader::setVec4( StringID name, const Math::Vec4& vec )
    { 
        if ( not hasShaderProperty( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setVec4(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return;
        }

        m_vec4Map[ name ] = vec;
        _SetVec4( name, vec );
    }

    //----------------------------------------------------------------------
    void IShader::setMatrix( StringID name, const DirectX::XMMATRIX& matrix )
    { 
        if ( not hasShaderProperty( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setMatrix(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return;
        }

        m_matrixMap[ name ] = matrix;
        _SetMatrix( name, matrix ); 
    }

    //----------------------------------------------------------------------
    void IShader::setColor( StringID name, Color color )
    { 
        if ( not hasShaderProperty( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setColor(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return;
        }

        auto normalized = color.normalized();
        Math::Vec4 colorAsVec( normalized[0], normalized[1], normalized[2], normalized[3] );
        m_vec4Map[ name ] = colorAsVec;
        _SetVec4( name, colorAsVec );
    }

    //----------------------------------------------------------------------
    void IShader::setTexture( StringID name, const TexturePtr& tex )
    {
        if ( not getShaderResource( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setTexture(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return;
        }

        m_textureMap[ name ] = tex;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void IShader::_BindTextures()
    {
        for (auto& pair : m_textureMap)
        {
            auto shaderRes = getShaderResource( pair.first );
            if (shaderRes) // shader res can be null when the shaders was reloaded but the res no longer exists in it
                pair.second->bind( shaderRes->getShaderType(), shaderRes->getBindingSlot() );
        }
    }

} // End namespaces