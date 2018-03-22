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

    // Contains infos about one specific member in a shader buffer
    struct ConstantBufferMemberInfo
    {
        StringID    name;
        U32         offset = 0;
        Size        size = 0;
    };

    // Contains information about an buffer in a shader
    struct ConstantBufferInfo
    {
        StringID                            name;
        U32                                 slot = 0;
        Size                                sizeInBytes = 0;
        ArrayList<ConstantBufferMemberInfo> members;
    };

    //**********************************************************************
    class ShaderBase
    {
    public:
        ShaderBase(const OS::Path& path) : m_filePath( path ) {}
        virtual ~ShaderBase() { SAFE_RELEASE( m_pShaderBlob ); SAFE_RELEASE( m_pShaderReflection ); }

        //----------------------------------------------------------------------
        virtual void bind() = 0;
        virtual bool compile(CString entryPoint) = 0;

        //----------------------------------------------------------------------
        const OS::Path& getFilePath()   const { return m_filePath; }
        CString         getEntryPoint() const { return m_entryPoint.c_str(); }
        bool            isUpToDate()    const { return m_fileTimeAtCompilation == m_filePath.getLastWrittenFileTime(); }
        ID3DBlob*       getShaderBlob() { return m_pShaderBlob; }

        //----------------------------------------------------------------------
        // @Return:
        //  List of information about every constant buffer found via reflection.
        //----------------------------------------------------------------------
        const ArrayList<ConstantBufferInfo>&    getConstantBufferInformation()  const { return m_constantBufferInformation; }

        //----------------------------------------------------------------------
        // @Return:
        //  Constant buffer info about first constant buffer with name "material" in it.
        // Note: Use "hasMaterialBuffer()" before calling this function to ensure 
        // that this shader has an material constant buffer!
        //----------------------------------------------------------------------
        const ConstantBufferInfo&               getMaterialBufferInformation()  const;

        //----------------------------------------------------------------------
        // @Return:
        //  True, when a material constant buffer exists in this shader.
        //  (A buffer with name 'material' in it)
        //----------------------------------------------------------------------
        bool hasMaterialBuffer() const;

    protected:
        ID3DBlob* m_pShaderBlob = nullptr;
        ID3D11ShaderReflection*         m_pShaderReflection     = nullptr;
        String                          m_entryPoint            = "main";
        OS::Path                        m_filePath;
        OS::SystemTime                  m_fileTimeAtCompilation;
        ArrayList<ConstantBufferInfo>   m_constantBufferInformation;

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
        void _ShaderReflection(ID3DBlob* pShaderBlob);
        void _ReflectConstantBuffers();

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