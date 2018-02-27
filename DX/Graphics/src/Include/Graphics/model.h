#pragma once
/**********************************************************************
    class: Model (model.h)

    author: S. Hau
    date: December 25, 2017

    - Represents a 3D model used for rendering

    A Model consists of one or more meshes which can be rendered with
    different materials set up in the CModelRenderer-Component

    What data do i need?
    - Vertex-Data + Vertex-Layout
    - (Index-Data)
**********************************************************************/

namespace Core { namespace Graphics {

    class VertexLayout;

    //**********************************************************************
    class Model
    {
    public:
        Model(const void* pVertices, const void* pIndices);
        Model(const void* pVertices, const void* pIndices, const VertexLayout& vertexLayout);
        ~Model() = default;

        U64 getID() const { return m_id; }

    private:
        U64 m_id = 0; // Link to the representation on the GPU

        //----------------------------------------------------------------------
        Model(const Model& other)               = delete;
        Model& operator = (const Model& other)  = delete;
        Model(Model&& other)                    = delete;
        Model& operator = (Model&& other)       = delete;
    };

} } // End namespaces