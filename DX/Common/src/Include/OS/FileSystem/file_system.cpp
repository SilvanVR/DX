#include "file_system.h"
/**********************************************************************
    class: FileSystem (file_system.cpp)

    author: S. Hau
    date: October 18, 2017

    Windows dependant implementations.
**********************************************************************/

namespace OS {


    //----------------------------------------------------------------------
    bool FileSystem::exists(const char* path)
    {
        struct stat buffer;
        return ( stat( path, &buffer ) == 0 );
    }

#ifndef _WIN32
    //----------------------------------------------------------------------
    void FileSystem::createDirectory(const char* outputFolder)
    {
        static_assert( false, "Not implemented for this OS. Either implement it or uncomment this line." );
    }

    //----------------------------------------------------------------------
    bool FileSystem::dirExists(const char* directory)
    {
        static_assert( false, "Not implemented for this OS. Either implement it or uncomment this line." );
    }

    //----------------------------------------------------------------------
    OS::SystemTime getLastWrittenFileTime(const char* physicalPath)
    {
        static_assert(false, "Not implemented for this OS. Either implement it or uncomment this line.");
    }
#endif


} // end namespaces