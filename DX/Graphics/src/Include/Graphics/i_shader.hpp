#pragma once
/**********************************************************************
    class: IShader (i_shader.hpp)

    author: S. Hau
    date: March 12, 2018

    Interface for a Shader class. A shader can be applied to a material.
    @TODO
**********************************************************************/

#include "OS/FileSystem/path.h"

namespace Graphics {

    class IMesh;

    //**********************************************************************
    class IShader
    {
    public:
        IShader() = default;
        virtual ~IShader() {}

        //----------------------------------------------------------------------
        // @Params:
        //  "vertPath": Path to the vertex shader file.
        //  "fragPath": Path to the fragment shader file.
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
        // @TODO make somehow private
        virtual void bind() = 0;
        virtual void drawMesh(IMesh* mesh, U32 subMeshIndex) = 0;

    private:
        //----------------------------------------------------------------------
        IShader(const IShader& other)               = delete;
        IShader& operator = (const IShader& other)  = delete;
        IShader(IShader&& other)                    = delete;
        IShader& operator = (IShader&& other)       = delete;
    };

    using Shader = IShader;

} // End namespaces

//**********************************************************************