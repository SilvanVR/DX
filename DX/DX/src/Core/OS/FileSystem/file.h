#pragma once
/**********************************************************************
    class: File (file.h)

    author: S. Hau
    date: October 16, 2017

    See below for a class description.
**********************************************************************/

#include "path.h"

namespace Core { namespace OS {

    //*********************************************************************
    // Different modes used for working with a raw file.
    // READ                 | open for reading
    // WRITE                | open for writing (file need not exist)
    // APPEND               | open for appending (file need not exist)
    // READ_WRITE           | open for reading and writing, start at beginning
    // READ_WRITE_OVERWRITE | open for reading and writing (overwrite file)
    // READ_WRITE_APPEND    | open for reading and writing (append if file exists)
    //*********************************************************************
    enum class EFileMode
    {
        INVALID,
        READ,
        WRITE,
        APPEND,
        READ_WRITE,
        READ_WRITE_OVERWRITE,
        READ_WRITE_APPEND
    };

    //*********************************************************************
    // Represents a file on disk.
    // Does not support virtual file paths.
    // Use the class TextFile/BinaryFile instead.
    //*********************************************************************
    class File
    {
    public:
        //----------------------------------------------------------------------
        // Creates a new file object. Use open() to open the file.
        //----------------------------------------------------------------------
        File(bool binary = false);

        //----------------------------------------------------------------------
        // @Params:
        //  "path": Virtual path or Physical path on disk.
        //  "append": Whether add new contents directly to the end of the file.
        //----------------------------------------------------------------------
        explicit File(const Path& path, EFileMode mode = EFileMode::READ_WRITE, bool binary = false);
        virtual ~File();

        //----------------------------------------------------------------------
        // @Params:
        //  "path": Virtual path or Physical path on disk.
        //  "append": Whether add new contents directly to the end of the file.
        // @Return:
        //  "Whether opening the file succeeded or not.
        //----------------------------------------------------------------------
        bool open(const Path& path, EFileMode mode = EFileMode::READ_WRITE);

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
        // Reads data from the file. Use either c-style or one of the 
        // predefined functions which returns the result.
        // @Params:
        //  "str": Format of data to read in c-style (e.g."%s %f").
        //  "args": Arguments in which to put the data into.
        //----------------------------------------------------------------------
        template <typename... Args>
        void read(const char* str, Args&&... args)
        {
            _READ_FUNC_BEGIN();
            int numMatches = fscanf( m_file, str, args... );
            ASSERT( ( numMatches == sizeof...(Args) ) && "Scanf could not read valid data for every argument!" );
            _READ_FUNC_END();
        }
        I8   nextI8()   { I8 val;   read("%hhd", &val); return val; }
        U8   nextU8()   { U8 val;   read("%hhu", &val); return val; }
        I16  nextI16()  { I16 val;  read("%hd",  &val); return val; }
        U16  nextU16()  { U16 val;  read("%hu",  &val); return val; }
        I32  nextI32()  { I32 val;  read("%d",   &val); return val; }
        U32  nextU32()  { U32 val;  read("%u",   &val); return val; }
        I64  nextI64()  { I64 val;  read("%lld", &val); return val; }
        U64  nextU64()  { U64 val;  read("%llu", &val); return val; }
        F32  nextF32()  { F32 val;  read("%f",   &val); return val; }
        F64  nextF64()  { F64 val;  read("%lf",  &val); return val; }
        char nextChar() { char val; read("%c",   &val); return val; }

