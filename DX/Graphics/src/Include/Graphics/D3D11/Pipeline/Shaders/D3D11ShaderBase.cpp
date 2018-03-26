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
    const ConstantBufferInfo& ShaderBase::getMaterialBufferInfo() const
    {
        ASSERT( hasMaterialBuffer() && "This shader has no material cb!" );

        for ( auto& cb : m_constantBufferInfos )
        {
            String lower = StringUtils::toLower( cb.name.toString() );
            if ( lower.find( MATERIAL_NAME ) != String::npos )
                return cb;
        }

        return m_constantBufferInfos[0];
    }

    //----------------------------------------------------------------------
    bool ShaderBase::hasMaterialBuffer() const
    {
        for ( auto& cb : m_constantBufferInfos )
        {
            String lower = StringUtils::toLower( cb.name.toString() );
            if ( lower.find( MATERIAL_NAME ) != String::npos )
                return true;
        }
        return false;
    }

    //----------------------------------------------------------------------
    bool ShaderBase::isUpToDate() const 
    { 
        if ( m_filePath.empty() ) // Compiled shaders from source are always up to date
            return true;

        return m_fileTimeAtCompilation == m_filePath.getLastWrittenFileTime(); 
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
            WARN_RENDERING( "Shader::recompile(): Recompilation not possible because no filepath exists in this class! "
                            "Maybe the shader was compiled directly from source?");
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
            WARN_RENDERING( "Failed to compile shader from source '" + source + "'." );
            if (errorBlob)
            {
                WARN_RENDERING( (const char*)errorBlob->GetBufferPointer() );
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
             WARN_RENDERING( "Missing shader-file: '" + path.toString() + "'.");
             return false;
        }

        SAFE_RELEASE( m_pShaderBlob );

        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompileFromFile( ConvertToWString( path.toString() ).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                         m_entryPoint.c_str(), profile, GetCompileFlags(), 0, &m_pShaderBlob, &errorBlob );

        if ( FAILED( hr ) )
        {
            WARN_RENDERING( "Failed to compile shader '" + path.toString() + "'." );
            if (errorBlob)
            {
                WARN_RENDERING( (const char*)errorBlob->GetBufferPointer() );
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
        m_constantBufferInfos.clear();
        m_boundTextureInfos.clear();

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
                texInfo.name = SID( bindDesc.Name );
                texInfo.slot = bindDesc.BindPoint;
                m_boundTextureInfos.emplace_back( texInfo );
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
        bufferInfo.name         = SID( bufferDesc.Name );
        bufferInfo.sizeInBytes  = bufferDesc.Size;
        bufferInfo.slot         = bindSlot;

        for (U32 j = 0; j < bufferDesc.Variables; j++)
        {
            auto var = cb->GetVariableByIndex( j );

            D3D11_SHADER_VARIABLE_DESC varDesc;
            HR( var->GetDesc( &varDesc ) );

            // If type information is needed
            //auto type = var->GetType();
            //D3D11_SHADER_TYPE_DESC typeDesc;
            //HR( type->GetDesc( &typeDesc ) );

            ConstantBufferMemberInfo info = {};
            info.name   = SID( varDesc.Name );
            info.offset = varDesc.StartOffset;
            info.size   = varDesc.Size;
            bufferInfo.members.emplace_back( info );
        }
        m_constantBufferInfos.emplace_back( bufferInfo );
    }



} } // End namespaces