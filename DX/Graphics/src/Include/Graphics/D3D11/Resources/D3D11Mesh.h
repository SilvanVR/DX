#pragma once
/**********************************************************************
    class: Mesh (D3D11Mesh.h)

    author: S. Hau
    date: March 8, 2018

    D3D11 implementation of an mesh.
**********************************************************************/

#include "i_mesh.h"

namespace Graphics { namespace D3D11 {

    class VertexBuffer;
    class IndexBuffer;

    //**********************************************************************
    class Mesh : public IMesh
    {
    public:
        Mesh() = default;
        ~Mesh();

    private:
        HashMap<StringID, VertexBuffer*> m_pVertexBuffers;

        // Array of index buffer. One indexbuffer for each submesh.
        ArrayList<IndexBuffer*> m_pIndexBuffers;

        //----------------------------------------------------------------------
        // IMesh Interface
        //----------------------------------------------------------------------
        void _Clear() override;
        void _CreateIndexBuffer(const SubMesh& subMesh, I32 index) override;
        void _DestroyIndexBuffer(I32 index) override;

        void _CreateBuffer(StringID name, const VertexStreamPtr& vs) override;
        void _DestroyBuffer(StringID name) override;

        //----------------------------------------------------------------------
        // IMesh Interface
        //----------------------------------------------------------------------
        void bind(const VertexLayout& vertLayout, U32 subMesh = 0) override;
        inline void _RecreateBuffers();

        //----------------------------------------------------------------------
        inline void _SetTopology(U32 subMesh);
        inline void _BindVertexBuffer(const VertexLayout& vertLayout);
        inline void _BindIndexBuffer(U32 subMesh);
        inline void _UpdateIndexBuffer(U32 index);

        NULL_COPY_AND_ASSIGN(Mesh)
    };


} } // End namespaces