#pragma once
/**********************************************************************
    class: ShaderBase (D3D11ShaderBase.h)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "Vulkan/Vulkan.hpp"
#include "OS/FileSystem/path.h"
#include "shader_resources.hpp"
#include <functional>

namespace Graphics { namespace Vulkan {

    struct ShaderBlob
    {
        const void* data;
        Size size;
    };

    //**********************************************************************
    class ShaderBase
    {
    public:
        ShaderBase(ShaderType shaderType) : m_shaderType(shaderType) {}
        virtual ~ShaderBase() {}

        //----------------------------------------------------------------------
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
        ShaderType  m_shaderType = ShaderType::Unknown;
        OS::Path    m_filePath;

        // Resources + UBO's bound to this shader
        ArrayList<ShaderUniformBufferDeclaration>   m_constantBuffers;
        ArrayList<ShaderResourceDeclaration>        m_resourceDeclarations;

        void _CompileFromSource(const String& source, CString entryPoint, std::function<void(const ShaderBlob&)>);
        void _CompileFromFile(const OS::Path& path, CString entryPoint, std::function<void(const ShaderBlob&)>);

    private:
        //----------------------------------------------------------------------
        void _ShaderReflection(const ShaderBlob& shaderBlob);
        //void _ReflectResources(const D3D11_SHADER_DESC& shaderDesc);
        //void _ReflectConstantBuffer(ID3D11ShaderReflectionConstantBuffer* cb, U32 bindSlot);
        //DataType _GetDataType(ID3D11ShaderReflectionVariable* var);

        //----------------------------------------------------------------------
        NULL_COPY_AND_ASSIGN(ShaderBase)
    };


} } // End namespaces