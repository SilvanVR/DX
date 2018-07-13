#include "D3D11VertexShader.h"
/**********************************************************************
    class: VertexShader (D3D11VertexShader.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "D3D11/D3D11Defines.hpp"

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
        HashMap<String, U32> inputSlotMap; // This ensures every float4x4 matrix has the same input slot
        for (U32 i = 0; i < shaderDesc.InputParameters; i++)
        {
            D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
            HR( m_pShaderReflection->GetInputParameterDesc( i, &paramDesc ) );

            D3D11_INPUT_ELEMENT_DESC elementDesc;
            elementDesc.SemanticName            = paramDesc.SemanticName;
            elementDesc.SemanticIndex           = paramDesc.SemanticIndex;
            elementDesc.AlignedByteOffset       = D3D11_APPEND_ALIGNED_ELEMENT;
            elementDesc.InputSlotClass          = D3D11_INPUT_PER_VERTEX_DATA;
            elementDesc.InstanceDataStepRate    = 0;

            bool instanced = false;

            // Check if semanticName ends with "SEMANTIC_INSTANCED" 
            String semanticName = paramDesc.SemanticName;
            Size pos = semanticName.find( SEMANTIC_INSTANCED );
            constexpr Size sizeOfInstancedName = (sizeof(SEMANTIC_INSTANCED) / sizeof(char)) - 1;
            Size posIfNameIsAtEnd = semanticName.size() - sizeOfInstancedName;
            instanced = (pos != String::npos) && (pos == posIfNameIsAtEnd);
            if (instanced)
            {
                // Cut-off the "SEMANTIC_INSTANCED" for _AddToVertexLayout()
                semanticName = semanticName.substr( 0, pos );

                elementDesc.InputSlotClass          = D3D11_INPUT_PER_INSTANCE_DATA;
                elementDesc.InstanceDataStepRate    = 1;
            }

            // Store increasing slot number in map and set input slot
            if ( inputSlotMap.find(semanticName) == inputSlotMap.end() )
                inputSlotMap[semanticName] = static_cast<U32>( inputSlotMap.size() );
            elementDesc.InputSlot = inputSlotMap[semanticName];

            // determine DXGI format
            U32 sizeInBytes = 0;
            if (paramDesc.Mask == 1)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
                sizeInBytes = 4;
            }
            else if (paramDesc.Mask <= 3)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
                sizeInBytes = 8;
            }
            else if (paramDesc.Mask <= 7)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                sizeInBytes = 12;
            }
            else if (paramDesc.Mask <= 15)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                sizeInBytes = 16;
            }

            _AddToVertexLayout( semanticName.c_str(), paramDesc.SemanticIndex, sizeInBytes, instanced );

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
    void VertexShader::_AddToVertexLayout( const String& semanticName, U32 semanticIndex, U32 sizeInBytes, bool instanced )
    {
        if ( semanticName.substr(0,3) == SEMANTIC_SYSTEM ) // Skip system semantics
            return;

        ASSERT( sizeInBytes > 0 && "This should never happen" );

        auto nameAsID = SID( semanticName.c_str() );
        if (semanticIndex > 0) // e.g. float4x4 has four semantic-indices, so we just add the size to that input
        {
            for (auto& input : m_vertexLayout.getLayoutDescription())
                if (input.name == nameAsID)
                {
                    input.sizeInBytes += sizeInBytes;
                    return;
                }
            ASSERT( false && "This should logically never happen" );
        }

        InputLayoutDescription input;
        input.instanced     = instanced;
        input.sizeInBytes   = sizeInBytes;
        input.name          = nameAsID;
        m_vertexLayout.add( input );
    }


} } // End namespaces