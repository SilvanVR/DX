#include "model.h"
/**********************************************************************
    class: Model (model.cpp)

    author: S. Hau
    date: December 25, 2017
**********************************************************************/

#include "locator.h"

namespace Assets {

    //----------------------------------------------------------------------
    Model::Model(const void* pVertices, U32 size, const void* pIndices, U32 size2, U32 numIndices )
    {
        auto mesh = Locator::getRenderer().createIndexedMesh( pVertices, size, pIndices, size2, numIndices );
        m_meshes.push_back( mesh );
    }

    //----------------------------------------------------------------------
    Model::~Model()
    {
        for (auto& mesh : m_meshes)
            SAFE_DELETE( mesh );
        m_meshes.clear();
    }

    //----------------------------------------------------------------------
    //Model::Model( const void* pVertices, const void* pIndices, const VertexLayout& vertexLayout )
    //{

    //}

} // End namespaces