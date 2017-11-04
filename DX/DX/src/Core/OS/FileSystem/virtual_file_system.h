#pragma once
/**********************************************************************
    class: VirtualFileSystem (virtual_file_system.h)

    author: S. Hau
    date: October 13, 2017

    See below for a class description.
**********************************************************************/

namespace Core { namespace OS {

    //*********************************************************************
    // Maps symbolic directories to real directories. 
    // An symbolic directory begins with the "/" character.
    // Example:
    // VirtualFileSystem::mount("meshes", "res/meshes");
    // String path = VirtualFileSystem::resolvePhysicalPath("/meshes/block.obj");
    //   --> "res/meshes/block.obj"
    //*********************************************************************
    class VirtualFileSystem
    {
    public:
        //----------------------------------------------------------------------
        // Add a new symbolic directory.
        // @Params:
        // "name": The name of the symbolic directory.
        // "path": The path to associated with.
        // "overrideOldOne": Override old entry if one exists.
        //----------------------------------------------------------------------
        static void mount(const String& name, const char* path, bool overrideOldOne = true);

        //----------------------------------------------------------------------
        // Resolves the given virtual path to a physical path. Symbolic
        // directories begins with a '/' character e.g. "/textures/test.png"
        // @Params:
        // "virtualPath": Virtual path to resolve
        //----------------------------------------------------------------------
        static String resolvePhysicalPath(const String& virtualPath);

        //----------------------------------------------------------------------
        // Erase an entry.
        // @Params:
        // "name": The name of the symbolic directory to erase.
        //----------------------------------------------------------------------
        static void unmount(const String& name);

        //----------------------------------------------------------------------
        // Unmount all symbolic directories.
        //----------------------------------------------------------------------
        static void unmountAll();

    private:
        static std::map<String, const char*> mountPoints;

        //----------------------------------------------------------------------
        VirtualFileSystem()                                             = delete;
        VirtualFileSystem(const VirtualFileSystem& other)               = delete;
        VirtualFileSystem& operator = (const VirtualFileSystem& other)  = delete;
        VirtualFileSystem(VirtualFileSystem&& other)                    = delete;
        VirtualFileSystem& operator = (VirtualFileSystem&& other)       = delete;
    };

} } // end namespaces
