#pragma once
/**********************************************************************
    class: File (file.h)

    author: S. Hau
    date: October 16, 2017

    See below for a class description.

**********************************************************************/

namespace Core { namespace OS {

    //*********************************************************************
    // Represents a file on disk.
    // Allows both reading and writing.
    // Supports virtual file paths.
    //*********************************************************************
    class File
    {
    public:
        //----------------------------------------------------------------------
        // Creates a new file object. Use open() to open the file.
        //----------------------------------------------------------------------
        File();

        //----------------------------------------------------------------------
        // @Params:
        //  "path": Virtual path or Physical path on disk.
        //  "append": Whether add new contents directly to the end of the file.
        //----------------------------------------------------------------------
        explicit File(const char* vpath, bool append = false);
        ~File();

        //----------------------------------------------------------------------
        // @Params:
        //  "path": Virtual path or Physical path on disk.
        //  "append": Whether add new contents directly to the end of the file.
        // @Return:
        //  "Whether opening the file succeeded or not.
        //----------------------------------------------------------------------
        bool open(const char* path, bool append = false);

        //----------------------------------------------------------------------
        // Close the file manually
        //----------------------------------------------------------------------
        void close();

        //----------------------------------------------------------------------
        // @Return:
        //  Whether this file exists or not on disk.
        //----------------------------------------------------------------------
        bool exists() const { return m_exists; }

        //----------------------------------------------------------------------
        // @Return:
        //  The size of the file in bytes.
        //----------------------------------------------------------------------
        Size getFileSize() const;

        //----------------------------------------------------------------------
        // @Return:
        //  A single character from the file. Check if end is reached with "eof()".
        //----------------------------------------------------------------------
        U8      nextChar();
        U64     nextInt();
        double  nextDouble();

        //----------------------------------------------------------------------
        // @Return:
        //  A single line from the file. Check if end is reached with "eof()".
        //----------------------------------------------------------------------
        String readLine();

        //----------------------------------------------------------------------
        // @Return:
        //  The content of the whole file as a string.
        //  Independent of the read / write cursor.
        //  Dynamically allocates the buffer for the content.
        //----------------------------------------------------------------------
        String readAll() const;

        //----------------------------------------------------------------------
        // Write the given data into the file. If the file does not exist, it
        // will be created.
        // @Params:
        //  "data": The data which should be written.
        //  "fractionWidth": Only for floating point numbers. Amount of numbers
        //                   written after the dot.
        //  "amountOfBytes": Amount of bytes to write.
        //----------------------------------------------------------------------
        template <class... Args>
        void write(const char* str, Args&&... args)
        {
            _WRITE_FUNC_BEGIN();
            fprintf( m_file, str, args... );
            _WRITE_FUNC_END();
        }
        void write(const char* data)                 { write( "%s", data ); }
        void write(I32 data)                         { write( "%d", data ); }
        void write(U32 data)                         { write( "%u", data ); }
        void write(I64 data)                         { write( "%lld", data ); }
        void write(U64 data)                         { write( "%llu", data ); }
        void write(F32 data, Byte fractionWidth = 6) { write( String( "%." + TS( fractionWidth ) + "f" ).c_str(), data ); }
        void write(F64 data, Byte fractionWidth = 6) { write( String( "%." + TS( fractionWidth ) + "lf" ).c_str(), data ); }
        void write(const Byte* data, Size amountOfBytes);

        //----------------------------------------------------------------------
        // Write the given data to the end of the file. This does not modify
        // the write cursor. It simply adds the contents to the end.
        // If the file does not exist, it will be created.
        // @Params:
        //  See write function for param explanation.
        //----------------------------------------------------------------------
        template <class... Args>
        void append(const char* str, Args&&... args)
        {
            _APPEND_FUNC_BEGIN();
            fprintf( m_file, str, args... );
            _APPEND_FUNC_END();
        }
        void append(const char* data)                   { append( "%s", data ); }
        void append(I32 data)                           { append( "%d", data ); }
        void append(U32 data)                           { append( "%u", data ); }
        void append(I64 data)                           { append( "%lld", data ); }
        void append(U64 data)                           { append( "%llu", data ); }
        void append(F32 data, Byte fractionWidth = 6)   { append( String( "%." + TS( fractionWidth ) + "f" ).c_str(), data ); }
        void append(F64 data, Byte fractionWidth = 6)   { append( String( "%." + TS( fractionWidth ) + "lf" ).c_str(), data ); }
        void append(const Byte* data, Size amountOfBytes);

        //----------------------------------------------------------------------
        // Clears the whole file and resets both read + write cursor pos.
        //----------------------------------------------------------------------
        void clear();

        //----------------------------------------------------------------------
        // Delete this file. Does nothing if the file not exists.
        //----------------------------------------------------------------------
        void deleteFromDisk();

        //----------------------------------------------------------------------
        // @Return:
        //  The file extension from the path. Example: "test.png" => "png"
        //----------------------------------------------------------------------
        String getExtension() const;

        //----------------------------------------------------------------------
        // @Return:
        //  The directory path from the file-path.
        //  Example: "dir/test.png" => "dir/"
        //----------------------------------------------------------------------
        String getDirectoryPath() const;

        //----------------------------------------------------------------------
        // @Return:
        //  Whether the read cursor is at the end of file.
        //----------------------------------------------------------------------
        bool eof() const { return m_eof; }

        //----------------------------------------------------------------------
        // @Params:
        //  "pos": New position of the read cursor. 0 = Beginning of file.
        //----------------------------------------------------------------------
        void setReadCursor(long pos) { m_readCursorPos = pos; m_eof = false; }

        //----------------------------------------------------------------------
        // @Params:
        //  "pos": New position of the write cursor. 0 = Beginning of file.
        // Be careful about overwriting subsequent bytes or newlines.
        //----------------------------------------------------------------------
        void setWriteCursor(long pos) { m_writeCursorPos = pos; }

        //----------------------------------------------------------------------
        const String&   getFilePath()       const { return m_filePath; }
        long            tellWriteCursor()   const { return m_writeCursorPos; }
        long            tellReadCursor()    const { return m_readCursorPos; }
        void            flush()             const { fflush( m_file ); }

        template <class T> File& operator << (T data) { write( data ); return (*this); }
        template <class T> File& operator >> (T& buff) { buff = readAll(); return (*this); }

    private:
        String  m_filePath;
        FILE*   m_file;
        long    m_readCursorPos;
        long    m_writeCursorPos;
        bool    m_exists;
        bool    m_eof;

        //----------------------------------------------------------------------
        bool        _FileExists(const char* filePath);
        bool        _OpenFile(bool append = false);
        void        _CloseFile();
        void        _CreateFile();
        void        _PeekNextCharAndSetEOF();
        String      _GetPhysicalPath(const char* vpath);
        String      _NextLine();
        inline void _File_Seek(long pos) const;

        inline void _WRITE_FUNC_BEGIN();
        inline void _WRITE_FUNC_END();

        inline void _APPEND_FUNC_BEGIN();
        inline void _APPEND_FUNC_END();
    };

    //----------------------------------------------------------------------

    //using TextFile   = File<true>;
    //using BinaryFile = File<false>;

} }