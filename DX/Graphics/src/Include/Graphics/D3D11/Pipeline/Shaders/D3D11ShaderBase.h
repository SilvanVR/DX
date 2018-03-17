#pragma once
/**********************************************************************
    class: ShaderBase (D3D11ShaderBase.h)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "../../D3D11.hpp"
#include "OS/FileSystem/path.h"
#include <d3dcompiler.h>

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class ShaderBase
    {
    public:
        ShaderBase(const OS::Path& path) : m_filePath( path ) {}
        virtual ~ShaderBase() { SAFE_RELEASE( m_shaderBlob ); };

        //----------------------------------------------------------------------
        const OS::Path& getFilePath()   const { return m_filePath; }
        CString         getEntryPoint() const { return m_entryPoint.c_str(); }
        bool            isUpToDate()    const { return m_fileTimeAtCompilation == m_filePath.getLastWrittenFileTime(); }
        ID3DBlob*       getShaderBlob() { return m_shaderBlob; }

        //----------------------------------------------------------------------
        virtual void bind() = 0;
        virtual bool compile(CString entryPoint) = 0;


    protected:
        ID3DBlob*       m_shaderBlob = nullptr;
        String          m_entryPoint = "main";
        OS::Path        m_filePath;
        OS::SystemTime  m_fileTimeAtCompilation;

        //----------------------------------------------------------------------
        template <typename T>
        bool _Compile( CString entryPoint ) 
        {
            m_entryPoint = entryPoint;
            m_fileTimeAtCompilation = m_filePath.getLastWrittenFileTime();

            return _Compile( GetLatestProfile<T>().c_str() );
        }

    private:

        //----------------------------------------------------------------------
        bool _Compile(CString profile);

        //----------------------------------------------------------------------
        ShaderBase(const ShaderBase& other)               = delete;
        ShaderBase& operator = (const ShaderBase& other)  = delete;
        ShaderBase(ShaderBase&& other)                    = delete;
        ShaderBase& operator = (ShaderBase&& other)       = delete;
    };

    
    // Get the latest profile for the specified Shader type.
    template<class ShaderCompilerClass>
    String GetLatestProfile();

    //----------------------------------------------------------------------
    template<> inline
    String GetLatestProfile<ID3D11VertexShader>()
    {
        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = g_pDevice->GetFeatureLevel();
        switch (featureLevel)
        {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "vs_5_0";
        case D3D_FEATURE_LEVEL_10_1:
            return "vs_4_1";
        case D3D_FEATURE_LEVEL_10_0:
            return "vs_4_0";
        case D3D_FEATURE_LEVEL_9_3:
            return "vs_4_0_level_9_3";
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            return "vs_4_0_level_9_1";
        }
        return "";
    }

    //----------------------------------------------------------------------
    template<> inline
    String GetLatestProfile<ID3D11PixelShader>()
    {
        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = g_pDevice->GetFeatureLevel();
        switch (featureLevel)
        {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "ps_5_0";
        case D3D_FEATURE_LEVEL_10_1:
            return "ps_4_1";
        case D3D_FEATURE_LEVEL_10_0:
            return "ps_4_0";
        case D3D_FEATURE_LEVEL_9_3:
            return "ps_4_0_level_9_3";
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            return "ps_4_0_level_9_1";
        }
        return "";
    }

} } // End namespaces