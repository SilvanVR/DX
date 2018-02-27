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

    }

    //----------------------------------------------------------------------
    void ResourceManager::OnTick( Time::Seconds delta )
    {

    }



} } // end namespaces