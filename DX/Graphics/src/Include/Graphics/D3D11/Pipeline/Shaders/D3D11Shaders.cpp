#include "D3D11Shaders.h"
/**********************************************************************
    class: ShaderBase + VertexShader etc. (D3D11Shaders.cpp)

    author: S. Hau
    date: December 3, 2017
**********************************************************************/

#include <d3dcompiler.h>

namespace Graphics { namespace D3D11 {


    //----------------------------------------------------------------------
    ShaderBase::ShaderBase( const OS::Path& path )
        : m_filePath( path )
    {
    }

    //----------------------------------------------------------------------
    ShaderBase::~ShaderBase()
    {
        SAFE_RELEASE( m_shaderBaseBlob );
    }

    //----------------------------------------------------------------------
    bool ShaderBase::_Compile( CString entryPoint, CString profile )
    {
        m_entryPoint = entryPoint;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
    #ifdef _DEBUG
        flags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
    #endif

        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompileFromFile( ConvertToWString( m_filePath.toString() ).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                         entryPoint, profile, flags, 0, &m_shaderBaseBlob, &errorBlob );

        m_fileTimeAtCompilation = m_filePath.getLastWrittenFileTime();

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

            SAFE_RELEASE( m_shaderBaseBlob );

            return false;
        }

        return true;
    }

    //----------------------------------------------------------------------
    bool ShaderBase::isUpToDate()
    {
        return m_fileTimeAtCompilation == m_filePath.getLastWrittenFileTime();
    }

    //**********************************************************************
    // VERTEX ShaderBase
    //**********************************************************************

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
        bool compiled = _Compile( entryPoint, GetLatestProfile<ID3D11VertexShader>().c_str() );
        if (not compiled)
            return false;

        // Clean-Up old vertex-shader
        SAFE_RELEASE( m_pVertexShader );

        // Create Vertex-Shader and input layout
        HR( g_pDevice->CreateVertexShader( m_shaderBaseBlob->GetBufferPointer(), m_shaderBaseBlob->GetBufferSize(), nullptr, &m_pVertexShader ) );
        _CreateInputLayout( m_shaderBaseBlob );

        // Shader blob no longer needed
        SAFE_RELEASE( m_shaderBaseBlob );

        return true;
    }

    //----------------------------------------------------------------------
    void VertexShader::_CreateInputLayout( ID3DBlob* pShaderBlob )
    {
        // Clean up old data
        SAFE_RELEASE( m_pInputLayout );
        m_vertexLayout.clear();

        // Reflect shader info
        ID3D11ShaderReflection* pVertexShaderReflection = NULL;
        if ( FAILED( D3DReflect( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), 
                                 IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection) ) )
        {
            ERROR_RENDERING( "VertexShader::_CreateInputLayout(): Shader reflection failed. Investigate this!" );
            return;
        }

        // Get shader info
        D3D11_SHADER_DESC shaderDesc;
        pVertexShaderReflection->GetDesc( &shaderDesc );

        // Read input layout description from shader info
        ArrayList<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
        for (U32 i = 0; i< shaderDesc.InputParameters; i++)
        {
            D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
            pVertexShaderReflection->GetInputParameterDesc( i, &paramDesc );

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
                                          pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pInputLayout) );

        // Free allocation shader reflection memory
        SAFE_RELEASE( pVertexShaderReflection );
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
            WARN_RENDERING( "Semantic name '" + semanticName + "' for shader '" + getFilePath().toString() + "' does not exist.");
    }

    //**********************************************************************
    // PIXEL ShaderBase
    //**********************************************************************

    //----------------------------------------------------------------------
    PixelShader::PixelShader( CString path )
        : ShaderBase( path )
    {
    }
    
    //----------------------------------------------------------------------
    PixelShader::~PixelShader()
    {
        SAFE_RELEASE( m_pPixelShader );
    }

    //----------------------------------------------------------------------
    void PixelShader::bind()
    {
        ASSERT( m_pPixelShader != nullptr );
        g_pImmediateContext->PSSetShader( m_pPixelShader, NULL, 0 );
    }

    //----------------------------------------------------------------------
    bool PixelShader::compile( CString entryPoint )
    {
        bool compiled = _Compile( entryPoint, GetLatestProfile<ID3D11PixelShader>().c_str() );
        if (not compiled)
            return false;

        SAFE_RELEASE( m_pPixelShader );

        HR( g_pDevice->CreatePixelShader( m_shaderBaseBlob->GetBufferPointer(), m_shaderBaseBlob->GetBufferSize(), nullptr, &m_pPixelShader ) );

        // Shader blob no longer needed
        SAFE_RELEASE( m_shaderBaseBlob );

        return true;
    }

} } // End namespaces