#pragma once
/**********************************************************************
    class: File (file.h)

    author: S. Hau
    date: October 13, 2017

    See below for a class description.
    
    Conclusion:
     - Can't use <fstream> because it allocates a static object in
       release mode and don't deallocates it. Because of this 
       The memory manager will always report a memory leak.

**********************************************************************/

namespace Core { namespace OS {

    //*********************************************************************
    // Represents a file on disk.
    // Allows both reading and writing.
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
        // "path": Physical path on disk.
        // "append": Whether add new contents directly to the end of the file.
        //----------------------------------------------------------------------
        File(const char* path, bool append = false);
        ~File();

        //----------------------------------------------------------------------
        // @Params:
        // "path": Physical path on disk.
        // "append": Whether add new contents directly to the end of the file.
        // @Return:
        // "Whether opening the file succeeded or not.
        //----------------------------------------------------------------------
        bool open(const char* path, bool append = false);

        //----------------------------------------------------------------------
        // @Return:
        //   Whether this file exists or not on disk.
        //----------------------------------------------------------------------
        bool exists() const { return m_exists; }

        //----------------------------------------------------------------------
        // @Return:
        //   The size of the file in bytes.
        //----------------------------------------------------------------------
        Size getFileSize() const;

        //----------------------------------------------------------------------
        // @Return:
        //   A single character from the file. Check if end is reached with "eof()"
        //----------------------------------------------------------------------
        char readChar();

        //----------------------------------------------------------------------
        // @Return:
        //   A single line from the file. Check if end is reached with "eof()"
        //----------------------------------------------------------------------
        String readLine();

        //----------------------------------------------------------------------
        // @Return:
        //   The content of the whole file as a string.
        //----------------------------------------------------------------------
        String readAll() const;

        //----------------------------------------------------------------------
        // Write the given data into the file. If the file does not exist, it
        // will be created.
        // @Params:
        //  The data which should be written to the file.
        //----------------------------------------------------------------------
        void write(const char* data);

        //----------------------------------------------------------------------
        // Write the given data to the end of the file. This does not modify
        // the write cursor. It simply adds the contents to the end.
        // If the file does not exist, it will be created.
        // @Params:
        //  The data which should be written to the end of the file.
        //----------------------------------------------------------------------
        void append(const char* data);

        //----------------------------------------------------------------------
        // Clears the whole file.
        //----------------------------------------------------------------------
        void clear();

        //----------------------------------------------------------------------
        // Delete this file. Does nothing if the file not exists.
        //----------------------------------------------------------------------
        void deleteFromDisk();

        //----------------------------------------------------------------------
        // @Return:
        //   The file extension from the path. Example: "test.png" => "png"
        //----------------------------------------------------------------------
        String getExtension() const;

        //----------------------------------------------------------------------
        // @Return:
        //   The directory path from the file-path. 
        //   Example: "dir/test.png" => "dir/"
        //----------------------------------------------------------------------
        String getDirectoryPath() const;

        //----------------------------------------------------------------------
        // @Return:
        //   Whether the read cursor is at the end of file
        //----------------------------------------------------------------------
        bool eof() const { return m_eof; }

        //----------------------------------------------------------------------
        // @Params:
        //  "pos": New position of the read cursor. 0 = Beginning of file.
        //----------------------------------------------------------------------
        void setReadCursor(Size pos) { m_readCursorPos = pos; }

        //----------------------------------------------------------------------
        // @Params:
        //  "pos": New position of the write cursor. 0 = Beginning of file.
        //----------------------------------------------------------------------
        void setWriteCursor(Size pos) { m_writeCursorPos = pos; }


        // Get FileTime


        //----------------------------------------------------------------------
        const char* getFilePath() const { return m_filePath; }

        File& operator << (const char* data) { write(data); return (*this); }
        File& operator >> (String& buff) { buff = readAll();  return (*this); }

    private:
        const char*     m_filePath;
        std::fstream*   m_file;
        Size            m_readCursorPos;
        Size            m_writeCursorPos;
        bool            m_exists;
        bool            m_eof;

        //----------------------------------------------------------------------
        bool _FileExists(const char* path);
        void _OpenFile(bool append = false);
        void _CloseFile();
        void _CreateFile();
        void _CheckEOF();
    };

} }