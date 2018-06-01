#pragma once
/**********************************************************************
    class: AssimpLoader (assimp_loader.h)

    author: S. Hau
    date: May 29, 2018
**********************************************************************/

#include "Graphics/i_mesh.h"
#include "OS/FileSystem/path.h"
#include "mesh_material_info.hpp"

namespace Assets { 

    //*********************************************************************
    class AssimpLoader
    {
    public:
        ~AssimpLoader() = default;

        //----------------------------------------------------------------------
        // @Params:
        //  "path": The file-path to the mesh file.
        //----------------------------------------------------------------------
        static MeshPtr LoadMesh(const OS::Path& path, MeshMaterialInfo* materials);

    private:

        //----------------------------------------------------------------------
        AssimpLoader()                                        = delete;
        AssimpLoader(const AssimpLoader& other)               = delete;
        AssimpLoader& operator = (const AssimpLoader& other)  = delete;
        AssimpLoader(AssimpLoader&& other)                    = delete;
        AssimpLoader& operator = (AssimpLoader&& other)       = delete;
    };

} // End namespaces