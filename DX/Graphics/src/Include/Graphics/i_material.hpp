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
        void        setShader(IShader* shader) { m_shader = shader; }

        IShader*    getShader() const { return m_shader; }

    protected:
        IShader* m_shader;

    private:
        //----------------------------------------------------------------------
        IMaterial(const IMaterial& other)               = delete;
        IMaterial& operator = (const IMaterial& other)  = delete;
        IMaterial(IMaterial&& other)                    = delete;
        IMaterial& operator = (IMaterial&& other)       = delete;
    };

    using Material = IMaterial;

} // End namespaces

//**********************************************************************