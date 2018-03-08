#pragma once
/**********************************************************************
    class: Model (model.h)

    author: S. Hau
    date: December 25, 2017

    Represents a 3D model used for rendering.
    A Model consists of one or more meshes which can be rendered with
    different materials set up in the ModelRenderer-Component.
**********************************************************************/

#include "Graphics/i_mesh.hpp"

namespace Assets {

    //class VertexLayout;

    //**********************************************************************
    class Model
    {
    public:
        Model(const void* pVertices, U32 size, const void* pIndices, U32 size2, U32 numIndices);
        //Model(const void* pVertices, const void* pIndices, const VertexLayout& vertexLayout);
        ~Model();

        const ArrayList<Graphics::Mesh*>&   getMeshes() const { return m_meshes; }
        F32                                 numMeshes() const { return static_cast<F32>( m_meshes.size() ); }

    private:
        ArrayList<Graphics::Mesh*> m_meshes;

        //----------------------------------------------------------------------
        Model(const Model& other)               = delete;
        Model& operator = (const Model& other)  = delete;
        Model(Model&& other)                    = delete;
        Model& operator = (Model&& other)       = delete;
    };

} // End namespaces