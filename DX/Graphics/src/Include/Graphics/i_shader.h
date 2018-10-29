#pragma once
/**********************************************************************
    class: IShader (i_shader.h)

    author: S. Hau
    date: March 12, 2018

    Interface for a Shader class. A shader can be applied to a material.
    A shader represents a full pipeline from vertex-shader up to the
    fragment-shader, with all the state settings required to render it.
**********************************************************************/

#include "OS/FileSystem/path.h"
#include "structs.hpp"
#include "vertex_layout.hpp"
#include "i_texture.h"
#include "shader_resources.hpp"
#include <functional>
#include "Utils/i_cached_shader_maps.h"

namespace Graphics {

    class IShader;
    typedef std::function<void(IShader*)> ShaderReloadCallback;

    //**********************************************************************
    // Shaders with a renderqueue >= BackToFrontBoundary will be rendered back to front.
    enum class RenderQueue : I32
    {
        // --------- FRONT TO BACK ------------
        Background          = 0,
        Geometry            = 1000,
        AlphaTest           = 2000,
        // --------- BACK TO FRONT -------------
        BackToFrontBoundary = 3000,
        Transparent         = 4000,
        Overlay             = 5000
    };

    //**********************************************************************
    class IShader : public ICachedShaderMaps
    {
    public:
        IShader() = default;
        virtual ~IShader() {}

        //----------------------------------------------------------------------
        const String&                   getName()           const { return m_name; }
        I32                             getRenderQueue()    const { return m_renderQueue; }

        void setName(const String& name) { m_name = name; }

        //----------------------------------------------------------------------
        // @Return:
        //  True, if the corresponding shader exists.
        //----------------------------------------------------------------------
        virtual bool hasFragmentShader() const = 0;
        virtual bool hasGeometryShader() const = 0;
        virtual bool hasTessellationShader() const = 0;

