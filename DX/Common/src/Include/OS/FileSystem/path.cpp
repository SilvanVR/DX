#include "path.h"
/**********************************************************************
    class: Path (path.cpp)

    author: S. Hau
    date: November 4, 2017
**********************************************************************/

#include "virtual_file_system.h"
#include "file_system.h"

namespace OS {

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
        Size dotPosition = m_path.find_last_of( "." ) + 1;
        return m_path.substr( dotPosition, ( m_path.size() - dotPosition ) );
    }

    //----------------------------------------------------------------------
    String Path::getDirectoryPath() const
    {
        return m_path.substr( 0, m_path.find_last_of( "/\\" ) ) + "/";
    }

    //----------------------------------------------------------------------
    String Path::getFileName() const
    {
        Size dotPosition = m_path.find_last_of( "." );
        Size slashPosition = m_path.find_last_of( "/\\" ) + 1;
        ASSERT( dotPosition > slashPosition );

        return m_path.substr( slashPosition, (dotPosition - slashPosition) );
    }

    //----------------------------------------------------------------------
    OS::SystemTime Path::getLastWrittenFileTime() const
    {
        ASSERT( exists() );
        return FileSystem::getLastWrittenFileTime( m_path.c_str() );
    }

    //----------------------------------------------------------------------
    ArrayList<String> Path::getDirectoryPaths() const
    {
        ArrayList<String> paths;

        I32 currentPos = 0;
        while ( currentPos < m_path.size() )
        {
            currentPos = m_path.find_first_of( "/\\", currentPos + 1 );
            if (currentPos > 0)
                paths.push_back( m_path.substr( 0, currentPos ) );
        }

        return paths;
    }

} // end namespaces