        //----------------------------------------------------------------------
        // Read raw bytes from the file into the given mem address.
        // @Params:
        //  "mem": The address of the buffer in which to put the data
        //  "amountOfBytes": Maximum amount of bytes to read. If the read cursor
        //                   exceeds the end it will only read up to the end.
        // @Return:
        //  Real amount of bytes which were read.
        //----------------------------------------------------------------------
        Size read(void* mem, Size amountOfBytes);

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
        // Write the given data into the file. If the file does not exist, it will be created.
        // Use either c-style (e.g. write("%d %f", int, float)) or one of the predefined functions.
        // @Params:
        //  "data": The data which should be written.
        //  "fractionWidth": Only for floating point numbers. Amount of numbers
        //                   written after the dot.
        //  "amountOfBytes": Amount of bytes to write.
        //----------------------------------------------------------------------
        template <typename... Args>
        void write(const char* str, Args&&... args)
        {
            _WRITE_FUNC_BEGIN();
            fprintf( m_file, str, args... );
            _WRITE_FUNC_END();
        }
        void write(const char* data)                 { write( "%s",   data ); }
        void write(I8 data)                          { write( "%hhd", data ); }
        void write(U8 data)                          { write( "%hhu", data ); }
        void write(I16 data)                         { write( "%hd",  data ); }
        void write(U16 data)                         { write( "%hu",  data ); }
        void write(I32 data)                         { write( "%d",   data ); }
        void write(U32 data)                         { write( "%u",   data ); }
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
        template <typename... Args>
        void append(const char* str, Args&&... args)
        {
            _APPEND_FUNC_BEGIN();
            fprintf( m_file, str, args... );
            _APPEND_FUNC_END();
        }
        void append(const char* data)                   { append( "%s",   data ); }
        void append(I8 data)                            { append( "%hhd", data ); }
        void append(U8 data)                            { append( "%hhu", data ); }
        void append(I16 data)                           { append( "%hd",  data ); }
        void append(U16 data)                           { append( "%hu",  data ); }
        void append(I32 data)                           { append( "%d",   data ); }
        void append(U32 data)                           { append( "%u",   data ); }
        void append(I64 data)                           { append( "%lld", data ); }
        void append(U64 data)                           { append( "%llu", data ); }
        void append(F32 data, Byte fractionWidth = 6)   { append( String( "%." + TS( fractionWidth ) + "f" ).c_str(), data ); }
        void append(F64 data, Byte fractionWidth = 6)   { append( String( "%." + TS( fractionWidth ) + "lf" ).c_str(), data ); }
        void append(const Byte* data, Size amountOfBytes);

        //----------------------------------------------------------------------
        // Clears the whole file and resets both read + write cursor pos.
        // If the file not exists, it will be created.
        //----------------------------------------------------------------------
        void clear();

        //----------------------------------------------------------------------
        // Delete this file. Does nothing if the file not exists.
        //----------------------------------------------------------------------
        void deleteFromDisk();

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
        const Path&     getFilePath()       const { return m_filePath; }
        Size            getFileSize()       const { return m_fileSize; }
        long            tellWriteCursor()   const { return m_writeCursorPos; }
        long            tellReadCursor()    const { return m_readCursorPos; }
        void            flush()             const { fflush( m_file ); }

        template <typename T> File& operator << (T data) { write( data ); return (*this); }
        template <typename T> File& operator >> (T& buff) { buff = readAll(); return (*this); }


        //----------------------------------------------------------------------
        // Read the file asynchronously via the job system. Be careful about
        // that the file does not get deleted until the read has been finished.
        //----------------------------------------------------------------------
        void readAsync(const std::function<void(const String& contents)>& callback);


    private:
        Path        m_filePath          = Path("", false);
        Size        m_fileSize          = 0;
        FILE*       m_file              = nullptr;
        long        m_readCursorPos     = 0;
        long        m_writeCursorPos    = 0;
        EFileMode   m_mode              = EFileMode::INVALID;
        bool        m_exists            = false;
        bool        m_eof               = false;
        bool        m_binary            = false;

        //----------------------------------------------------------------------
        bool        _FileExists(const char* filePath);
        bool        _OpenFile(EFileMode mode, bool binary);
        void        _CloseFile();
        void        _CreateFile();
        void        _PeekNextCharAndSetEOF();
        String      _NextLine();
        inline void _File_Seek(long pos) const;
        Size        _GetFileSize() const;

        //----------------------------------------------------------------------
        void _WRITE_FUNC_BEGIN();
        void _WRITE_FUNC_END();

        void _APPEND_FUNC_BEGIN();
        void _APPEND_FUNC_END();

        void _READ_FUNC_BEGIN();
        void _READ_FUNC_END();

        //----------------------------------------------------------------------
        File(const File& other)                 = delete;
        File& operator = (const File& other)    = delete;
        File(File&& other)                      = delete;
        File& operator = (File&& other)         = delete;
    };

    //*********************************************************************
    // Text-File
    //*********************************************************************
    class TextFile : public File
    {
    public:
        TextFile()
            : File( false )
        {}

        explicit TextFile::TextFile(const Path& path, EFileMode mode = EFileMode::READ_WRITE)
            : File( path, mode, false )
        {}
    };

    //*********************************************************************
    // Binary-File
    //*********************************************************************
    class BinaryFile : public File
    {
    public:
        BinaryFile() 
            : File(true) 
        {}

        explicit BinaryFile::BinaryFile(const Path& path, EFileMode mode = EFileMode::READ_WRITE)
            : File( path, mode, true )
        {}
    };

} }