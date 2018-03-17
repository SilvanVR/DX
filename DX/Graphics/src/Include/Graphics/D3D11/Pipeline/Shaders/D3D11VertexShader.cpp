#include "D3D11VertexShader.h"
/**********************************************************************
    class: VertexShader (D3D11VertexShader.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/



namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    VertexShader::VertexShader( CString path )
        : ShaderBase( path )
    {
    }

    //----------------------------------------------------------------------
    VertexShader::~VertexShader()
    {
        SAFE_RELEASE( m_pVertexShader );
        SAFE_RELEASE( m_pInputLayout );
    }

    //----------------------------------------------------------------------
    void VertexShader::bind() 
    { 
        ASSERT( m_pVertexShader != nullptr );
        g_pImmediateContext->IASetInputLayout( m_pInputLayout );
        g_pImmediateContext->VSSetShader( m_pVertexShader, NULL, 0 ); 
    }

    //----------------------------------------------------------------------
    bool VertexShader::compile( CString entryPoint )
    {
        bool compiled = _Compile<ID3D11VertexShader>( entryPoint );
        if (not compiled)
            return false;

        // Clean-Up old vertex-shader
        SAFE_RELEASE( m_pVertexShader );

        // Create Vertex-Shader and input layout
        HR( g_pDevice->CreateVertexShader( m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_pVertexShader ) );

        // Shader Reflection
        _PerformShaderReflection( m_shaderBlob );

        // Shader blob no longer needed
        SAFE_RELEASE( m_shaderBlob );

        return true;
    }

    //----------------------------------------------------------------------
    void VertexShader::_PerformShaderReflection( ID3DBlob* pShaderBlob )
    {
        // Reflect shader info
        ID3D11ShaderReflection* pShaderReflection = NULL;
        HR( D3DReflect( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), 
                        IID_ID3D11ShaderReflection, (void**)&pShaderReflection) );

        // Get shader info
        D3D11_SHADER_DESC shaderDesc;
        HR( pShaderReflection->GetDesc( &shaderDesc ) );

        _CreateInputLayout( pShaderReflection, shaderDesc );
        _CreateBufferLayout( pShaderReflection, shaderDesc );

        // Free allocation shader reflection memory
        SAFE_RELEASE( pShaderReflection );
    }

    //----------------------------------------------------------------------
    void VertexShader::_CreateInputLayout( ID3D11ShaderReflection* pShaderReflection, const D3D11_SHADER_DESC& shaderDesc )
    {
        // Clean up old data
        SAFE_RELEASE( m_pInputLayout );
        m_vertexLayout.clear();

        // Read input layout description from shader info
        ArrayList<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
        for (U32 i = 0; i< shaderDesc.InputParameters; i++)
        {
            D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
            HR( pShaderReflection->GetInputParameterDesc( i, &paramDesc ) );

            // Fill out input element desc
            D3D11_INPUT_ELEMENT_DESC elementDesc;
            elementDesc.SemanticName            = paramDesc.SemanticName;
            elementDesc.SemanticIndex           = paramDesc.SemanticIndex;
            elementDesc.InputSlot               = i;
            elementDesc.AlignedByteOffset       = (i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT);
            elementDesc.InputSlotClass          = D3D11_INPUT_PER_VERTEX_DATA;
            elementDesc.InstanceDataStepRate    = 0;

            _AddToVertexLayout( paramDesc.SemanticName );

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

        // Vertex layout should never be empty
        ASSERT( not m_vertexLayout.isEmpty() );

        // Create Input Layout
        HR( g_pDevice->CreateInputLayout( inputLayoutDesc.data(), (U32)inputLayoutDesc.size(), 
                                          m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), &m_pInputLayout) );
    }

    //----------------------------------------------------------------------
    void VertexShader::_AddToVertexLayout( String semanticName )
    {
        bool nameExists = false;
        if (semanticName == "POSITION")
        {
            m_vertexLayout.add( { InputLayoutType::POSITION } );
            nameExists = true;
        }
        else if (semanticName == "COLOR")
        {
            m_vertexLayout.add( { InputLayoutType::COLOR } );
            nameExists = true;
        }
        if (not nameExists)
            WARN_RENDERING( "D3D11VertexShader: Semantic name '" + semanticName + "' for shader '" + getFilePath().toString() + "' does not exist.");
    }

    //----------------------------------------------------------------------
    void VertexShader::_CreateBufferLayout( ID3D11ShaderReflection* pShaderReflection, const D3D11_SHADER_DESC& shaderDesc )
    {
        // Reflect constant buffers
        for (U32 i = 0; i < shaderDesc.ConstantBuffers; i++)
        {
            auto buffer = pShaderReflection->GetConstantBufferByIndex( i );

            D3D11_SHADER_BUFFER_DESC bufferDesc;
            HR( buffer->GetDesc( &bufferDesc ) );

            for (U32 j = 0; j < bufferDesc.Variables; j++)
            {
                auto var = buffer->GetVariableByIndex(j);

                D3D11_SHADER_VARIABLE_DESC varDesc;
                HR( var->GetDesc( &varDesc ) );

                auto type = var->GetType();

                D3D11_SHADER_TYPE_DESC typeDesc;
                HR( type->GetDesc( &typeDesc ) );
            }
        }

    }

} } // End namespaces