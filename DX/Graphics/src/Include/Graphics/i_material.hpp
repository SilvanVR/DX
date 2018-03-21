#pragma once
/**********************************************************************
    class: IMaterial (i_material.hpp)

    author: S. Hau
    date: March 12, 2018

    Interface for a Material class. Each material is rendered by a
    shader (which can consists of vertex/frag/geo/tesselation) and
    contains the necessary data for this shader.
**********************************************************************/

#include "i_shader.hpp"

namespace Graphics {

    //**********************************************************************
    class IMaterial
    {
    public:
        IMaterial() = default;
        virtual ~IMaterial() {}

        //----------------------------------------------------------------------
        void        setShader(IShader* shader) { m_shader = shader; _ChangedShader(); }

        //----------------------------------------------------------------------
        IShader*    getShader() const { return m_shader; }

        //----------------------------------------------------------------------
        // Set material parameters.
        //----------------------------------------------------------------------
        void setFloat(CString name, F32 val)                { m_floatMap[ SID( name ) ] = val; _SetFloat( name, val ); }
        void setVec4(CString name, const Math::Vec4& vec)   { m_vec4Map[ SID( name ) ] = vec; _SetVec4( name, vec ); }

    protected:
        IShader*    m_shader = nullptr;

        // Data maps
        HashMap<StringID, F32>          m_floatMap;
        HashMap<StringID, Math::Vec4>   m_vec4Map;

        // Each API should decide themselves how to efficiently update their data
        virtual void _SetFloat(CString name, F32 val) = 0;
        virtual void _SetVec4(CString name, const Math::Vec4& vec) = 0;

    private:
        //----------------------------------------------------------------------
        friend class D3D11Renderer;
        virtual void bind() = 0;
        virtual void _ChangedShader() = 0;

        //----------------------------------------------------------------------
        IMaterial(const IMaterial& other)               = delete;
        IMaterial& operator = (const IMaterial& other)  = delete;
        IMaterial(IMaterial&& other)                    = delete;
        IMaterial& operator = (IMaterial&& other)       = delete;
    };

    using Material = IMaterial;

} // End namespaces

//**********************************************************************