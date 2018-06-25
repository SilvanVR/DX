#include "D3D11VertexShader.h"
/**********************************************************************
    class: VertexShader (D3D11VertexShader.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "D3D11/D3D11Defines.hpp"
#include "OS/FileSystem/file_system.h"
#include "OS/FileSystem/file.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void VertexShader::bind() 
    { 
        ASSERT( m_pVertexShader.get() != nullptr );
        g_pImmediateContext->IASetInputLayout( m_pInputLayout.get() );
        g_pImmediateContext->VSSetShader( m_pVertexShader.get(), NULL, 0 ); 
    }

    //----------------------------------------------------------------------
    void VertexShader::unbind()
    { 
        g_pImmediateContext->VSSetShader( NULL, NULL, 0 ); 
    }

    //----------------------------------------------------------------------
    void VertexShader::compileFromFile( const OS::Path& path, CString entryPoint )
    {
        _CompileFromFile( path, entryPoint, [this](const ShaderBlob& shaderBlob) {
            _CreateD3D11VertexShader( shaderBlob );
        } );
    }

    //----------------------------------------------------------------------
    void VertexShader::compileFromSource( const String& source, CString entryPoint )
    {
        _CompileFromSource( source, entryPoint, [this] (const ShaderBlob& shaderBlob) {
            _CreateD3D11VertexShader( shaderBlob );
        } );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void VertexShader::_CreateD3D11VertexShader( const ShaderBlob& shaderBlob )
    {
        // Clean-Up old vertex-shader and create a new one
        HR( g_pDevice->CreateVertexShader( shaderBlob.data, shaderBlob.size, nullptr, &m_pVertexShader.releaseAndGet() ) );

        _CreateInputLayout( shaderBlob.data, shaderBlob.size );

        // Reflection data no longer needed
        m_pShaderReflection.release();
    }

    //----------------------------------------------------------------------
    void VertexShader::_CreateInputLayout( const void* pShaderByteCode, Size sizeInBytes )
    {
        // Clean up old data
        m_vertexLayout.clear();

        // Get shader info
        D3D11_SHADER_DESC shaderDesc;
        HR( m_pShaderReflection->GetDesc( &shaderDesc ) );

        // Read input layout description from shader info
        ArrayList<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
        for (U32 i = 0; i< shaderDesc.InputParameters; i++)
        {
            D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
            HR( m_pShaderReflection->GetInputParameterDesc( i, &paramDesc ) );

            // Fill out input element desc
            D3D11_INPUT_ELEMENT_DESC elementDesc;
            elementDesc.SemanticName            = paramDesc.SemanticName;
            elementDesc.SemanticIndex           = paramDesc.SemanticIndex;
            elementDesc.InputSlot               = i;
            elementDesc.AlignedByteOffset       = (i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT);
            elementDesc.InputSlotClass          = D3D11_INPUT_PER_VERTEX_DATA;
            elementDesc.InstanceDataStepRate    = 0;

            _AddToVertexLayout( paramDesc.SemanticName, paramDesc.SemanticIndex );

            // determine DXGI format
            if (paramDesc.Mask == 1)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
            }
            else if (paramDesc.Mask <= 3)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
            }
            else if (paramDesc.Mask <= 7)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            }
            else if (paramDesc.Mask <= 15)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            }

            inputLayoutDesc.push_back( elementDesc );
        }

        if ( not m_vertexLayout.isEmpty() )
        {
            // Create Input Layout
            HR( g_pDevice->CreateInputLayout( inputLayoutDesc.data(), (U32)inputLayoutDesc.size(), 
                                              pShaderByteCode, sizeInBytes, &m_pInputLayout.releaseAndGet() ) );
        }
    }

    //----------------------------------------------------------------------
    void VertexShader::_AddToVertexLayout( const String& semanticName, U32 semanticIndex )
    {
        if (semanticName.substr(0,3) == "SV_") // Skip system semantics
            return;

        bool nameExists = false;
        if (semanticName == SEMANTIC_POSITION)
        {
            m_vertexLayout.add( { InputLayoutType::POSITION } );
            nameExists = true;
        }
        else if (semanticName == SEMANTIC_COLOR)
        {
            m_vertexLayout.add( { InputLayoutType::COLOR } );
            nameExists = true;
        }
        else if (semanticName == SEMANTIC_TEXCOORD)
        {
            nameExists = true;
            switch (semanticIndex)
            {
            case 0: m_vertexLayout.add( { InputLayoutType::TEXCOORD0 } ); break;
            case 1: m_vertexLayout.add( { InputLayoutType::TEXCOORD1 } ); break;
            case 2: m_vertexLayout.add( { InputLayoutType::TEXCOORD2 } ); break;
            case 3: m_vertexLayout.add( { InputLayoutType::TEXCOORD3 } ); break;
            default: nameExists = false;
            }
        }
        else if (semanticName == SEMANTIC_NORMAL)
        {
            m_vertexLayout.add({ InputLayoutType::NORMAL });
            nameExists = true;
        }
        else if (semanticName == SEMANTIC_TANGENT)
        {
            m_vertexLayout.add({ InputLayoutType::TANGENT });
            nameExists = true;
        }
        if (not nameExists)
            LOG_WARN_RENDERING( "D3D11VertexShader: Semantic name '" + semanticName + "' for shader '" + getFilePath().toString() + "' does not exist.");
    }


} } // End namespaces