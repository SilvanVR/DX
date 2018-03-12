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

namespace OS { 


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


    //----------------------------------------------------------------------
    OS::SystemTime FileSystem::getLastWrittenFileTime( const char* physicalPath )
    {
        HANDLE hFile;
        FILETIME ftLastWriteTime;
        SYSTEMTIME stUTC, stLocal;
        hFile = CreateFile( physicalPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

        if (hFile == INVALID_HANDLE_VALUE)
        {
            LOG( "FileSystem-Windows: Could not open file '" + String( physicalPath ) + "'" );
        }

        if (not GetFileTime( hFile, NULL, NULL, &ftLastWriteTime ))
        {
            LOG( "FileSystem-Windows: Could not get the filetime of file '" + String( physicalPath ) + "'" );
        }

        CloseHandle( hFile );

        FileTimeToSystemTime( &ftLastWriteTime, &stUTC );
        SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &stLocal );

        SystemTime sysTime = {};
        sysTime.year         = stLocal.wYear;
        sysTime.month        = stLocal.wMonth;
        sysTime.day          = stLocal.wDay;
        sysTime.hour         = stLocal.wHour;
        sysTime.minute       = stLocal.wMinute;
        sysTime.second       = stLocal.wSecond;
        sysTime.milliseconds = stLocal.wMilliseconds;

        return sysTime;
    }

} // end namespaces


#endif