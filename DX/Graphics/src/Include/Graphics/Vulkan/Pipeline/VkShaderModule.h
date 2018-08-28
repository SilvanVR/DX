#pragma once
/**********************************************************************
    class: ShaderModule

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#include "Vulkan/Vulkan.hpp"
#include "OS/FileSystem/path.h"
#include "enums.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class ShaderModule
    {
    public:
        ShaderModule(ShaderType shaderType) : m_shaderType(shaderType) {}
        ~ShaderModule();

        //----------------------------------------------------------------------
        void compileFromFile(const OS::Path& path, CString entryPoint);
        void compileFromSource(const String& shaderSource, CString entryPoint);

        //----------------------------------------------------------------------
        const OS::Path&         getFilePath()       const { return m_filePath; }
        const VkShaderModule&   getVkShaderModule() const { return m_shaderModule; }
        CString                 getEntryPoint()     const { return m_entryPoint.c_str(); }

    private:
        VkShaderModule  m_shaderModule;
        ShaderType      m_shaderType = ShaderType::Unknown;
        OS::Path        m_filePath;
        String          m_entryPoint;

        void _CompileGLSL(const String& source, CString entryPoint);

        NULL_COPY_AND_ASSIGN(ShaderModule)
    };


} } // End namespaces