#include "model.h"
/**********************************************************************
    class: Model (model.cpp)

    author: S. Hau
    date: December 25, 2017
**********************************************************************/

//#include "locator.h"

namespace Graphics {

    //----------------------------------------------------------------------
    Model::Model(const void* pVertices, const void* pIndices)
    {
        //m_id = Locator::getRenderer().createIndexedModel( pVertices, pIndices );
    }

    //----------------------------------------------------------------------
    Model::Model( const void* pVertices, const void* pIndices, const VertexLayout& vertexLayout )
    {

    }

} // End namespaces