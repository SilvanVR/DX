#include "D3D11Mesh.h"
/**********************************************************************
    class: D3D11Mesh (D3D11Mesh.cpp)

    author: S. Hau
    date: March 8, 2018
**********************************************************************/

#include "../Pipeline/Buffers/D3D11Buffers.h"
#include "../../structs.hpp"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    D3D11Mesh::D3D11Mesh( const void* pVertices, U32 sizeInBytes )
    {
        pVertexBuffer = new D3D11::VertexBuffer( sizeInBytes, pVertices );
    }

    //----------------------------------------------------------------------
    D3D11Mesh::~D3D11Mesh()
    {
        SAFE_DELETE( pVertexBuffer );
    }

    //----------------------------------------------------------------------
    void D3D11Mesh::bind()
    {
        // @TODO: Get stride from VertexLayout for this mesh
        pVertexBuffer->bind( 0, sizeof(Vertex), 0 );
    }

    //**********************************************************************
    // D3D11IndexedMesh
    //**********************************************************************

    //----------------------------------------------------------------------
    D3D11IndexedMesh::D3D11IndexedMesh( const void* pVertices, U32 sizeInBytes, const void* pIndices, U32 sizeInBytes2, U32 numIndices )
        : IndexedMesh( numIndices )
    {
        pVertexBuffer = new D3D11::VertexBuffer( sizeInBytes, pVertices );
        pIndexBuffer  = new D3D11::IndexBuffer( sizeInBytes2, pIndices );
    }

    //----------------------------------------------------------------------
    D3D11IndexedMesh::~D3D11IndexedMesh()
    {
        SAFE_DELETE( pVertexBuffer );
        SAFE_DELETE( pIndexBuffer );
    }

    //----------------------------------------------------------------------
    void D3D11IndexedMesh::bind()
    {
        pVertexBuffer->bind( 0, sizeof(Vertex), 0 );
        pIndexBuffer->bind( DXGI_FORMAT_R32_UINT, 0 );
    }

} } // End namespaces