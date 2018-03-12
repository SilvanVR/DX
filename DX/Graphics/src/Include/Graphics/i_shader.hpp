#pragma once
/**********************************************************************
    class: IShader (i_shader.hpp)

    author: S. Hau
    date: March 12, 2018

    Interface for a Shader class. A shader can be applied to a material.
    @TODO
**********************************************************************/

namespace Graphics {

    //**********************************************************************
    class IShader
    {
    public:
        IShader() = default;
        virtual ~IShader() {}

        //----------------------------------------------------------------------
        virtual void setShaderPaths( CString vertPath, CString fragPath ) = 0;
        virtual void compile( CString entryPoint ) = 0;
        virtual void bind() = 0;

    protected:


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