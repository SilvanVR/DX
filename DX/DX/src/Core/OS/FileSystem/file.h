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
        unsigned char readChar();

        //----------------------------------------------------------------------
        // @Return:
        //  A single line from the file. Check if end is reached with "eof()".
        //----------------------------------------------------------------------
        String readLine();

        //----------------------------------------------------------------------
        // @Return:
        //  The content of the whole file as a string.
        //  Independent of the read / write cursor.
        //  Dynamically allocates the buffer.
        //----------------------------------------------------------------------
        String readAll() const;

        //----------------------------------------------------------------------
        // Write the given data into the file. If the file does not exist, it
        // will be created.
        // @Params:
        //  "data": The data which should be written to the file.
        //  "fractionWidth": Only for floating point numbers. Amount of numbers
        //                   written after the dot.
        //----------------------------------------------------------------------
        void write(const char* data);
        void write(double data, Byte fractionWidth = 6);
        void write(int data);

        //----------------------------------------------------------------------
        // Write the given data to the end of the file. This does not modify
        // the write cursor. It simply adds the contents to the end.
        // If the file does not exist, it will be created.
        // @Params:
        //  The data which should be written to the end of the file.
        //----------------------------------------------------------------------
        void append(const char* data);
        void append(double data);
        void append(int data);

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
        void seekReadCursor(long pos) { m_readCursorPos = pos; m_eof = false; }

        //----------------------------------------------------------------------
        // @Params:
        //  "pos": New position of the write cursor. 0 = Beginning of file.
        // Be careful about overwriting subsequent bytes or newlines.
        //----------------------------------------------------------------------
        void seekWriteCursor(long pos) { m_writeCursorPos = pos; }

        //----------------------------------------------------------------------
        const String&   getFilePath() const { return m_filePath; }
        long            tellWriteCursor() const { return m_writeCursorPos; }
        long            tellReadCursor() const { return m_readCursorPos; }

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
    };

    //----------------------------------------------------------------------

    //using TextFile   = File<true>;
    //using BinaryFile = File<false>;

} }