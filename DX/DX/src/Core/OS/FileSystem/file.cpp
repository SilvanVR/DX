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

      @Consideration:
        - If directories in given path does not exist:
           => recursively create directories
**********************************************************************/

#include "virtual_file_system.h"
#include "file_system.h"
#include "Core/locator.h"

namespace Core { namespace OS {

    #define FILE_EXISTS_AND_NOT_EOF() ASSERT( m_exists && !eof() && "File does not exist or read-cursor is at the end!" );

    //----------------------------------------------------------------------
    File::File( bool binary )
        : m_binary( binary )
    {}

    //----------------------------------------------------------------------
    File::File( const char* path, EFileMode mode, bool binary )
        : m_filePath( _GetPhysicalPath( path ) ), m_exists( _FileExists( m_filePath.c_str() ) ), 
          m_binary( binary ), m_mode( mode )
    {
        if (m_exists)
        {
            _OpenFile( m_mode, m_binary );
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
    bool File::open(const char* path, EFileMode mode)
    {
        ASSERT( not m_exists && "File was already opened!" );

        m_filePath  = _GetPhysicalPath( path );
        m_exists    = _FileExists( m_filePath.c_str() );
        m_mode      = mode;

        return _OpenFile( m_mode, m_binary );
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
    Size File::read( void* mem, Size amountOfBytes )
    {
        FILE_EXISTS_AND_NOT_EOF();

        // Ensure that reading does not happen after the end
        Size bytesToRead = amountOfBytes;
        if ( (m_readCursorPos + bytesToRead) > m_fileSize)
            bytesToRead = (m_fileSize - m_readCursorPos);

        if ( bytesToRead == 0 )
            return 0; // Nothing to read, file is empty or read cursor at end

        // Read bytes from file
        _File_Seek( m_readCursorPos );
        fread( mem, sizeof(Byte), bytesToRead, m_file );
        m_readCursorPos = ftell( m_file );

        return bytesToRead;
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

        if (m_fileSize == 0)
            return ""; // Nothing to read, file is empty

        // Create buffer
        char* buffer = new char[m_fileSize + 1];
        buffer[m_fileSize] = '\0';

        // Move to beginning and read all bytes
        _File_Seek( 0 );
        fread( buffer, sizeof(char), m_fileSize, m_file );

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

        _File_Seek( m_writeCursorPos );
    }

    void File::_WRITE_FUNC_END()
    {
        if (m_writeCursorPos == m_readCursorPos)
            m_eof = false;

        m_writeCursorPos = ftell( m_file );

        // Update filesize if we wrote over the end
        if (m_writeCursorPos > m_fileSize)
            m_fileSize = m_writeCursorPos;
    }

    void File::write( const Byte* data, Size amountOfBytes )
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
        fseek( m_file, 0L, SEEK_END );
    }

    void File::_APPEND_FUNC_END()
    {
        m_eof = false; 
        m_fileSize = ftell( m_file );
    }

    void File::append( const Byte* data, Size amountOfBytes )
    {
        _APPEND_FUNC_BEGIN();
        fwrite( data, sizeof(Byte), amountOfBytes, m_file );
        _APPEND_FUNC_END();
    }

    //----------------------------------------------------------------------
    void File::clear()
    {
        if ( m_exists )
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
        return FileSystem::exists( filePath );
    }

    //----------------------------------------------------------------------
    bool File::_OpenFile( EFileMode mode, bool binary )
    { 
        String cmode = "  ";

        switch ( mode )
        {
            case EFileMode::READ:                   cmode = binary ? "rb"  : "r";  break;
            case EFileMode::WRITE:                  cmode = binary ? "wb"  : "w";  break;
            case EFileMode::READ_WRITE:             cmode = binary ? "rb+" : "r+"; break;
            case EFileMode::READ_WRITE_OVERWRITE:   cmode = binary ? "wb+" : "w+"; break;
            case EFileMode::APPEND:                 cmode = binary ? "ab"  : "a";  break;
            case EFileMode::READ_WRITE_APPEND:      cmode = binary ? "ab+" : "a+"; break;
        }

        m_file = fopen( m_filePath.c_str(), cmode.c_str() );
        if (m_file != nullptr)
            m_writeCursorPos = ftell( m_file );

        _PeekNextCharAndSetEOF();

        m_fileSize = _GetFileSize();

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
        m_fileSize = m_readCursorPos = m_writeCursorPos = 0;
    }

    //----------------------------------------------------------------------
    void File::_CreateFile()
    {
        // Open file with appropriate flag to create it
        m_file = m_binary ? fopen( m_filePath.c_str(), "wb+" ) : fopen( m_filePath.c_str(), "w+" );
        ASSERT( m_file != NULL && "Could not create a new file" );

        m_exists = true;
    }

    //----------------------------------------------------------------------
    Size File::_GetFileSize() const
    {
        fseek( m_file, 0L, SEEK_END );
        long length = ftell( m_file );

        return length;
    }

    //----------------------------------------------------------------------
    void File::_PeekNextCharAndSetEOF()
    {
        int c = fgetc( m_file );

        if (c == EOF)
            m_eof = true;
        else
            ungetc( c, m_file );
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


    //**********************************************************************
    // ASYNC
    //**********************************************************************

    //----------------------------------------------------------------------
    void File::readAsync(const std::function<void(const String&)>& callback)
    {
        FILE_EXISTS_AND_NOT_EOF();

        ASYNC_JOB([=]() {
            String contents = readAll();
            callback( contents );
        });
    }

    //----------------------------------------------------------------------
    //void File::readAsync(const std::function<void(const Byte*, Size)>& callback)
    //{
    //    FILE_EXISTS_AND_NOT_EOF();

    //    ASYNC_JOB([=]() {
    //        //String contents = readAll();
    //        //callback(contents);
    //    });
    //}

} }