#pragma once
/**********************************************************************
    class: IShader (i_shader.hpp)

    author: S. Hau
    date: March 12, 2018

    Interface for a Shader class. A shader can be applied to a material.
    A shader represents a full pipeline from vertex-shader up to the
    fragment-shader, with all the state settings required to render it.
**********************************************************************/

#include "OS/FileSystem/path.h"
#include "structs.hpp"
#include "vertex_layout.hpp"

namespace Graphics {

    //**********************************************************************
    // Shaders with a renderqueue <= 1000 will be rendered front to back, >= 1000 back to front.
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
    class IShader
    {
    public:
        IShader() = default;
        virtual ~IShader() {}

        //----------------------------------------------------------------------
        const String&   getName() const             { return m_name; }
        I32             getRenderQueue() const      { return m_renderQueue; }
        void            setName(const String& name) { m_name = name; }

        //----------------------------------------------------------------------
        // Try to compile this shader.
        // @Params:
        //  "vertPath": Path to the vertex shader file.
        //  "fragPath": Path to the fragment shader file.
        //  "entryPoint": Entry point of the shader.
        // @Return:
        //  True, if compilation was successful, otherwise false and prints what went wrong.
        //----------------------------------------------------------------------
        virtual bool compileFromFile(const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        // Try to compile this shader.
        // @Params:
        //  "vertSrc": Source of the vertex shader in ASCII.
        //  "fragSrc": Source of the fragment shader in ASCII.
        //  "entryPoint": Entry point of the shader.
        // @Return:
        //  True, if compilation was successful, otherwise false and prints what went wrong.
        //----------------------------------------------------------------------
        virtual bool compileFromSource(const String& vertSrc, const String& fragSrc, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        // Try to compile one shader.
        // @Params:
        //  "src": Source of the shader in ASCII.
        //  "entryPoint": Entry point of the shader.
        // @Return:
        //  True, if compilation was successful, otherwise false and prints what went wrong.
        //----------------------------------------------------------------------
        virtual bool compileVertexShaderFromSource(const String& src, CString entryPoint) = 0;
        virtual bool compileFragmentShaderFromSource(const String& src, CString entryPoint) = 0;
        //virtual bool compileGeometryShaderFromSource(const String& src, CString entryPoint) = 0;
        //virtual bool compileTessellationShaderFromSource(const String& src, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        // Recompile all shaders which are not up to date. Does nothing if shader was compiled from source.
        // @Return:
        //  List of shader paths, which were recompiled.
        //----------------------------------------------------------------------
        virtual ArrayList<OS::Path> recompile() = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  True, if all shader files are up-to-date (weren't modified since they were compiled).
        //----------------------------------------------------------------------
        virtual bool isUpToDate() = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  All shaderpaths used by this shader class.
        //----------------------------------------------------------------------
        virtual ArrayList<OS::Path> getShaderPaths() const = 0;

        //----------------------------------------------------------------------
        // Change pipeline states for rendering this shader
        //----------------------------------------------------------------------
        virtual void setRasterizationState(const RasterizationState& rzState ) = 0;
        virtual void setDepthStencilState(const DepthStencilState& dsState ) = 0;
        virtual void setBlendState(const BlendState& bState ) = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  The vertex layout from the attached vertex-shader.
        //----------------------------------------------------------------------
        virtual const VertexLayout& getVertexLayout() const = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  The datatype of a property in this shader. Unknown if property does not exist.
        //----------------------------------------------------------------------
        virtual DataType getDataTypeOfProperty(StringID name) const = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  The datatype of a material property in this shader. Unknown if property does not exist.
        //----------------------------------------------------------------------
        virtual DataType getDataTypeOfMaterialProperty(StringID name) const = 0;

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
        // @Return:
        //  True if the property exists and is considered to be part of the material.
        //----------------------------------------------------------------------
        bool hasMaterialProperty(StringID name) { return getDataTypeOfMaterialProperty( name ) != DataType::Unknown; }

    protected:
        // These are only used when blending is enabled
        std::array<F32, 4>  m_blendFactors  = { 1.0f, 1.0f, 1.0f, 1.0f };
        String              m_name          = "NO NAME";
        I32                 m_renderQueue   = (I32)RenderQueue::Geometry;

    private:
        //----------------------------------------------------------------------
        friend class D3D11Renderer;
        virtual void bind() = 0;

        //----------------------------------------------------------------------
        IShader(const IShader& other)               = delete;
        IShader& operator = (const IShader& other)  = delete;
        IShader(IShader&& other)                    = delete;
        IShader& operator = (IShader&& other)       = delete;
    };

    using Shader = IShader;

} // End namespaces

using ShaderPtr = std::shared_ptr<Graphics::IShader>;