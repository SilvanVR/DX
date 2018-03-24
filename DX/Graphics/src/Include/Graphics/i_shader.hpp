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
    class IShader
    {
    public:
        IShader() = default;
        virtual ~IShader() {}

        //----------------------------------------------------------------------
        const String& getName() const { return m_name; }
        void setName(const String& name) { m_name = name; }

        //----------------------------------------------------------------------
        // Try to compile this shader.
        // @Params:
        //  "vertPath": Path to the vertex shader file.
        //  "fragPath": Path to the fragment shader file.
        //  "entryPoint": Entry point of the shader.
        // @Return:
        //  True, if compilation was successful, otherwise false and prints what went wrong.
        //----------------------------------------------------------------------
        virtual bool compile(const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        // Try to compile this shader.
        // @Params:
        //  "vertSrc": Source of the vertex shader in ASCII.
        //  "fragSrc": Source of the fragment shader in ASCII.
        //  "entryPoint": Entry point of the shader.
        // @Return:
        //  True, if compilation was successful, otherwise false and prints what went wrong.
        //----------------------------------------------------------------------
        virtual bool compile( const String& vertSrc, const String& fragSrc, CString entryPoint ) = 0;

        //----------------------------------------------------------------------
        // Recompile all shaders which are not up to date.
        // @Return:
        //  List of shader paths, which were recompiled.
        virtual ArrayList<OS::Path> recompile() = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  True, if all shader files are up-to-date (weren't modified).
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
        // Return the vertex layout from the attached vertex-shader.
        //----------------------------------------------------------------------
        virtual const VertexLayout& getVertexLayout() const = 0;

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
        void enableAlphaBlending( bool alphaToCoverage = false )
        {
            Graphics::BlendState blendState;
            blendState.alphaToCoverage = alphaToCoverage;
            blendState.blendStates[0].blendEnable    = true;
            blendState.blendStates[0].srcBlend       = Graphics::Blend::SRC_ALPHA;
            blendState.blendStates[0].destBlend      = Graphics::Blend::INV_SRC_ALPHA;
            blendState.blendStates[0].blendOp        = Graphics::BlendOP::ADD;
            blendState.blendStates[0].srcBlendAlpha  = Graphics::Blend::ONE;
            blendState.blendStates[0].destBlendAlpha = Graphics::Blend::ZERO;
            blendState.blendStates[0].blendOpAlpha   = Graphics::BlendOP::ADD;

            setBlendState( blendState );
        }

    protected:
        // These are only used when configured correctly
        std::array<F32, 4> m_blendFactors = { 1.0f, 1.0f, 1.0f, 1.0f };
        String             m_name = "NO NAME";

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

//**********************************************************************