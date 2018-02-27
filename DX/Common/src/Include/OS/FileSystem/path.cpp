#include "path.h"
/**********************************************************************
    class: Path (path.cpp)

    author: S. Hau
    date: November 4, 2017
**********************************************************************/

#include "virtual_file_system.h"
#include "file_system.h"

namespace Core{ namespace OS {

    //----------------------------------------------------------------------
    Path::Path( const char* path, bool resolveVirtualPath )
        : m_path( resolveVirtualPath ? VirtualFileSystem::resolvePhysicalPath( path ) : path )
    {
    }

    //----------------------------------------------------------------------
    bool Path::exists() const
    {
        return FileSystem::exists( m_path.c_str() );
    }

    //----------------------------------------------------------------------
    String Path::getExtension() const
    {
        Size dotPosition = ( m_path.find_last_of( "." ) + 1 );
        return m_path.substr( dotPosition, ( m_path.size() - dotPosition ) );
    }

    //----------------------------------------------------------------------
    String Path::getDirectoryPath() const
    {
        return m_path.substr( 0, m_path.find_last_of( "/\\" ) ) + "/";
    }

} } // end namespaces

