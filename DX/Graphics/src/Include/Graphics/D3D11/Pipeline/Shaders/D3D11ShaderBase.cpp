#include "D3D11ShaderBase.h"
/**********************************************************************
    class: ShaderBase (D3D11ShaderBase.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "Common/string_utils.h"
#include "D3D11/D3D11Defines.hpp"
#include "OS/FileSystem/file_system.h"
#include "OS/FileSystem/file.h"
#include "Utils/utils.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    UINT GetCompileFlags()
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#ifdef _DEBUG
        flags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
#endif
        return flags;
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    const ShaderUniformBufferDeclaration* ShaderBase::getUniformBufferDeclaration( StringID name ) const
    {
        auto it = std::find_if( m_constantBuffers.begin(), m_constantBuffers.end(), [name](const ShaderUniformBufferDeclaration& ubo) {
            return ubo.getName() == name;
        } );
        if ( it == m_constantBuffers.end() )
            return nullptr;

        return &(*it);
    }

    //----------------------------------------------------------------------
    const ShaderResourceDeclaration* ShaderBase::getResourceDeclaration( StringID name ) const
    {
        for (auto& decl : m_resourceDeclarations)
        {
            if (decl.getName() == name)
                return &decl;
        }
        return nullptr;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void ShaderBase::_CompileFromFile( const OS::Path& path, CString entryPoint, std::function<void(const ShaderBlob&)> fn )
    {
        if ( not path.exists() )
            throw std::runtime_error("Missing shader-file: '" + path.toString() + "'.");

        ComPtr<ID3DBlob> d3d11ShaderBlob;
        ComPtr<ID3DBlob> d3D11ErrorBlob;
        HRESULT hr = D3DCompileFromFile( ConvertToWString( path.toString() ).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                         entryPoint, _GetLatestProfile().c_str(), GetCompileFlags(), 0, &d3d11ShaderBlob.get(), &d3D11ErrorBlob.get() );

        String shaderName = GetShaderTypeName( m_shaderType );
        if ( FAILED( hr ) )
        {
            if (d3D11ErrorBlob)
                throw std::runtime_error( "Failed to compile "+ shaderName + " shader from source:\n" + (const char*)d3D11ErrorBlob->GetBufferPointer() );
            throw std::runtime_error( "Failed to compile " + shaderName + " shader from source." );
        }

        ShaderBlob shaderBlob{ d3d11ShaderBlob->GetBufferPointer(), d3d11ShaderBlob->GetBufferSize() };

        _ShaderReflection( shaderBlob );
        fn( shaderBlob );
    }

    //----------------------------------------------------------------------
    void ShaderBase::_CompileFromSource( const String& source, CString entryPoint, std::function<void(const ShaderBlob&)> fn )
    {
        String shaderName = GetShaderTypeName( m_shaderType );

        StringID hash = SID( source.c_str() );

#ifdef _DEBUG
        OS::Path binaryShaderPath( "/engine/shaders/bin/debug/" + shaderName + TS( hash.id ) + ".cso" );
#else
        OS::Path binaryShaderPath( "/engine/shaders/bin/release/" + shaderName + TS( hash.id ) + ".cso" );
#endif // DEBUG

        // Precompiled binary file does not exist
        if ( not OS::FileSystem::exists( binaryShaderPath ) )
        {
            ComPtr<ID3DBlob> d3d11ShaderBlob;
            ComPtr<ID3DBlob> d3D11ErrorBlob;
            HRESULT hr = D3DCompile( source.c_str(), source.size(), NULL, NULL, NULL,
                                     entryPoint, _GetLatestProfile().c_str(), GetCompileFlags(), 0, &d3d11ShaderBlob.get(), &d3D11ErrorBlob.get() );

            if ( FAILED( hr ) )
            {
                if (d3D11ErrorBlob)
                    throw std::runtime_error( "Failed to compile "+ shaderName + " shader from source:\n" + (const char*)d3D11ErrorBlob->GetBufferPointer() );
                throw std::runtime_error( "Failed to compile " + shaderName + " shader from source." );
            }

            ShaderBlob shaderBlob{ d3d11ShaderBlob->GetBufferPointer(), d3d11ShaderBlob->GetBufferSize() };

            // Store compiled binary data into file
            OS::BinaryFile binaryShaderFile( binaryShaderPath, OS::EFileMode::WRITE );
            binaryShaderFile.write( (const Byte*)shaderBlob.data, shaderBlob.size );

            _ShaderReflection( shaderBlob );
            fn( shaderBlob );
        }
        else
        {
            // Load compiled binary data from file
            OS::BinaryFile binaryShaderFile( binaryShaderPath, OS::EFileMode::READ );
            String content = binaryShaderFile.readAll();

            ShaderBlob shaderBlob{ content.data(), content.size() };
            _ShaderReflection( shaderBlob );
            fn( shaderBlob );
        }
    }

    //----------------------------------------------------------------------
    void ShaderBase::_ShaderReflection( const ShaderBlob& shaderBlob )
    {
        // Perform reflection
        HR( D3DReflect( shaderBlob.data, shaderBlob.size,
                        IID_ID3D11ShaderReflection, (void**)&m_pShaderReflection.releaseAndGet() ) );

        D3D11_SHADER_DESC shaderDesc;
        HR( m_pShaderReflection->GetDesc( &shaderDesc ) );

        _ReflectResources( shaderDesc );

        // Add constant buffers to the manager, which creates real buffers for it depending on specific keywords
        ConstantBufferManager::ReflectConstantBuffers( m_constantBuffers );
    }

    //----------------------------------------------------------------------
    void ShaderBase::_ReflectResources( const D3D11_SHADER_DESC& shaderDesc )
    {
        // Make sure old buffer information is no longer valid
        m_constantBuffers.clear();
        m_resourceDeclarations.clear();

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
                DataType type = DataType::Unknown;
                switch (bindDesc.Dimension)
                {
                case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
                case D3D11_SRV_DIMENSION_TEXTURE1D:         type = DataType::Texture1D; break;
                case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
                case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
                case D3D11_SRV_DIMENSION_TEXTURE2D:         type = DataType::Texture2D; break;
                case D3D11_SRV_DIMENSION_TEXTURE3D:         type = DataType::Texture3D; break;
                case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:
                case D3D11_SRV_DIMENSION_TEXTURECUBE:       type = DataType::TextureCubemap; break;
                }
                ASSERT( type != DataType::Unknown && "Could not deduce texture type." );

                ShaderResourceDeclaration decl( m_shaderType, bindDesc.BindPoint, SID( bindDesc.Name ), type );
                m_resourceDeclarations.push_back( decl );
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

        ShaderUniformBufferDeclaration buffer( m_shaderType, SID( bufferDesc.Name ), bindSlot, bufferDesc.Size );

        for (U32 j = 0; j < bufferDesc.Variables; j++)
        {
            auto var = cb->GetVariableByIndex( j );

            D3D11_SHADER_VARIABLE_DESC varDesc;
            HR( var->GetDesc( &varDesc ) );

            ShaderUniformDeclaration uniform( SID( varDesc.Name ), varDesc.StartOffset, varDesc.Size, _GetDataType( var ) );
            buffer._AddUniformDecl( uniform );
        }
        m_constantBuffers.push_back( buffer );
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

    //----------------------------------------------------------------------
    String ShaderBase::_GetLatestProfile()
    {
        switch (m_shaderType)
        {
        case ShaderType::Vertex:    return GetLatestProfile<ID3D11VertexShader>();
        case ShaderType::Fragment:  return GetLatestProfile<ID3D11PixelShader>();
        case ShaderType::Geometry:  return GetLatestProfile<ID3D11GeometryShader>();
        ASSERT( false && "Unsupported shader!" );
        }
        return "";
    }

} } // End namespaces