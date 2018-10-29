#include "i_shader.h"
/**********************************************************************
    class: IShader (i_shader.cpp)

    author: S. Hau
    date: May 29, 2018
**********************************************************************/

#include "Logging/logging.h"
#include "Common/string_utils.h"

#define MATERIAL_NAME "material"
#define SHADER_NAME   "shader"

namespace Graphics {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    DataType IShader::getDataTypeOfMaterialProperty( StringID name ) const
    {
        for (auto& ubo : m_uniformBuffers)
        {
            String lower = StringUtils::toLower( ubo.getName().toString() );
            if (lower.find( MATERIAL_NAME ) == String::npos)
                continue;

            if (auto member = ubo.getMember(name))
                return member->getDataType();
        }
        return DataType::Unknown;
    }

    //----------------------------------------------------------------------
    DataType IShader::getDataTypeOfMaterialPropertyOrResource( StringID name ) const
    {
        auto typeMaterial = getDataTypeOfMaterialProperty( name );

        auto typeResource = DataType::Unknown;
        if ( auto shaderResource = getShaderResource( name ) )
            typeResource = shaderResource->getDataType();

        if ( typeMaterial != DataType::Unknown && typeResource != DataType::Unknown )
            LOG_WARN_RENDERING( "IShader::getDataTypeOfMaterialPropertyOrResource(): A shader property exists in a shader buffer and as a resource. "
                                "This might cause issues. Consider renaming one of them with name: " + name.toString());

        if (typeMaterial != DataType::Unknown)
            return typeMaterial;
        else if (typeResource != DataType::Unknown)
            return typeResource;

        return DataType::Unknown;
    }

    //----------------------------------------------------------------------
    DataType IShader::getDataTypeOfShaderMember( StringID name ) const
    {
        for ( auto& ubo : m_uniformBuffers)
            if ( auto member = ubo.getMember( name ) )
                return member->getDataType();
        return DataType::Unknown;
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* IShader::getVSUniformMaterialBuffer() const
    {
        return _GetUniformBuffer( MATERIAL_NAME, ShaderType::Vertex );
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* IShader::getFSUniformMaterialBuffer() const
    {
        return _GetUniformBuffer( MATERIAL_NAME, ShaderType::Fragment );
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* IShader::getGSUniformMaterialBuffer() const
    {
        return _GetUniformBuffer( MATERIAL_NAME, ShaderType::Geometry );
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* IShader::getVSUniformShaderBuffer() const
    {
        return _GetUniformBuffer( SHADER_NAME, ShaderType::Vertex );
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* IShader::getFSUniformShaderBuffer() const
    {
        return _GetUniformBuffer( SHADER_NAME, ShaderType::Fragment );
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* IShader::getGSUniformShaderBuffer() const
    {
        return _GetUniformBuffer( SHADER_NAME, ShaderType::Geometry );
    }

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* IShader::getUniformBufferDecl( StringID name ) const
    {
        for (auto& ubo : m_uniformBuffers)
            if (ubo.getName() == name)
                return &ubo;
        return nullptr;
    }

    //----------------------------------------------------------------------
    const ShaderResourceDeclaration* IShader::getShaderResource( StringID name ) const
    {
        for (auto& res : m_shaderResources)
            if (res.getName() == name)
                return &res;
        return nullptr;
    }

    //**********************************************************************
    // PROTECTED
    //**********************************************************************

    //----------------------------------------------------------------------
    void IShader::_WarnMissingInt( StringID name ) const
    {
        LOG_WARN_RENDERING( "IShader::getInt(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IShader::_WarnMissingFloat( StringID name ) const
    {
        LOG_WARN_RENDERING( "IShader::getFloat(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IShader::_WarnMissingColor( StringID name ) const
    {
        LOG_WARN_RENDERING( "IShader::getColor(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IShader::_WarnMissingVec4( StringID name ) const
    {
        LOG_WARN_RENDERING( "IShader::getVec4(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IShader::_WarnMissingMatrix( StringID name ) const
    {
        LOG_WARN_RENDERING( "IShader::getMatrix(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    void IShader::_WarnMissingTexture( StringID name ) const
    {
        LOG_WARN_RENDERING( "IShader::getTexture(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
    }

    //----------------------------------------------------------------------
    bool IShader::_HasShaderInt( StringID name ) const
    {
        if ( not hasShaderProperty( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setInt(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool IShader::_HasShaderFloat( StringID name ) const
    {
        if ( not hasShaderProperty( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setFloat(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool IShader::_HasShaderColor( StringID name ) const
    {
        if ( not hasShaderProperty( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setColor(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool IShader::_HasShaderVec4( StringID name ) const
    {
        if ( not hasShaderProperty( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setVec4(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool IShader::_HasShaderMatrix( StringID name ) const
    {
        if ( not hasShaderProperty( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setMatrix(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    bool IShader::_HasShaderTexture( StringID name ) const
    {
        if ( not getShaderResource( name ) )
        {
            LOG_WARN_RENDERING( "IShader::setTexture(): Name '" + name.toString() + "' does not exist in shader '" + getName() + "'" );
            return false;
        }
        return true;
    }

    //----------------------------------------------------------------------
    void IShader::_BindTextures()
    {
        for (auto& pair : m_textureMap)
        {
            auto shaderRes = getShaderResource( pair.first );
            if (shaderRes) // shader res can be null when the shaders was reloaded but the res no longer exists in it
                pair.second->bind( *shaderRes );
        }
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* IShader::_GetUniformBuffer( const String& name, Graphics::ShaderType shaderType ) const
    {
        for (auto& ubo : m_uniformBuffers)
        {
            if (not (I32)(ubo.getShaderStages() & shaderType))
                continue; // Skip if ubo is not in given shader-stage

            String lower = StringUtils::toLower( ubo.getName().toString() );
            if ( lower.find( name ) != String::npos )
                return &ubo;
        }
        return nullptr;
    }

} // End namespaces