#pragma once
/**********************************************************************
    class: None (mesh_material_info.hpp)

    author: S. Hau
    date: June 1, 2018
**********************************************************************/

#include "OS/FileSystem/path.h"

namespace Assets {

    //----------------------------------------------------------------------
    enum class MaterialTextureType
    {
        Albedo,
        Normal,
        Ambient,
        Specular,
        Height,
        Shininess,
        Displacement,
        Emissive
    };

    //----------------------------------------------------------------------
    struct TextureInfo
    {
        MaterialTextureType type;
        OS::Path            filePath;
    };

    //----------------------------------------------------------------------
    // Contains information about materials from loaded model files.
    // To get the material for submesh N use:
    //  auto material = materials[subMesh];
    //----------------------------------------------------------------------
    struct MeshMaterialInfo
    {
        struct MaterialInfo
        {
            ArrayList<TextureInfo>  textures;
            Color                   diffuseColor = Color::WHITE;
        };

        //----------------------------------------------------------------------
        // Return the material info struct for a given subMesh index.
        //----------------------------------------------------------------------
        const MaterialInfo& operator[](I32 subMesh) const { return materials[materialIndices[subMesh]]; }

        //----------------------------------------------------------------------
        // Whether this struct contains any valid data. This should be called before using the "[]" operator, otherwise the program might crash.
        //----------------------------------------------------------------------
        bool isValid() const { return not materials.empty(); }

    private:
        ArrayList<U32> materialIndices; // Index of the array: Submesh, Value: Material (the index into the materials array)
        ArrayList<MaterialInfo> materials;

        friend class AssimpLoader; // Allow the loader to fill this struct with data
    };

} // End namespaces

