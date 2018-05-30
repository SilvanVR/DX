#include "assimp_loader.h"
/**********************************************************************
    class: AssimpLoader (assimp_loader.cpp)

    author: S. Hau
    date: May 29, 2018
**********************************************************************/

#include "Core/locator.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

namespace Assets {

    static const I32 IMPORTER_FLAGS = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace
                                      | aiProcess_FlipUVs | aiProcess_GenUVCoords | aiProcess_FindInvalidData;

    //----------------------------------------------------------------------
    MeshPtr AssimpLoader::LoadMesh( const OS::Path& path )
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile( path.c_str(), IMPORTER_FLAGS );

        // If the import failed, report it
        if ( not scene )
        {
            String err = importer.GetErrorString();
            throw std::runtime_error( "AssimpLoader::LoadMesh(): " + err );
        }

        // Create new mesh
        MeshPtr mesh = RESOURCES.createMesh();

        ArrayList<Math::Vec3> vertices;
        ArrayList<Math::Vec2> uvs;
        ArrayList<Math::Vec3> normals;

        // Create submeshes for each mesh in the aiScene
        aiVector3D Zero3D( 0.0f, 0.0f, 0.0f );
        for (U32 m = 0; m < scene->mNumMeshes; m++)
        {
            aiMesh* aMesh = scene->mMeshes[m];
            bool hasTextureCoords       = aMesh->HasTextureCoords(0);
            bool hasNormals             = aMesh->HasNormals();
            bool hasTangentsBitangents  = aMesh->HasTangentsAndBitangents();

            if ( not hasTextureCoords )
                LOG_WARN( TS( m ) + "th Submesh of mesh '" + path.toString() + "' has no UV-Coordinates. All uvs set to zero." );
            if ( not hasNormals )
                LOG_WARN( TS( m ) + "th Submesh of mesh '" + path.toString() + "' has no Normals. All normals set to zero." );

            // Save base vertex for this submesh
            U32 baseVertex = (U32)vertices.size();

            // Fill vertices
            for (U32 j = 0; j < aMesh->mNumVertices; j++)
            {
                aiVector3D* pPos        = &(aMesh->mVertices[j]);
                aiVector3D* pTexCoord   = hasTextureCoords      ? &(aMesh->mTextureCoords[0][j]) : &Zero3D;
                aiVector3D* pNormal     = hasNormals            ? &(aMesh->mNormals[j])          : &Zero3D;
                aiVector3D* pTangent    = hasTangentsBitangents ? &(aMesh->mTangents[j])         : &Zero3D;
                aiVector3D* pBiTangent  = hasTangentsBitangents ? &(aMesh->mBitangents[j])       : &Zero3D;

                vertices.emplace_back( pPos->x, pPos->y, pPos->z );
                uvs.emplace_back( pTexCoord->x, pTexCoord->y );
                normals.emplace_back( pNormal->x, pNormal->y, pNormal->z );
            }

            // Fill Indices
            ArrayList<U32> indices;
            for (U32 k = 0; k < aMesh->mNumFaces; k++)
            {
                const aiFace& Face = aMesh->mFaces[k];
                if (Face.mNumIndices != 3)
                {
                    ASSERT(false && "check this");
                    continue;
                }

                indices.push_back( Face.mIndices[0] );
                indices.push_back( Face.mIndices[1] );
                indices.push_back( Face.mIndices[2] );
            }
            mesh->setIndices( indices, m, Graphics::MeshTopology::Triangles, baseVertex );
        }

        // Load materials from the scene
        if (scene->HasMaterials())
            LOG("Has materials", Color::BLUE);
            //loadMaterials(physicalPath, mesh, scene);

        mesh->setVertices( vertices );
        mesh->setUVs( uvs );
        mesh->setNormals( normals );

