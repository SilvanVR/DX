#pragma once
/**********************************************************************
    class: Mesh (D3D11Mesh.h)

    author: S. Hau
    date: March 8, 2018

    D3D11 implementation of an mesh.
**********************************************************************/

#include "../../i_mesh.h"

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
        void _Clear() override;
        void _CreateVertexBuffer(const ArrayList<Math::Vec3>& vertices) override;
        void _CreateIndexBuffer(const SubMesh& subMesh, I32 index) override;
        void _CreateUVBuffer(const ArrayList<Math::Vec2>& uvs) override;
        void _CreateColorBuffer(const ArrayList<Color>& colors) override;
        void _CreateNormalBuffer(const ArrayList<Math::Vec3>& normals) override;
        void _CreateTangentBuffer(const ArrayList<Math::Vec4>& tangents) override;

    private:
        VertexBuffer*   m_pVertexBuffer   = nullptr;
        VertexBuffer*   m_pColorBuffer    = nullptr;
        VertexBuffer*   m_pUVBuffer       = nullptr;
        VertexBuffer*   m_pNormalBuffer   = nullptr;
        VertexBuffer*   m_pTangentBuffer  = nullptr;

        // Array of index buffer. One indexbuffer for each submesh.
        ArrayList<IndexBuffer*> m_pIndexBuffers;

        //----------------------------------------------------------------------
        // IMesh Interface
        //----------------------------------------------------------------------
        friend class D3D11Renderer;
        void bind(const VertexLayout& vertLayout, U32 subMesh = 0) override;
        inline void _RecreateBuffers();

        //----------------------------------------------------------------------
        inline void _SetTopology(U32 subMesh);
        inline void _BindVertexBuffer(const VertexLayout& vertLayout, U32 subMesh);
        inline void _BindIndexBuffer(U32 subMesh);
        inline void _UpdateUVBuffer();
        inline void _UpdateVertexBuffer();
        inline void _UpdateColorBuffer();
        inline void _UpdateIndexBuffer(U32 index);
        inline void _UpdateNormalBuffer();
        inline void _UpdateTangentBuffer();

        //----------------------------------------------------------------------
        Mesh(const Mesh& other)               = delete;
        Mesh& operator = (const Mesh& other)  = delete;
        Mesh(Mesh&& other)                    = delete;
        Mesh& operator = (Mesh&& other)       = delete;
    };


} } // End namespaces