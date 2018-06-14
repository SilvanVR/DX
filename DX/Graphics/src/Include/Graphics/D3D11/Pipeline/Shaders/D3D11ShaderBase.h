#pragma once
/**********************************************************************
    class: ShaderBase (D3D11ShaderBase.h)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "D3D11/D3D11.hpp"
#include "D3D11/D3D11ConstantBufferManager.h"
#include "OS/FileSystem/path.h"
#include <d3dcompiler.h>
#include "shader_resources.hpp"

namespace Graphics { namespace D3D11 {

    // Get the latest profile for the specified Shader type.
    template<class ShaderCompilerClass>
    String GetLatestProfile();

    //**********************************************************************
    class ShaderBase
    {
    public:
        ShaderBase(ShaderType shaderType) : m_shaderType(shaderType) {};
        virtual ~ShaderBase() { SAFE_RELEASE( m_pShaderBlob ); SAFE_RELEASE( m_pShaderReflection ); }

        //----------------------------------------------------------------------
        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual bool compileFromFile(const OS::Path& path, CString entryPoint) = 0;
        virtual bool compileFromSource(const String& shaderSource, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        const OS::Path&                                     getFilePath()   const { return m_filePath; }
        CString                                             getEntryPoint() const { return m_entryPoint.c_str(); }
        bool                                                recompile();
        const ArrayList<ShaderResourceDeclaration>&         getResourceDeclarations() const { return m_resourceDeclarations; }
        const ArrayList<ShaderUniformBufferDeclaration>&    getConstantBufferBindings() const { return m_constantBuffers; }
        const ShaderResourceDeclaration*                    getResourceDeclaration(StringID name) const;

        //----------------------------------------------------------------------
        // @Return:
        //  Whether the shader (if compiled from file) is up to date on disk.
        //  This returns always true if the shader was compiled from source.
        //----------------------------------------------------------------------
        bool isUpToDate() const;

        //----------------------------------------------------------------------
        // @Return:
        //  Constant buffer with name "name". Nullptr if not existent.
        //----------------------------------------------------------------------
        const ShaderUniformBufferDeclaration* getUniformBufferDeclaration(StringID name) const;

        //----------------------------------------------------------------------
        // @Return:
        //  Constant buffer info about first constant buffer with name "material" in it.
        //  Nullptr if not existent.
        //----------------------------------------------------------------------
        const ShaderUniformBufferDeclaration* getMaterialBufferDeclaration() const;

        //----------------------------------------------------------------------
        // @Return:
        //  Constant buffer info about first constant buffer with name "shader" in it.
        //  Nullptr if not existent.
        //----------------------------------------------------------------------
        const ShaderUniformBufferDeclaration* getShaderBufferDeclaration() const;

    protected:
        ID3DBlob*                               m_pShaderBlob           = nullptr;
        ID3D11ShaderReflection*                 m_pShaderReflection     = nullptr;
        String                                  m_entryPoint            = "main";
        ShaderType                              m_shaderType            = ShaderType::Unknown;
        OS::Path                                m_filePath;
        OS::SystemTime                          m_fileTimeAtCompilation;

        // Resources + UBO's bound to this shader
        ArrayList<ShaderUniformBufferDeclaration>   m_constantBuffers;
        ArrayList<ShaderResourceDeclaration>        m_resourceDeclarations;

        //----------------------------------------------------------------------
        template <typename T>
        bool _CompileFromFile( const OS::Path& path, CString entryPoint )
        {
            m_filePath = path;
            m_entryPoint = entryPoint;
            m_fileTimeAtCompilation = m_filePath.getLastWrittenFileTime();

            return _CompileFromFile( m_filePath, GetLatestProfile<T>().c_str() );
        }

        //----------------------------------------------------------------------
        template <typename T>
        bool _CompileFromSource( const String& source, CString entryPoint )
        {
            m_entryPoint = entryPoint;
            return _CompileFromSource( source, GetLatestProfile<T>().c_str() );
        }

    private:
        //----------------------------------------------------------------------
        bool _CompileFromFile(const OS::Path& path, CString profile);
        bool _CompileFromSource(const String& source, CString profile);

        void _ShaderReflection(ID3DBlob* pShaderBlob);
        void _ReflectResources(const D3D11_SHADER_DESC& shaderDesc);
        void _ReflectConstantBuffer(ID3D11ShaderReflectionConstantBuffer* cb, U32 bindSlot);
        DataType _GetDataType(ID3D11ShaderReflectionVariable* var);

        //----------------------------------------------------------------------
        ShaderBase(const ShaderBase& other)               = delete;
        ShaderBase& operator = (const ShaderBase& other)  = delete;
        ShaderBase(ShaderBase&& other)                    = delete;
        ShaderBase& operator = (ShaderBase&& other)       = delete;
    };


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

    //----------------------------------------------------------------------
    template<> inline
    String GetLatestProfile<ID3D11GeometryShader>()
    {
        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = g_pDevice->GetFeatureLevel();
        switch (featureLevel)
        {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "gs_5_0";
        case D3D_FEATURE_LEVEL_10_1:
            return "gs_4_1";
        case D3D_FEATURE_LEVEL_10_0:
            return "gs_4_0";
        case D3D_FEATURE_LEVEL_9_3:
            return "gs_4_0_level_9_3";
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            return "gs_4_0_level_9_1";
        }
        return "";
    }

} } // End namespaces