        return mesh;
    }


    //// Check if the given material is the default one or a real material
    //// The only way to do this in Assimp currently is to check the name
    //bool isDefaultMaterial(const aiMaterial* material)
    //{
    //    aiString name;
    //    material->Get(AI_MATKEY_NAME, name);
    //    return std::string(name.C_Str()) == AI_DEFAULT_MATERIAL_NAME;
    //}

    //// Tries to load a texture from the given material.
    //// Return nullptr if texture does not exist.
    //TexturePtr loadTexture(const aiMaterial* material, aiTextureType textureType, const std::string& filePath, bool logMissingTextureWarning)
    //{
    //    aiString texturePath;
    //    if (material->GetTextureCount(textureType) > 0 && material->GetTexture(textureType, 0, &texturePath) == AI_SUCCESS)
    //    {
    //        const std::string fullTexturePath = FileSystem::getDirectoryPath(filePath) + texturePath.C_Str();
    //        if (FileSystem::fileExists(fullTexturePath))
    //            return TEXTURE(fullTexturePath);
    //        else if (logMissingTextureWarning)
    //            Logger::Log("Could not find texture '" + fullTexturePath + "'", LOGTYPE_WARNING);
    //    }
    //    // Texture type does not exist in material so just return nullptr
    //    return nullptr;
    //}

    //// Load all textures specified in the scene object and make materials from it
    //void AssimpLoader::loadMaterials(const std::string& filePath, Mesh* mesh, const aiScene* scene)
    //{
    //    std::vector<TexturePtr>& textures = mesh->textures;
    //    std::map<uint32_t, MaterialPtr>& materials = mesh->materials;

    //    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    //    {
    //        const aiMaterial* material = scene->mMaterials[i];

    //        // Because Mesh-Files without a material file still have one material, i have to check it manually
    //        // This can cause problems if the first material is named "DefaultMaterial"
    //        if (scene->mNumMaterials == 1 && isDefaultMaterial(material))
    //            continue;

    //        PBRMaterialPtr newMaterial = PBRMATERIAL({ nullptr });

    //        // Diffuse-Texture
    //        auto diffuseMap = loadTexture(material, aiTextureType_DIFFUSE, filePath, true);
    //        bool hasDiffuseMap = diffuseMap != nullptr;
    //        if (hasDiffuseMap)
    //        {
    //            newMaterial->setTexture(SHADER_DIFFUSE_MAP_NAME, diffuseMap);
    //            textures.push_back(diffuseMap);
    //        }
    //        else { // Diffuse-Texture is not even present in the material-class
    //            std::string missingTextureMessage = "There is no diffuse texture specified for material #" + TS(i) +
    //                " for file " + filePath;
    //            Logger::Log(missingTextureMessage, LOGTYPE_WARNING);
    //        }

    //        if (hasDiffuseMap)
    //        {
    //            // Normal-Map
    //            auto normalMap = loadTexture(material, aiTextureType_NORMALS, filePath, false);
    //            if (normalMap != nullptr)
    //            {
    //                newMaterial->setMatNormalMap(normalMap);
    //                textures.push_back(normalMap);
    //            }

    //            // AO-Map
    //            auto aoMap = loadTexture(material, aiTextureType_AMBIENT, filePath, false);
    //            if (aoMap != nullptr)
    //            {
    //                newMaterial->setMatAOMap(aoMap);
    //                textures.push_back(aoMap);
    //            }

    //            // Metalness (Specular)-Map 
    //            auto metallicMap = loadTexture(material, aiTextureType_SPECULAR, filePath, false);
    //            if (metallicMap != nullptr)
    //            {
    //                newMaterial->setMatMetallicMap(metallicMap);
    //                textures.push_back(metallicMap);
    //            }

    //            // Roughness-Map
    //            auto roughnessMap = loadTexture(material, aiTextureType_SHININESS, filePath, false);
    //            if (roughnessMap != nullptr)
    //            {
    //                newMaterial->setMatRoughnessMap(roughnessMap);
    //                textures.push_back(roughnessMap);
    //            }

    //            // Displacement-Map (aiTextureType_DISPLACEMENT or aiTextureType_HEIGHT)
    //            auto displacementMap = loadTexture(material, aiTextureType_DISPLACEMENT, filePath, false);
    //            if (!displacementMap.isValid())
    //                displacementMap = loadTexture(material, aiTextureType_HEIGHT, filePath, false);
    //            if (displacementMap.isValid())
    //            {
    //                newMaterial->setMatDisplacementMap(displacementMap);
    //                textures.push_back(displacementMap);
    //            }

    //            //int normal = material->GetTextureCount(aiTextureType_NORMALS);
    //            //int specular = material->GetTextureCount(aiTextureType_SPECULAR);
    //            //int disp = material->GetTextureCount(aiTextureType_DISPLACEMENT);
    //            //int height = material->GetTextureCount(aiTextureType_HEIGHT);
    //            //int ambent = material->GetTextureCount(aiTextureType_AMBIENT);
    //            //int emmissive = material->GetTextureCount(aiTextureType_EMISSIVE);
    //            //int shininess = material->GetTextureCount(aiTextureType_SHININESS);

    //            // Set Material-Properties
    //            for (unsigned int j = 0; j < material->mNumProperties; j++)
    //            {
    //                aiMaterialProperty* prop = material->mProperties[j];
    //                // Add parameters to material
    //            }
    //        }
    //        materials[i] = newMaterial;
    //    }
    //}

} // End namespaces