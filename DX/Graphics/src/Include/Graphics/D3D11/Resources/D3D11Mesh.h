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
        D3D11Mesh();
        virtual ~D3D11Mesh();

        //----------------------------------------------------------------------
        // IMesh Interface
        //----------------------------------------------------------------------
        void clear() override;
        void bind() override;
        void setVertices(const ArrayList<Math::Vec3>& vertices) override;
        void setTriangles(const ArrayList<U32>& indices) override;
        void setColors(const ArrayList<Color>& colors) override;

    private:
        VertexBuffer*   pVertexBuffer   = nullptr;
        VertexBuffer*   pColorBuffer    = nullptr;
        IndexBuffer*    pIndexBuffer    = nullptr;

        //----------------------------------------------------------------------
        D3D11Mesh(const D3D11Mesh& other)               = delete;
        D3D11Mesh& operator = (const D3D11Mesh& other)  = delete;
        D3D11Mesh(D3D11Mesh&& other)                    = delete;
        D3D11Mesh& operator = (D3D11Mesh&& other)       = delete;
    };


} } // End namespaces