#include "resource_manager.h"

/**********************************************************************
    class: ResourceManager (resource_manager.cpp)

    author: S. Hau
    date: December 25, 2017

**********************************************************************/

#include "locator.h"

namespace Core { namespace Resources {

    //----------------------------------------------------------------------
    void ResourceManager::init()
    {
        Locator::getCoreEngine().subscribe( this );
    }

    //----------------------------------------------------------------------
    void ResourceManager::shutdown()
    {
        for (auto& mesh : m_meshes)
            SAFE_DELETE( mesh );
        m_meshes.clear();
    }

    //----------------------------------------------------------------------
    void ResourceManager::OnTick( Time::Seconds delta )
    {

    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    Graphics::Mesh* ResourceManager::createMesh()
    {
        auto mesh = Locator::getRenderer().createMesh();
        m_meshes.push_back( mesh );
        return mesh;
    }

    //----------------------------------------------------------------------
    Graphics::Mesh* ResourceManager::createMesh( const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices )
    {
        auto mesh = createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices );
        return mesh;
    }

} } // end namespaces