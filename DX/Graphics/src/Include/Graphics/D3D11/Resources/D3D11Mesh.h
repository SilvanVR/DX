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
        D3D11Mesh( const void* pVertices, U32 sizeInBytes );
        virtual ~D3D11Mesh();

        //----------------------------------------------------------------------
        void bind() override;

    private:
        VertexBuffer* pVertexBuffer = nullptr;

        //----------------------------------------------------------------------
        D3D11Mesh(const D3D11Mesh& other)               = delete;
        D3D11Mesh& operator = (const D3D11Mesh& other)  = delete;
        D3D11Mesh(D3D11Mesh&& other)                    = delete;
        D3D11Mesh& operator = (D3D11Mesh&& other)       = delete;
    };

    //**********************************************************************
    class D3D11IndexedMesh : public IndexedMesh
    {
    public:
        D3D11IndexedMesh(const void* pVertices, U32 sizeInBytes, const void* pIndices, U32 sizeInBytes2, U32 numIndices);
        ~D3D11IndexedMesh();

        //----------------------------------------------------------------------
        void bind() override;

    private:
        VertexBuffer*   pVertexBuffer = nullptr;
        IndexBuffer*    pIndexBuffer  = nullptr;

        //----------------------------------------------------------------------
        D3D11IndexedMesh(const D3D11IndexedMesh& other)               = delete;
        D3D11IndexedMesh& operator = (const D3D11IndexedMesh& other)  = delete;
        D3D11IndexedMesh(D3D11IndexedMesh&& other)                    = delete;
        D3D11IndexedMesh& operator = (D3D11IndexedMesh&& other)       = delete;
    };

} } // End namespaces