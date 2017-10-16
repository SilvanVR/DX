#include "file.h"
/**********************************************************************
    class: File (file.cpp)

    author: S. Hau
    date: October 13, 2017

    fopen - modes:
      r - open for reading
      w - open for writing(file need not exist)
      a - open for appending(file need not exist)
      r + -open for reading and writing, start at beginning
      w + -open for reading and writing(overwrite file)
      a + -open for reading and writing(append if file exists)
**********************************************************************/

#include "virtual_file_system.h"

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    File::File()
        : m_filePath( "" ), m_exists( false ), m_file( nullptr ),
          m_readCursorPos( 0 ), m_writeCursorPos( 0 ), m_eof( false )
    {}

    //----------------------------------------------------------------------
    File::File(const char* path, bool append)
        : m_filePath( _GetPhysicalPath( path ) ), m_exists( _FileExists( m_filePath.c_str() ) ),
          m_file( nullptr ), m_readCursorPos( 0 ), m_writeCursorPos( 0 ), m_eof( false )
    {
        if (m_exists)
        {
            _OpenFile( append );
        }
    }

    //----------------------------------------------------------------------
    File::~File()
    {
        _CloseFile();
    }

    //----------------------------------------------------------------------
    bool File::open(const char* path, bool append)
    {
        m_filePath  = _GetPhysicalPath( path );
        m_exists    = _FileExists( m_filePath.c_str() );

        if (m_exists)
        {
            _OpenFile( append );
            return true;
        }

        return false;
    }

    //----------------------------------------------------------------------
    void File::close()
    {
        if (m_exists)
        {
            _CloseFile();
            m_filePath.clear();
            m_exists = m_eof = false;
            m_readCursorPos = m_writeCursorPos = 0;
        }
    }

    //----------------------------------------------------------------------
    unsigned char File::readChar()
    {
        ASSERT( m_exists && !eof() && "File does not exist or read-cursor is at the end!" );

        int c = fgetc( m_file );

        if (c == EOF)
        {
            m_eof = true;
        }

        return c;
    }

    //----------------------------------------------------------------------
    //String File::readLine()
    //{
    //    ASSERT( m_exists && !eof() && "File does not exist or read-cursor is at the end!" );

    //    m_file->seekg( m_readCursorPos );

    //    String line;
    //    std::getline( (*m_file), line);

    //    m_readCursorPos = m_file->tellg();

    //    _CheckEOF();

    //    return line;
    //}

    //----------------------------------------------------------------------
    //String File::readAll() const
    //{
    //    ASSERT( m_exists );

    //    const Size fileSize = getFileSize();

    //    if (fileSize == 0)
    //        return ""; // Nothing to read, file is empty

    //    // Create buffer and read data into it
    //    char* buffer = new char[fileSize + 1];
    //    buffer[fileSize] = '\0';

    //    // Move to beginning and read all bytes
    //    m_file->seekg( 0, std::ios::beg );
    //    m_file->read( buffer, fileSize );

    //    // Clear eof-bit
    //    m_file->clear();

    //    String str( buffer );
    //    delete[] buffer;

    //    return str;
    //}

#define WRITE_FUNC(type) void File::write(type data) \
    { \
        if (not m_exists) \
            _CreateFile(); \

    //----------------------------------------------------------------------
    void File::write(const char* data)
    {
        if (not m_exists)
            _CreateFile();

        fwrite( data, sizeof( char ), strlen( data ), m_file );
    }

    WRITE_FUNC(int)
        fprintf( m_file, "%d", data );
    }

    WRITE_FUNC(float)
        fprintf( m_file, "%f", data );
    }

    WRITE_FUNC(double)
        fprintf( m_file, "%f", data );
    }

    //----------------------------------------------------------------------
    //void File::append(const char* data)
    //{
    //    if (not m_exists)
    //        _CreateFile();

    //    // not at end anymore
    //    if (m_writeCursorPos == m_readCursorPos)
    //        m_eof = false;

    //    // append data to the end
    //    m_file->seekg( 0, std::ios::end );
    //    (*m_file) << data;
    //}

    //----------------------------------------------------------------------
    void File::clear()
    {
        _CloseFile();
        _CreateFile();
    }

    //----------------------------------------------------------------------
    void File::deleteFromDisk()
    {
        if (not m_exists)
            return;

        _CloseFile();
        std::remove( m_filePath.c_str() );
        m_exists = false;
    }

    //----------------------------------------------------------------------
    Size File::getFileSize() const
    {
        ASSERT( m_exists && "File does not exist" );

        fseek( m_file, 0L, SEEK_END );
        long length = ftell( m_file );

        // SEEK TO OLD POS?
        fseek( m_file, 0L, SEEK_SET );

        return length;
    }

    //----------------------------------------------------------------------
    String File::getExtension() const
    {
        Size dotPosition = ( m_filePath.find_last_of( "." ) + 1 );
        return m_filePath.substr( dotPosition, ( m_filePath.size() - dotPosition ) );
    }

    //----------------------------------------------------------------------
    String File::getDirectoryPath() const
    {
        return m_filePath.substr( 0, m_filePath.find_last_of( "/\\" ) ) + "/";
    }

    //----------------------------------------------------------------------
    bool File::_FileExists(const char* filePath)
    {
        struct stat buffer;
        return ( stat( filePath, &buffer ) == 0 );
    }

    //----------------------------------------------------------------------
    void File::_OpenFile( bool append )
    {
        if (append)
            m_file = fopen( m_filePath.c_str(), "a+" );
        else
            m_file = fopen( m_filePath.c_str(), "w+" );

        _CheckEOF();
    }

    //----------------------------------------------------------------------
    void File::_CloseFile()
    {
        if ( m_file != nullptr )
        {
            int err = fclose( m_file );
            ASSERT( err == 0 );
            m_file = nullptr;
        }
    }

    //----------------------------------------------------------------------
    void File::_CreateFile()
    {
        // create file with appropriate flags
        m_file = fopen( m_filePath.c_str(), "w+" );
        ASSERT( m_file != NULL );

        m_exists = true;
    }

    //----------------------------------------------------------------------
    void File::_CheckEOF()
    {

    }

    //----------------------------------------------------------------------
    String File::_GetPhysicalPath(const char* vpath)
    {
        return VirtualFileSystem::resolvePhysicalPath( vpath );
    }


} }