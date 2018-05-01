#include "D3D11ShaderBase.h"
/**********************************************************************
    class: ShaderBase (D3D11ShaderBase.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "Common/string_utils.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    static CString MATERIAL_NAME = "material";

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    const ConstantBufferInfo* ShaderBase::getConstantBufferInfo( StringID name ) const
    {
        if ( m_constantBuffers.find( name ) == m_constantBuffers.end() )
            return nullptr;

        return &m_constantBuffers.at( name );
    }

    //----------------------------------------------------------------------
    const TextureBindingInfo* ShaderBase::getTextureBindingInfo( StringID name ) const
    {
        if ( m_textures.find( name ) == m_textures.end() )
            return nullptr;

        return &m_textures.at( name );
    }

    //----------------------------------------------------------------------
    const ConstantBufferInfo* ShaderBase::getMaterialBufferInfo() const
    {
        for ( auto& pair : m_constantBuffers )
        {
            String lower = StringUtils::toLower( pair.first.toString() );
            if ( lower.find( MATERIAL_NAME ) != String::npos )
                return &pair.second;
        }

        // This shader has no material set
        return nullptr;
    }

    //----------------------------------------------------------------------
    bool ShaderBase::isUpToDate() const 
    { 
        if ( m_filePath.empty() ) // Compiled shaders from source are always up to date
            return true;

        try {
            return m_fileTimeAtCompilation == m_filePath.getLastWrittenFileTime();
        }
        catch (...) {
            // Do nothing, simply return true. Being here means file could not be opened.
        }

        return true; 
    }

    //----------------------------------------------------------------------
    bool ShaderBase::recompile()
    {
        // Recompilation only possible on shaders which were compiled by path
        if ( not m_filePath.toString().empty() )
            if ( compileFromFile( m_filePath, m_entryPoint.c_str() ) )
                return true;
            else
                return false;
        else
            LOG_WARN_RENDERING( "Shader::recompile(): Recompilation not possible because no filepath exists in this class! "
                                "Maybe the shader was compiled directly from source?" );
        return false;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    UINT GetCompileFlags()
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
        #ifdef _DEBUG
            flags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
        #endif
        return flags;
    }

    //----------------------------------------------------------------------
    bool ShaderBase::_CompileFromSource( const String& source, CString profile )
    {
        SAFE_RELEASE( m_pShaderBlob );
        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompile( source.c_str(), source.size(), NULL, NULL, NULL, 
                                 m_entryPoint.c_str(), profile, GetCompileFlags(), 0, &m_pShaderBlob, &errorBlob );

        if ( FAILED( hr ) )
        {
            LOG_WARN_RENDERING( "Failed to compile shader from source:\n" + source );
            if (errorBlob)
            {
                LOG_WARN_RENDERING( (const char*)errorBlob->GetBufferPointer() );
                SAFE_RELEASE( errorBlob );
            }

            SAFE_RELEASE( m_pShaderBlob );

            return false;
        }

        _ShaderReflection( m_pShaderBlob );
        return true;
    }

    //----------------------------------------------------------------------
    bool ShaderBase::_CompileFromFile( const OS::Path& path, CString profile )
    {
        if ( not path.exists() )
        {
             LOG_WARN_RENDERING( "Missing shader-file: '" + path.toString() + "'.");
             return false;
        }

        SAFE_RELEASE( m_pShaderBlob );

        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompileFromFile( ConvertToWString( path.toString() ).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                         m_entryPoint.c_str(), profile, GetCompileFlags(), 0, &m_pShaderBlob, &errorBlob );

        if ( FAILED( hr ) )
        {
            LOG_WARN_RENDERING( "Failed to compile shader '" + path.toString() + "'." );
            if (errorBlob)
            {
                LOG_WARN_RENDERING( (const char*)errorBlob->GetBufferPointer() );
                SAFE_RELEASE( errorBlob );
            }

            SAFE_RELEASE( m_pShaderBlob );

            return false;
        }

        _ShaderReflection( m_pShaderBlob );
        return true;
    }

    //----------------------------------------------------------------------
    void ShaderBase::_ShaderReflection( ID3DBlob* pShaderBlob )
    {
        // Perform reflection
        SAFE_RELEASE( m_pShaderReflection );
        HR( D3DReflect( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(),
                        IID_ID3D11ShaderReflection, (void**)&m_pShaderReflection) );

        D3D11_SHADER_DESC shaderDesc;
        HR( m_pShaderReflection->GetDesc( &shaderDesc ) );

        _ReflectResources( shaderDesc );
    }

    //----------------------------------------------------------------------
    void ShaderBase::_ReflectResources( const D3D11_SHADER_DESC& shaderDesc )
    {
        // Make sure old buffer information is no longer valid
        m_constantBuffers.clear();
        m_textures.clear();

        for (U32 i = 0; i < shaderDesc.BoundResources; i++)
        {
            D3D11_SHADER_INPUT_BIND_DESC bindDesc;
            HR( m_pShaderReflection->GetResourceBindingDesc( i, &bindDesc ) );

            switch (bindDesc.Type)
            {
            case D3D_SIT_CBUFFER:
            {
                auto cb = m_pShaderReflection->GetConstantBufferByName( bindDesc.Name );
                _ReflectConstantBuffer( cb, bindDesc.BindPoint );
                break;
            }
            case D3D_SIT_TEXTURE:
            {
                TextureBindingInfo texInfo;
                texInfo.slot = bindDesc.BindPoint;

                switch (bindDesc.Dimension)
                {
                case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
                case D3D11_SRV_DIMENSION_TEXTURE1D:         texInfo.type = DataType::Texture1D; break;
                case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
                case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
                case D3D11_SRV_DIMENSION_TEXTURE2D:         texInfo.type = DataType::Texture2D; break;
                case D3D11_SRV_DIMENSION_TEXTURE3D:         texInfo.type = DataType::Texture3D; break;
                case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:
                case D3D11_SRV_DIMENSION_TEXTURECUBE:       texInfo.type = DataType::TextureCubemap; break;
                }
                ASSERT(texInfo.type != DataType::Unknown && "Could not deduce texture type.");

                m_textures[ SID( bindDesc.Name ) ] = texInfo;
                break;
            }
            }
        }
    }

    //----------------------------------------------------------------------
    void ShaderBase::_ReflectConstantBuffer( ID3D11ShaderReflectionConstantBuffer* cb, U32 bindSlot )
    {
        D3D11_SHADER_BUFFER_DESC bufferDesc;
        HR( cb->GetDesc( &bufferDesc ) );

        ConstantBufferInfo bufferInfo;
        bufferInfo.sizeInBytes  = bufferDesc.Size;
        bufferInfo.slot         = bindSlot;

        for (U32 j = 0; j < bufferDesc.Variables; j++)
        {
            auto var = cb->GetVariableByIndex( j );

            D3D11_SHADER_VARIABLE_DESC varDesc;
            HR( var->GetDesc( &varDesc ) );

            ConstantBufferMemberInfo info = {};
            info.name   = SID( varDesc.Name );
            info.offset = varDesc.StartOffset;
            info.size   = varDesc.Size;
            info.type   = _GetDataType( var );
            bufferInfo.members.emplace_back( info );
        }
        m_constantBuffers[ SID( bufferDesc.Name ) ] = bufferInfo;
    }

    //----------------------------------------------------------------------
    DataType ShaderBase::_GetDataType( ID3D11ShaderReflectionVariable* var )
    {
        auto type = var->GetType();
        D3D11_SHADER_TYPE_DESC typeDesc;
        HR( type->GetDesc( &typeDesc ) );

        switch (typeDesc.Class)
        {
        case D3D10_SHADER_VARIABLE_CLASS::D3D10_SVC_SCALAR:
        {
            switch (typeDesc.Type)
            {
            case D3D10_SHADER_VARIABLE_TYPE::D3D10_SVT_BOOL:        return DataType::Boolean;
            case D3D10_SHADER_VARIABLE_TYPE::D3D10_SVT_INT:         return DataType::Int;
            case D3D10_SHADER_VARIABLE_TYPE::D3D10_SVT_FLOAT:       return DataType::Float;
            case D3D10_SHADER_VARIABLE_TYPE::D3D11_SVT_DOUBLE:      return DataType::Double;
            }
            break;
        }
        case D3D10_SHADER_VARIABLE_CLASS::D3D10_SVC_OBJECT:
        {
            switch (typeDesc.Type)
            {
            case D3D10_SHADER_VARIABLE_TYPE::D3D10_SVT_TEXTURE1D:   return DataType::Texture1D;
            case D3D10_SHADER_VARIABLE_TYPE::D3D10_SVT_TEXTURE2D:   return DataType::Texture2D;
            case D3D10_SHADER_VARIABLE_TYPE::D3D10_SVT_TEXTURE3D:   return DataType::Texture3D;
            case D3D10_SHADER_VARIABLE_TYPE::D3D10_SVT_TEXTURECUBE: return DataType::TextureCubemap;
            }
        }
        case D3D10_SHADER_VARIABLE_CLASS::D3D10_SVC_VECTOR:
        {
            if (typeDesc.Columns == 2 || typeDesc.Rows == 2)        return DataType::Vec2;
            else if(typeDesc.Columns == 3 || typeDesc.Rows == 3)    return DataType::Vec3;
            else if(typeDesc.Columns == 4 || typeDesc.Rows == 4)    return DataType::Vec4;
            break;
        }
        case D3D10_SHADER_VARIABLE_CLASS::D3D10_SVC_STRUCT:
            return DataType::Struct;
        case D3D10_SHADER_VARIABLE_CLASS::D3D10_SVC_MATRIX_ROWS:
        case D3D10_SHADER_VARIABLE_CLASS::D3D10_SVC_MATRIX_COLUMNS:
            return DataType::Matrix;
        }

        ASSERT( false && "Variable type could not be deduced." );
        return DataType::Unknown;
    }



} } // End namespaces