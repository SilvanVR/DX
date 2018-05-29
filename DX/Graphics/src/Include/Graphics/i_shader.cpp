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
            LOG_WARN_RENDERING( "IShader::getDataTypeOfMaterialProperty(): A material property exists in multiple shader stages. "
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
            LOG_WARN_RENDERING( "IShader::getDataTypeOfMaterialPropertyOrResource(): A material property exists in a material buffer and as a resource. "
                                "This might cause issues. Consider renaming one of them with name: " + name.toString());

        if (typeMaterial != DataType::Unknown)
            return typeMaterial;
        else if (typeResource != DataType::Unknown)
            return typeResource;

        return DataType::Unknown;
    }

    //----------------------------------------------------------------------
    TexturePtr IShader::getTexture( StringID name ) const
    {
        if ( m_textureMap.find( name ) != m_textureMap.end() )
            return m_textureMap.at( name );

        LOG_WARN_RENDERING( "IShader::getTexture(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
        return nullptr;
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
            pair.second->bind( shaderRes->getShaderType(), shaderRes->getBindingSlot() );
        }
    }

} // End namespaces