        //----------------------------------------------------------------------
        // Try to compile this shader.
        // @Params:
        //  "vertPath": Path to the vertex shader file.
        //  "fragPath": Path to the fragment shader file.
        //  "entryPoint": Entry point of the shader.
        // @Throws:
        //  std::runtime_error if compilation failed.
        //----------------------------------------------------------------------
        virtual void compileFromFile(const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        // Try to compile this shader.
        // @Params:
        //  "vertSrc": Source of the vertex shader in ASCII.
        //  "fragSrc": Source of the fragment shader in ASCII.
        //  "entryPoint": Entry point of the shader.
        // @Throws:
        //  std::runtime_error if compilation failed.
        //----------------------------------------------------------------------
        virtual void compileFromSource(const String& vertSrc, const String& fragSrc, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        // Try to compile one shader.
        // @Params:
        //  "src": Source of the shader in ASCII.
        //  "entryPoint": Entry point of the shader.
        // @Throws:
        //  std::runtime_error if compilation failed.
        //----------------------------------------------------------------------
        virtual void compileVertexShaderFromSource(const String& src, CString entryPoint) = 0;
        virtual void compileFragmentShaderFromSource(const String& src, CString entryPoint) = 0;
        virtual void compileGeometryShaderFromSource(const String& src, CString entryPoint) = 0;
        //virtual bool compileTessellationShaderFromSource(const String& src, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        // Change pipeline states for rendering this shader
        //----------------------------------------------------------------------
        virtual void setRasterizationState(const RasterizationState& rzState) = 0;
        virtual void setDepthStencilState(const DepthStencilState& dsState) = 0;
        virtual void setBlendState(const BlendState& bState) = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  The vertex layout from the attached vertex-shader.
        //----------------------------------------------------------------------
        virtual const VertexLayout& getVertexLayout() const = 0;

        //----------------------------------------------------------------------
        // Usually creates the pipeline for this shader and reflects all resources.
        // Should be called after every shader-stage has been compiled.
        //----------------------------------------------------------------------
        virtual void createPipeline() = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  All Resources used by this shaders.
        //----------------------------------------------------------------------
        const ArrayList<ShaderUniformBufferDeclaration>& getUniformBufferDeclarations() const { return m_uniformBuffers; }
        const ArrayList<ShaderResourceDeclaration>&      getResourceDeclarations()      const { return m_shaderResources; }

        //----------------------------------------------------------------------
        // @Return:
        //  Information about the corresponding uniform buffer which is used by a material.
        //----------------------------------------------------------------------
        const ShaderUniformBufferDeclaration* getVSUniformMaterialBuffer() const;
        const ShaderUniformBufferDeclaration* getFSUniformMaterialBuffer() const;
        const ShaderUniformBufferDeclaration* getGSUniformMaterialBuffer() const;

        //----------------------------------------------------------------------
        // @Return:
        //  Information about the corresponding uniform buffer which is used by a shader.
        //----------------------------------------------------------------------
        const ShaderUniformBufferDeclaration* getVSUniformShaderBuffer() const;
        const ShaderUniformBufferDeclaration* getFSUniformShaderBuffer() const;
        const ShaderUniformBufferDeclaration* getGSUniformShaderBuffer() const;

        //----------------------------------------------------------------------
        // @Return:
        //  The ubo declaration with name 'name' across all shader stages. Nullptr if not existent.
        //----------------------------------------------------------------------
        const ShaderUniformBufferDeclaration* getUniformBufferDecl(StringID name) const;

        //----------------------------------------------------------------------
        // @Return:
        //  The resource declaration with name 'name' across all shader stages. Nullptr if not existent.
        //----------------------------------------------------------------------
        const ShaderResourceDeclaration* getShaderResource(StringID name) const;

        //----------------------------------------------------------------------
        // @Return:
        //  The datatype of a property with the given name. Issues a warning if the
        //  name exists in more than one shader stage. (This does not include shader resources like textures)
        //----------------------------------------------------------------------
        DataType getDataTypeOfMaterialProperty(StringID name) const;
        DataType getDataTypeOfMaterialPropertyOrResource(StringID name) const;

        //----------------------------------------------------------------------
        // @Return:
        //  True if the property exists and is considered to be part of the material. (This does not include shader resources like textures)
        //----------------------------------------------------------------------
        bool hasMaterialProperty(StringID name) const { return getDataTypeOfMaterialProperty(name) != DataType::Unknown; }

        //----------------------------------------------------------------------
        // @Return:
        //  The datatype of a property with the given name. (This does not include shader resources like textures)
        //----------------------------------------------------------------------
        DataType getDataTypeOfShaderMember(StringID name) const;

        //----------------------------------------------------------------------
        // @Return:
        //  True if the property exists and is considered to be part of the shader. (This does not include shader resources like textures)
        //----------------------------------------------------------------------
        bool hasShaderProperty(StringID name) const { return getDataTypeOfShaderMember(name) != DataType::Unknown; }

        //----------------------------------------------------------------------
        // @Return:
        //  Manual Blend-Factors. Only used when manual blending is enabled.
        //----------------------------------------------------------------------
        std::array<F32, 4> getBlendFactors() const { return m_blendFactors; }

        //----------------------------------------------------------------------
        // Set manually the blend factors. Only used when manual blending is enabled.
        //----------------------------------------------------------------------
        void setBlendFactors(std::array<F32, 4> newBlendFactors) { m_blendFactors = newBlendFactors; }

        //----------------------------------------------------------------------
        // Convenvience function for enabling the standard alpha blending
        //----------------------------------------------------------------------
        void enableAlphaBlending(bool alphaToCoverage = false)
        {
            Graphics::BlendState blendState;
            blendState.alphaToCoverage = alphaToCoverage;
            blendState.blendStates[0].blendEnable    = true;
            blendState.blendStates[0].srcBlend       = Graphics::Blend::SrcAlpha;
            blendState.blendStates[0].destBlend      = Graphics::Blend::InvSrcAlpha;
            blendState.blendStates[0].blendOp        = Graphics::BlendOP::Add;
            blendState.blendStates[0].srcBlendAlpha  = Graphics::Blend::One;
            blendState.blendStates[0].destBlendAlpha = Graphics::Blend::Zero;
            blendState.blendStates[0].blendOpAlpha   = Graphics::BlendOP::Add;

            setBlendState( blendState );
        }

        //----------------------------------------------------------------------
        // Set the renderqueue for this shader.
        //----------------------------------------------------------------------
        void setRenderQueue(I32 renderQueue) { m_renderQueue = renderQueue; }

        //----------------------------------------------------------------------
        // The given function will be invoked whenever the shader gets reloaded.
        //----------------------------------------------------------------------
        void setReloadCallback(const ShaderReloadCallback& f) { m_reloadCallback = f; invokeReloadCallback(); }

        //----------------------------------------------------------------------
        // Invokes the reload callback if one exists.
        //----------------------------------------------------------------------
        void invokeReloadCallback() { if (m_reloadCallback) m_reloadCallback(this); }

    protected:
        std::array<F32, 4>  m_blendFactors      = { 1.0f, 1.0f, 1.0f, 1.0f };   // These are only used when blending is enabled
        String              m_name              = "NO NAME";
        I32                 m_renderQueue       = (I32)RenderQueue::Geometry;

        // Can be set & invoked for shader recompilation
        ShaderReloadCallback m_reloadCallback = nullptr;

        // All shader resources from all shaders
        ArrayList<ShaderUniformBufferDeclaration> m_uniformBuffers;
        ArrayList<ShaderResourceDeclaration>      m_shaderResources;

        // Bind all textures in the texture map
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
        virtual void unbind() = 0;

        const ShaderUniformBufferDeclaration* _GetUniformBuffer(const String& name, Graphics::ShaderType shaderType) const;

        NULL_COPY_AND_ASSIGN(IShader)
    };

    using Shader = IShader;

} // End namespaces

using ShaderPtr = std::shared_ptr<Graphics::IShader>;