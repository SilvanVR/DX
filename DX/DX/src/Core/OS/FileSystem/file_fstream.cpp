#include "file.h"
/**********************************************************************
    class: File (file.cpp)

    author: S. Hau
    date: October 13, 2017

**********************************************************************/

#include <fstream>

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    File::File()
        : m_filePath(""), m_exists(false), m_file(nullptr), m_readCursorPos(0), m_writeCursorPos(0), m_eof(false)
    {}

    //----------------------------------------------------------------------
    File::File(const char* path, bool append)
        : m_filePath(path), m_exists( _FileExists( path ) ), m_file(nullptr), m_readCursorPos(0), m_writeCursorPos(0), m_eof(false)
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
        m_filePath = path;
        m_exists = _FileExists(path);

        if (m_exists)
        {
            _OpenFile(append);
            return true;
        }

        return false;
    }

    //----------------------------------------------------------------------
    char File::readChar()
    {
        ASSERT( m_exists && !eof() && "File does not exist or read-cursor is at the end!" );

        m_file->seekg( m_readCursorPos );

        char c = m_file->get();

        m_readCursorPos = m_file->tellg();

        _CheckEOF();

        return c;
    }

    //----------------------------------------------------------------------
    String File::readLine()
    {
        ASSERT( m_exists && !eof() && "File does not exist or read-cursor is at the end!" );

        m_file->seekg( m_readCursorPos );

        String line;
        std::getline( (*m_file), line);

        m_readCursorPos = m_file->tellg();

        _CheckEOF();

        return line;
    }

    //----------------------------------------------------------------------
    String File::readAll() const
    {
        ASSERT( m_exists );

        const Size fileSize = getFileSize();

        if (fileSize == 0)
            return ""; // Nothing to read, file is empty

        // Create buffer and read data into it
        char* buffer = new char[fileSize + 1];
        buffer[fileSize] = '\0';

        // Move to beginning and read all bytes
        m_file->seekg( 0, std::ios::beg );
        m_file->read( buffer, fileSize );

        // Clear eof-bit
        m_file->clear();

        String str( buffer );
        delete[] buffer;

        return str;
    }

    //----------------------------------------------------------------------
    void File::write(const char* data)
    {
        if (not m_exists)
            _CreateFile();

        // not at end anymore
        if ( m_writeCursorPos == m_readCursorPos )
            m_eof = false;

        // write data to file
        m_file->seekg( m_writeCursorPos );
        (*m_file) << data;
        m_writeCursorPos = m_file->tellg();
    }

    //----------------------------------------------------------------------
    void File::append(const char* data)
    {
        if (not m_exists)
            _CreateFile();

        // not at end anymore
        if (m_writeCursorPos == m_readCursorPos)
            m_eof = false;

        // append data to the end
        m_file->seekg( 0, std::ios::end );
        (*m_file) << data;
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
        std::remove( m_filePath );
        m_exists = false;
    }

    //----------------------------------------------------------------------
    Size File::getFileSize() const
    {
        ASSERT( m_exists && "File does not exist" );

        m_file->seekg( 0, std::ios::end );
        auto length = m_file->tellg();
        ASSERT( length >= 0 );

        return length;
    }

    //----------------------------------------------------------------------
    String File::getExtension() const
    {
        String filePath( m_filePath );

        Size dotPosition = (filePath.find_last_of(".") + 1);
        return filePath.substr( dotPosition, (filePath.size() - dotPosition) );
    }

    //----------------------------------------------------------------------
    String File::getDirectoryPath() const
    {
        String filePath( m_filePath );
        return filePath.substr( 0, filePath.find_last_of( "/\\" ) ) + "/";
    }

    //----------------------------------------------------------------------
    bool File::_FileExists( const char* path )
    {
        struct stat buffer;
        return ( stat( path, &buffer ) == 0 );
    }

    //----------------------------------------------------------------------
    void File::_OpenFile( bool append )
    {
        auto flags = (std::fstream::in | std::fstream::out);

        if (append)
            flags |= std::fstream::app;

        m_file = new std::fstream( m_filePath, flags );

        _CheckEOF();
    }

    //----------------------------------------------------------------------
    void File::_CloseFile()
    {
        if (m_file != nullptr)
        {
            delete m_file;
            m_file = nullptr;
        }
    }

    //----------------------------------------------------------------------
    void File::_CreateFile()
    {
        auto flags = (std::fstream::in | std::fstream::out | std::fstream::trunc);
        m_file = new std::fstream( m_filePath, flags );

        if (m_file->is_open())
        {
            m_exists = true;
            m_eof = true;
        }
        else
        {
            ASSERT( false && "Could not create a file. Maybe the directory does not exist?" );
        }
    }

    //----------------------------------------------------------------------
    void File::_CheckEOF()
    {
        // Handle eof manually
        // Why do i handle it manually?
        // Because seekg() + tellg() does not return proper results
        // when the eof bit is set in the fstream. But for reading
        // the contents or calculating the size of the file, those
        // functions are needed.
        char eofCheck = m_file->peek();
        m_eof = m_file->eof();

        // clear eof-bit from file
        m_file->clear();
    }


} }