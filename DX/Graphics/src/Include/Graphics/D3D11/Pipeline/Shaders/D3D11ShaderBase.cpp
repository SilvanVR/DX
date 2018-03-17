#include "D3D11ShaderBase.h"
/**********************************************************************
    class: ShaderBase (D3D11ShaderBase.cpp)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include <d3dcompiler.h>

namespace Graphics { namespace D3D11 {

        //----------------------------------------------------------------------
        bool ShaderBase::_Compile( CString profile )
        {
            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
        #ifdef _DEBUG
            flags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
        #endif

            ID3DBlob* errorBlob = nullptr;
            HRESULT hr = D3DCompileFromFile( ConvertToWString( m_filePath.toString() ).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                             m_entryPoint.c_str(), profile, flags, 0, &m_shaderBlob, &errorBlob );

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

                SAFE_RELEASE( m_shaderBlob );

                return false;
            }

            return true;
        }

} } // End namespaces