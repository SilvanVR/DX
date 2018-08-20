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

    //----------------------------------------------------------------------
    struct PushConstant
    {
        U32 sizeInBytes;
        ArrayList<ShaderUniformDeclaration> members;
    };

    //**********************************************************************
    class ShaderBase
    {
    public:
        ShaderBase(ShaderType shaderType) : m_shaderType(shaderType) {}
        virtual ~ShaderBase();

        //----------------------------------------------------------------------
        void compileFromFile(const OS::Path& path, CString entryPoint);
        void compileFromSource(const String& shaderSource, CString entryPoint);

        //----------------------------------------------------------------------
        const OS::Path&                                     getFilePath()               const { return m_filePath; }
        const ArrayList<ShaderResourceDeclaration>&         getResourceDeclarations()   const { return m_resourceDeclarations; }
        const ArrayList<ShaderUniformBufferDeclaration>&    getUniformBufferBindings()  const { return m_uniformBuffers; }
        const ShaderResourceDeclaration*                    getResourceDeclaration(StringID name) const;
        const VkShaderModule&                               getVkShaderModule()         const { return m_shaderModule; }

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

        // Can be overriden by a subclass, eg. by the vertex shader to reflect the input layout
        virtual void _ShaderReflection(const ArrayList<uint32_t>& spv);

    private:
        VkShaderModule  m_shaderModule;
        ShaderType      m_shaderType = ShaderType::Unknown;
        OS::Path        m_filePath;

        // Resources + UBO's bound to this shader
        ArrayList<ShaderUniformBufferDeclaration>   m_uniformBuffers;
        ArrayList<ShaderResourceDeclaration>        m_resourceDeclarations;
        PushConstant                                m_pushConstant;


        NULL_COPY_AND_ASSIGN(ShaderBase)
    };


} } // End namespaces