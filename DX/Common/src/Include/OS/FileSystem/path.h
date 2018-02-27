#pragma once
/**********************************************************************
    class: Path (path.h)

    author: S. Hau
    date: November 4, 2017
**********************************************************************/

namespace Core{ namespace OS {

    //**********************************************************************
    // Class for dealing with raw paths and translating virtual paths
    // to physical paths if desired.
    //**********************************************************************
    class Path
    {
    public:
        //----------------------------------------------------------------------
        // @Params:
        //  "path": The physical (or virtual) path on disk
        //  "resolveVirtualPath": Resolve the path into a physical path
        //----------------------------------------------------------------------
        Path(const char* path = "", bool resolveVirtualPath = true);
        ~Path() = default;

        //----------------------------------------------------------------------
        operator            const char* () const { return m_path.c_str(); }
        explicit operator   String      () const { return toString(); }

        //----------------------------------------------------------------------
        const char* c_str()     const { return m_path.c_str(); }
        String      toString()  const { return m_path; }

        //----------------------------------------------------------------------
        // @Return: 
        //  True if a file exists on this path, otherwise false
        //----------------------------------------------------------------------
        bool exists() const;

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

    private:
        String m_path;
    };


} } // end namespaces