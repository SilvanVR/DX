#include "file_system.h"
/**********************************************************************
    class: FileSystem (file_system.cpp)

    author: S. Hau
    date: October 18, 2017

    Windows dependant implementations. 
**********************************************************************/

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Core { namespace OS { 


    //----------------------------------------------------------------------
    bool FileSystem::dirExists( const char* directory )
    {
        DWORD ftyp = GetFileAttributesA( directory );

        if (ftyp == INVALID_FILE_ATTRIBUTES)
            return false;

        return (ftyp & FILE_ATTRIBUTE_DIRECTORY);
    }

    //----------------------------------------------------------------------
    void FileSystem::createDirectory( const char* outputFolder )
    {
        bool success = CreateDirectory( outputFolder, NULL );

        if (success)
            return;

        DWORD lastError = GetLastError();
        if (lastError == ERROR_ALREADY_EXISTS)
        {
            // Failed to create directory because it already existed.
            printf( "Failed to create the new directory: %s. "
                    "Reason: Folder already exists.", outputFolder );
        }
        else
        {
            // Failed to create directory for whatever reason.
            printf( "Failed to create the new directory: %s", outputFolder );
        }
    }


} } // end namespaces


#endif