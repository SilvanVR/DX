#pragma once
/**********************************************************************
    class: D3D11Mesh (D3D11Mesh.h)

    author: S. Hau
    date: March 8, 2018

    D3D11 implementation of an mesh.
**********************************************************************/

#include "../../i_mesh.hpp"


namespace Graphics { namespace D3D11 {

    class VertexBuffer;
    class IndexBuffer;

    //**********************************************************************
    class D3D11Mesh : public Mesh
    {
    public:
        D3D11Mesh() = default;
        ~D3D11Mesh();

        //----------------------------------------------------------------------
        // IMesh Interface
        //----------------------------------------------------------------------
        void clear() override;
        void bind(U32 subMesh = 0) override;
        void setVertices(const ArrayList<Math::Vec3>& vertices) override;
        void setIndices(const ArrayList<U32>& indices, U32 subMesh = 0, U32 baseVertex = 0) override;
        void setColors(const ArrayList<Color>& colors) override;

    private:
        VertexBuffer*   m_pVertexBuffer   = nullptr;
        VertexBuffer*   m_pColorBuffer    = nullptr;

        // Array of index buffer. One indexbuffer for each submesh.
        ArrayList<IndexBuffer*> m_pIndexBuffers;

        //----------------------------------------------------------------------
        D3D11Mesh(const D3D11Mesh& other)               = delete;
        D3D11Mesh& operator = (const D3D11Mesh& other)  = delete;
        D3D11Mesh(D3D11Mesh&& other)                    = delete;
        D3D11Mesh& operator = (D3D11Mesh&& other)       = delete;
    };


} } // End namespaces