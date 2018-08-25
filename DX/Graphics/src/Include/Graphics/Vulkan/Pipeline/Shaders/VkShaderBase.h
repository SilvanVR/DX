#pragma once
/**********************************************************************
    class: ShaderBase

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#include "Vulkan/Vulkan.hpp"
#include "OS/FileSystem/path.h"
#include "shader_resources.hpp"

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
        const PushConstant&                                 getPushConstant()           const { return m_pushConstant; }
        CString                                             getEntryPoint()             const { return m_entryPoint.c_str(); }

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
        const ShaderUniformBufferDeclaration* getMaterialBufferDeclaration() const { return m_materialUBO; }

        //----------------------------------------------------------------------
        // @Return:
        //  Constant buffer info about first constant buffer with name "shader" in it.
        //  Nullptr if not existent.
        //----------------------------------------------------------------------
        const ShaderUniformBufferDeclaration* getShaderBufferDeclaration() const { return m_shaderUBO; }

    protected:

        // Can be overriden by a subclass, eg. by the vertex shader to reflect the input layout
        virtual void _ShaderReflection(const ArrayList<uint32_t>& spv);

    private:
        VkShaderModule  m_shaderModule;
        ShaderType      m_shaderType = ShaderType::Unknown;
        OS::Path        m_filePath;
        String          m_entryPoint;

        // Resources + UBO's bound to this shader
        ArrayList<ShaderUniformBufferDeclaration>   m_uniformBuffers;
        ArrayList<ShaderResourceDeclaration>        m_resourceDeclarations;
        PushConstant                                m_pushConstant;

        ShaderUniformBufferDeclaration*             m_shaderUBO   = nullptr;
        ShaderUniformBufferDeclaration*             m_materialUBO = nullptr;

        NULL_COPY_AND_ASSIGN(ShaderBase)
    };


} } // End namespaces