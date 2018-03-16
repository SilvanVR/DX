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

namespace Graphics {

    class IMesh;

    //**********************************************************************
    class IShader
    {
        friend class D3D11Renderer; // Access to bind() & drawMesh()

    public:
        IShader() = default;
        virtual ~IShader() {}

        //----------------------------------------------------------------------
        // @Params:
        //  "vertPath": Path to the vertex shader file.
        //  "fragPath": Path to the fragment shader file.
        // This sets only the paths. To compile and use this shader you must call compile().
        //----------------------------------------------------------------------
        virtual void setShaderPaths( const OS::Path& vertPath, const OS::Path& fragPath ) = 0;

        //----------------------------------------------------------------------
        // Try to compile this shader.
        // @Params:
        // "entryPoint": Entry point of the shader.
        // @Return:
        //  True, if compilation was successful, otherwise false and prints what went wrong.
        //----------------------------------------------------------------------
        virtual bool compile( CString entryPoint ) = 0;

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

    private:
        //----------------------------------------------------------------------
        virtual void bind() = 0;
        virtual void drawMesh(IMesh* mesh, U32 subMeshIndex) = 0;

        //----------------------------------------------------------------------
        IShader(const IShader& other)               = delete;
        IShader& operator = (const IShader& other)  = delete;
        IShader(IShader&& other)                    = delete;
        IShader& operator = (IShader&& other)       = delete;
    };

    using Shader = IShader;

} // End namespaces

//**********************************************************************