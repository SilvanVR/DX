#pragma once
/**********************************************************************
    class: IMaterial (i_material.h)

    author: S. Hau
    date: March 12, 2018

    Interface for a Material class. Each material is rendered by a
    shader (which can consists of vertex/frag/geo/tesselation) and
    contains the necessary data for this shader.
**********************************************************************/

#include "i_shader.h"
#include "Utils/i_cached_shader_maps.h"

namespace Graphics {

    //**********************************************************************
    class IMaterial : public ICachedShaderMaps
    {
    public:
        IMaterial() = default;
        virtual ~IMaterial() {}

        //----------------------------------------------------------------------
        const ShaderPtr&    getShader()         const { return m_shader; }
        const String&       getName()           const { return m_name; }
        const ShaderPtr     getReplacementShader(StringID tag) const { return hasReplacementShader(tag) ? m_replacementShaders.at(tag) : nullptr; }
        bool                hasReplacementShader(StringID tag) const { return m_replacementShaders.find(tag) != m_replacementShaders.end(); }

        void                setShader               (const ShaderPtr& shader);
        void                setName                 (const String& name)                    { m_name = name; }
        void                setReplacementShader    (StringID tag, const ShaderPtr& shader) { m_replacementShaders[tag] = shader; }

        DataType            getDataType(StringID name)  const;
        DataType            getDataType(CString name)  const { return getDataType(SID(name)); }

    protected:
        ShaderPtr   m_shader = nullptr; // Shader used to render this material
        String      m_name = "NO NAME";

        // List of replacement shaders
        HashMap<StringID, ShaderPtr> m_replacementShaders;

        //----------------------------------------------------------------------
        void _BindTextures();

        //**********************************************************************
        // ICachedShaderMaps Interface
        //**********************************************************************
        void _WarnMissingInt(StringID name)     const override;
        void _WarnMissingFloat(StringID name)   const override;
        void _WarnMissingColor(StringID name)   const override;
        void _WarnMissingVec4(StringID name)    const override;
        void _WarnMissingMatrix(StringID name)  const override;
        void _WarnMissingTexture(StringID name) const override;

        bool _HasShaderInt(StringID name)     const override;
        bool _HasShaderFloat(StringID name)   const override;
        bool _HasShaderColor(StringID name)   const override;
        bool _HasShaderVec4(StringID name)    const override;
        bool _HasShaderMatrix(StringID name)  const override;
        bool _HasShaderTexture(StringID name) const override;

    private:
        //----------------------------------------------------------------------
        friend class D3D11Renderer;
        friend class VkRenderer;
        virtual void bind() = 0;

        //----------------------------------------------------------------------
        // This function should recreate the necessary buffers
        //----------------------------------------------------------------------
        virtual void _ChangedShader() = 0;

        NULL_COPY_AND_ASSIGN(IMaterial)
    };

    using Material = IMaterial;

} // End namespaces

using MaterialPtr = std::shared_ptr<Graphics::IMaterial>;