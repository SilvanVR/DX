#include "D3D11ShaderBase.h"
/**********************************************************************
    class: ShaderBase (D3D11ShaderBase.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "Common/string_utils.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    const ConstantBufferInfo& ShaderBase::getMaterialBufferInformation() const
    {
        ASSERT( hasMaterialBuffer() && "This shader has no material cb!" );

        for ( auto& cb : m_constantBufferInformation )
        {
            String lower = StringUtils::toLower( cb.name.toString() );
            if ( lower.find("material") != String::npos )
                return cb;
        }

        return m_constantBufferInformation[0];
    }

    //----------------------------------------------------------------------
    bool ShaderBase::hasMaterialBuffer() const
    {
        for ( auto& cb : m_constantBufferInformation )
        {
            String lower = StringUtils::toLower( cb.name.toString() );
            if ( lower.find("material") != String::npos )
                return true;
        }
        return false;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    bool ShaderBase::_Compile( CString profile )
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
    #ifdef _DEBUG
        flags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
    #endif

        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompileFromFile( ConvertToWString( m_filePath.toString() ).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                         m_entryPoint.c_str(), profile, flags, 0, &m_pShaderBlob, &errorBlob );

        if ( FAILED( hr ) )
        {
            if (errorBlob)
            {
                WARN_RENDERING( "Failed to compile Shader '" + m_filePath.toString() + "'." );
                WARN_RENDERING( (const char*)errorBlob->GetBufferPointer() );
                SAFE_RELEASE( errorBlob );
            }
            else
            {
                WARN_RENDERING( "Missing shader-file: '" + m_filePath.toString() + "'.");
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
        HR( D3DReflect( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(),
                        IID_ID3D11ShaderReflection, (void**)&m_pShaderReflection) );

        _ReflectConstantBuffers();
    }

    //----------------------------------------------------------------------
    void ShaderBase::_ReflectConstantBuffers()
    {
        // Make sure old buffer information is cleaned up
        m_constantBufferInformation.clear();

        D3D11_SHADER_DESC shaderDesc;
        HR( m_pShaderReflection->GetDesc( &shaderDesc ) );

        // Reflect constant buffers
        for (U32 i = 0; i < shaderDesc.ConstantBuffers; i++)
        {
            auto buffer = m_pShaderReflection->GetConstantBufferByIndex( i );

            D3D11_SHADER_BUFFER_DESC bufferDesc;
            HR( buffer->GetDesc( &bufferDesc ) );

            ConstantBufferInfo bufferInfo;
            bufferInfo.name         = SID( bufferDesc.Name );
            bufferInfo.sizeInBytes  = bufferDesc.Size;
            bufferInfo.slot         = i; // ASSUME CB's in order @TODO: GET REAL BINDING SLOT

            for (U32 j = 0; j < bufferDesc.Variables; j++)
            {
                auto var = buffer->GetVariableByIndex( j );

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

            m_constantBufferInformation.emplace_back( bufferInfo );
        }
    }

} } // End namespaces