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

    #define FILE_EXISTS_AND_NOT_EOF() ASSERT( m_exists && !eof() && "File does not exist or read-cursor is at the end!" );

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
        if (m_exists)
        {
            _CloseFile();
        }
    }

    //----------------------------------------------------------------------
    bool File::open(const char* path, bool append)
    {
        ASSERT( not m_exists && "File was already opened!" );

        m_filePath  = _GetPhysicalPath( path );
        m_exists    = _FileExists( m_filePath.c_str() );

        return _OpenFile( append );
    }

    //----------------------------------------------------------------------
    void File::close()
    {
        _CloseFile();
        m_filePath.clear();
        m_exists = false;
    }

    //**********************************************************************
    // READING
    //**********************************************************************
    void File::_READ_FUNC_BEGIN()
    {
        FILE_EXISTS_AND_NOT_EOF();

        _File_Seek( m_readCursorPos );
    }

    void File::_READ_FUNC_END()
    {
        m_readCursorPos = ftell( m_file );
        _PeekNextCharAndSetEOF();
    }

    //----------------------------------------------------------------------
    String File::readLine()
    {
        FILE_EXISTS_AND_NOT_EOF();

        _File_Seek( m_readCursorPos );
        String line = _NextLine();
        m_readCursorPos = ftell( m_file );

        _PeekNextCharAndSetEOF();

        return line;
    }

    //----------------------------------------------------------------------
    String File::readAll() const
    {
        ASSERT( m_exists && "File does not exist or was already closed" );

        const Size fileSize = getFileSize();

        if ( fileSize == 0 )
            return ""; // Nothing to read, file is empty

        // Create buffer and read data into it
        char* buffer = new char[fileSize + 1];
        buffer[fileSize] = '\0';

        // Move to beginning and read all bytes
        _File_Seek( 0 );
        fread( buffer, sizeof(char), fileSize, m_file );

        String str( buffer );
        delete[] buffer;

        return str;
    }

    //**********************************************************************
    // WRITING
    //**********************************************************************
    void File::_WRITE_FUNC_BEGIN()
    {
        if (not m_exists)
            _CreateFile();

        _File_Seek(m_writeCursorPos);
    }

    void File::_WRITE_FUNC_END()
    {
        m_writeCursorPos = ftell(m_file);
        if (m_writeCursorPos == m_readCursorPos)
            m_eof = false;
    }

    void File::write(const Byte* data, Size amountOfBytes)
    {
        _WRITE_FUNC_BEGIN();
            fwrite( data, sizeof(Byte), amountOfBytes, m_file );
        _WRITE_FUNC_END();
    }

    //**********************************************************************
    // APPEND
    //**********************************************************************
    void File::_APPEND_FUNC_BEGIN()
    {
        if (not m_exists)
            _CreateFile();
        fseek(m_file, 0L, SEEK_END);
    }

    void File::_APPEND_FUNC_END()
    {
        m_eof = false; 
    }

    void File::append(const Byte* data, Size amountOfBytes)
    {
        _APPEND_FUNC_BEGIN();
        fwrite( data, sizeof(Byte), amountOfBytes, m_file );
        _APPEND_FUNC_END();
    }

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
    }

    //----------------------------------------------------------------------
    Size File::getFileSize() const
    {
        ASSERT( m_exists && "File does not exist" );

        fseek( m_file, 0L, SEEK_END );
        long length = ftell( m_file );

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
    bool File::_OpenFile( bool append )
    {
        if (append)
        {
            m_file = fopen( m_filePath.c_str(), "a+" );
            if ( m_file != nullptr )
                m_writeCursorPos = ftell( m_file );
        }
        else
        {
            m_file = fopen( m_filePath.c_str(), "w+" );
        }

        _PeekNextCharAndSetEOF();

        return (m_file != nullptr);
    }

    //----------------------------------------------------------------------
    void File::_CloseFile()
    {
        ASSERT( m_exists && "Can't close a non-existent file!" );

        int err = fclose( m_file );
        ASSERT( err == 0 );

        m_file = nullptr;
        m_eof = m_exists = false;
        m_readCursorPos = m_writeCursorPos = 0;
    }

    //----------------------------------------------------------------------
    void File::_CreateFile()
    {
        // create file with appropriate flags
        m_file = fopen( m_filePath.c_str(), "w+" );
        ASSERT( m_file != NULL && "Could not create a new file" );

        m_exists = true;
    }

    //----------------------------------------------------------------------
    void File::_PeekNextCharAndSetEOF()
    {
        int c = fgetc( m_file );

        if (c == EOF)
        {
            m_eof = true;
        }
        else
        {
            ungetc( c, m_file );
        }
    }

    //----------------------------------------------------------------------
    String File::_GetPhysicalPath(const char* vpath)
    {
        return VirtualFileSystem::resolvePhysicalPath( vpath );
    }

    //----------------------------------------------------------------------
    String File::_NextLine()
    {
        // @TODO: Increase buffer size automatically if line is longer
        static const int bufferSize = 1024;

        char lineBuffer[bufferSize];
        memset( lineBuffer, 0, bufferSize );

        // Read line. It returns whenever '\n', EOF or (bufferSize - 1) are reached.
        char* err = fgets( lineBuffer, bufferSize, m_file );

        if ( err == nullptr )
            return ""; // Special case: Empty line

        // Remove trailing '\n' and '\r'
        lineBuffer[strcspn( lineBuffer, "\r\n" )] = 0;
       
        return String( lineBuffer );
    }

    //----------------------------------------------------------------------
    void File::_File_Seek(long pos) const
    {
        fseek( m_file, pos, SEEK_SET );
    }


} }