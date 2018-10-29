#pragma once
/**********************************************************************
    class: AssimpLoader (assimp_loader.h)

    author: S. Hau
    date: May 29, 2018
**********************************************************************/

#include "Graphics/i_mesh.h"
#include "OS/FileSystem/path.h"
#include "mesh_material_info.hpp"
#include "Animation/skeleton.h"
#include "Animation/animation_clip.h"

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
        static MeshPtr LoadMesh(const OS::Path& path, MeshMaterialInfo* materials,
                                Animation::Skeleton* skeleton, ArrayList<Animation::AnimationClip>* animations);

        AssimpLoader() = delete;
        NULL_COPY_AND_ASSIGN(AssimpLoader)
    };

} // End namespaces