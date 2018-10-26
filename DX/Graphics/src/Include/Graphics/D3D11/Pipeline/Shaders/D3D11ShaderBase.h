#pragma once
/**********************************************************************
    class: ShaderBase (D3D11ShaderBase.h)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "D3D11/D3D11.hpp"
#include "OS/FileSystem/path.h"
#include "shader_resources.hpp"
#include <d3dcompiler.h>
#include <functional>

namespace Graphics { namespace D3D11 {

    // Get the latest profile for the specified Shader type.
    template<class ShaderCompilerClass>
    String GetLatestProfile();

    struct ShaderBlob
    {
        const void* data;
        Size size;
    };

    UINT GetCompileFlags();

    //**********************************************************************
    class ShaderBase
    {
    public:
        ShaderBase(ShaderType shaderType) : m_shaderType(shaderType) {}
        virtual ~ShaderBase() {}

        //----------------------------------------------------------------------
        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual void compileFromFile(const OS::Path& path, CString entryPoint) = 0;
        virtual void compileFromSource(const String& shaderSource, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        const OS::Path&                                     getFilePath()   const { return m_filePath; }
        const ArrayList<ShaderResourceDeclaration>&         getResourceDeclarations() const { return m_resourceDeclarations; }
        const ArrayList<ShaderUniformBufferDeclaration>&    getConstantBufferBindings() const { return m_constantBuffers; }
        const ShaderResourceDeclaration*                    getResourceDeclaration(StringID name) const;

        //----------------------------------------------------------------------
        // @Return:
        //  Constant buffer with name "name". Nullptr if not existent.
        //----------------------------------------------------------------------
        const ShaderUniformBufferDeclaration* getUniformBufferDeclaration(StringID name) const;

    protected:
        ComPtr<ID3D11ShaderReflection>          m_pShaderReflection     = nullptr;
        ShaderType                              m_shaderType            = ShaderType::Unknown;
        OS::Path                                m_filePath;

        // Resources + UBO's bound to this shader
        ArrayList<ShaderUniformBufferDeclaration>   m_constantBuffers;
        ArrayList<ShaderResourceDeclaration>        m_resourceDeclarations;

        void _CompileFromSource(const String& source, CString entryPoint, std::function<void(const ShaderBlob&)>);
        void _CompileFromFile(const OS::Path& path, CString entryPoint, std::function<void(const ShaderBlob&)>);

    private:
        //----------------------------------------------------------------------
        void _ShaderReflection(const ShaderBlob& shaderBlob);
        void _ReflectResources(const D3D11_SHADER_DESC& shaderDesc);
        void _ReflectConstantBuffer(ID3D11ShaderReflectionConstantBuffer* cb, U32 bindSlot);
        U32 _GetArraySize(ID3D11ShaderReflectionVariable* var);
        DataType _GetDataType(ID3D11ShaderReflectionVariable* var);
        String _GetLatestProfile();

        //----------------------------------------------------------------------
        NULL_COPY_AND_ASSIGN(ShaderBase)
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