#include "virtual_file_system.h"
/**********************************************************************
    class: VFS (virtual_file_system.cpp)

    author: S. Hau
    date: October 13, 2017

    See below for a class description.

**********************************************************************/

#include "Core/locator.h"

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    std::map<String, const char*> VirtualFileSystem::mountPoints;

    //----------------------------------------------------------------------
    void VirtualFileSystem::mount( String name, const char* path, bool overrideOldOne )
    {
        if ( mountPoints.count( name ) != 0 && !overrideOldOne ) 
            return;

        LOG( "Mount virtual-path '" + name + "' to '" + String( path ) + "'", Logging::LOG_LEVEL_NOT_IMPORTANT );
        mountPoints[name] = path;
    }

    //----------------------------------------------------------------------
    void VirtualFileSystem::unmount( String name )
    {
        ASSERT( mountPoints.count( name ) != 0 );
        mountPoints.erase( name );
    }

    //----------------------------------------------------------------------
    void VirtualFileSystem::unmountAll()
    {
        mountPoints.clear();
    }

    //----------------------------------------------------------------------
    String VirtualFileSystem::resolvePhysicalPath( String virtualPath )
    {
        ASSERT( virtualPath.size() > 1 );

        if ( virtualPath[0] == '/' )
        {
            Size slashPosition = virtualPath.find_first_of( '/', 1 );
            String virtualDir = virtualPath.substr( 1, (slashPosition - 1) );

            ASSERT( mountPoints.count( virtualDir ) != 0 );

            return String( mountPoints[virtualDir] ) + virtualPath.substr( slashPosition );
        }

        return virtualPath;
    }


} } // end namespaces