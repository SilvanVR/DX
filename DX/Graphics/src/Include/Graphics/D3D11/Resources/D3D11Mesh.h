#pragma once
/**********************************************************************
    class: Mesh (D3D11Mesh.h)

    author: S. Hau
    date: March 8, 2018

    D3D11 implementation of an mesh.
**********************************************************************/

#include "../../i_mesh.hpp"

namespace Graphics { namespace D3D11 {

    class VertexBuffer;
    class IndexBuffer;

    //**********************************************************************
    class Mesh : public IMesh
    {
    public:
        Mesh() = default;
        ~Mesh();

        //----------------------------------------------------------------------
        // IMesh Interface
        //----------------------------------------------------------------------
        void clear() override;
        void setVertices(const ArrayList<Math::Vec3>& vertices) override;
        void setIndices(const ArrayList<U32>& indices, U32 subMesh = 0, 
                        MeshTopology topology = MeshTopology::Triangles, U32 baseVertex = 0) override;
        void setColors(const ArrayList<Color>& colors) override;
        void setUVs(const ArrayList<Math::Vec2>& uvs) override;

    private:
        VertexBuffer*   m_pVertexBuffer   = nullptr;
        VertexBuffer*   m_pColorBuffer    = nullptr;
        VertexBuffer*   m_pUVBuffer       = nullptr;

        // Array of index buffer. One indexbuffer for each submesh.
        ArrayList<IndexBuffer*> m_pIndexBuffers;

        //----------------------------------------------------------------------
        // IMesh Interface
        //----------------------------------------------------------------------
        friend class D3D11Renderer;
        void bind(const VertexLayout& vertLayout, U32 subMesh = 0) override;
        void _RecreateBuffers();

        //----------------------------------------------------------------------
        inline void _SetTopology(U32 subMesh);
        inline void _BindVertexBuffer(const VertexLayout& vertLayout, U32 subMesh);
        inline void _BindIndexBuffer(U32 subMesh);

        //----------------------------------------------------------------------
        Mesh(const Mesh& other)               = delete;
        Mesh& operator = (const Mesh& other)  = delete;
        Mesh(Mesh&& other)                    = delete;
        Mesh& operator = (Mesh&& other)       = delete;
    };


} } // End namespaces