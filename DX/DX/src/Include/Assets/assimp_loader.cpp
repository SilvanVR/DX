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

    //----------------------------------------------------------------------
    // Check if the given material is the default one or a real material
    // The only way to do this in Assimp currently is to check the name.
    //----------------------------------------------------------------------
    bool isDefaultMaterial(const aiMaterial* material)
    {
        aiString name;
        material->Get( AI_MATKEY_NAME, name );
        return String( name.C_Str() ) == AI_DEFAULT_MATERIAL_NAME;
    }

    //----------------------------------------------------------------------
    MeshPtr AssimpLoader::LoadMesh( const OS::Path& path, MeshMaterialInfo* materials )
    {
        static const I32 IMPORTER_FLAGS = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace
                                          | aiProcess_FlipUVs | aiProcess_GenUVCoords | aiProcess_FindInvalidData;

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
        ArrayList<Math::Vec4> tangents;

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

            if (materials)
                materials->materialIndices.push_back( aMesh->mMaterialIndex );

            // Save base vertex for this submesh
            U32 baseVertex = (U32)vertices.size();

            // Fill vertices
            for (U32 j = 0; j < aMesh->mNumVertices; j++)
            {
                aiVector3D* pPos        = &(aMesh->mVertices[j]);
                aiVector3D* pTexCoord   = hasTextureCoords      ? &(aMesh->mTextureCoords[0][j]) : &Zero3D;
                aiVector3D* pNormal     = hasNormals            ? &(aMesh->mNormals[j])          : &Zero3D;
                aiVector3D* pTangent    = hasTangentsBitangents ? &(aMesh->mTangents[j])         : &Zero3D;

                vertices.emplace_back( pPos->x, pPos->y, pPos->z );
                uvs.emplace_back( pTexCoord->x, pTexCoord->y );
                normals.emplace_back( pNormal->x, pNormal->y, pNormal->z );
                tangents.emplace_back( pTangent->x, pTangent->y, pTangent->z, 1.0f );
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

        // Apply data to the mesh
        mesh->setVertices( vertices );
        mesh->setUVs( uvs );
        mesh->setNormals( normals );
        mesh->setTangents( tangents );

        // Load material information from the scene if requested.
        // Because Mesh-Files without a material file still have one material, i have to check it manually.
        // This can cause problems if the first material is named "DefaultMaterial".
        bool hasOnlyDefaultMaterial = scene->mNumMaterials == 1 && isDefaultMaterial( scene->mMaterials[0] );
        if ( materials != nullptr && scene->HasMaterials() && not hasOnlyDefaultMaterial )
        {
            materials->materials.resize( scene->mNumMaterials );
            for (U32 i = 0; i < scene->mNumMaterials; i++)
            {
                aiString texturePath;
                const aiMaterial* material = scene->mMaterials[i];

                // Albedo map
                if ( material->GetTexture( aiTextureType_DIFFUSE, 0, &texturePath ) == AI_SUCCESS )
                {
                    if (auto texture = scene->GetEmbeddedTexture(texturePath.C_Str())) {
                        LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                    }
                    else {
                        const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                        materials->materials[i].textures.push_back({ MaterialTextureType::Albedo, fullTexturePath });
                    }
                }

                // Normal map
                if ( material->GetTexture( aiTextureType_NORMALS, 0, &texturePath ) == AI_SUCCESS )
                {
                    if (auto texture = scene->GetEmbeddedTexture(texturePath.C_Str())) {
                        LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                    }
                    else {
                        const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                        materials->materials[i].textures.push_back({ MaterialTextureType::Normal, fullTexturePath });
                    }
                }

                // Shininess map
                if ( material->GetTexture( aiTextureType_SHININESS, 0, &texturePath ) == AI_SUCCESS )
                {
                    if (auto texture = scene->GetEmbeddedTexture(texturePath.C_Str())) {
                        LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                    }
                    else {
                        const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                        materials->materials[i].textures.push_back({ MaterialTextureType::Shininess, fullTexturePath });
                    }
                }
                
                // AO map
                if ( material->GetTexture( aiTextureType_AMBIENT, 0, &texturePath ) == AI_SUCCESS )
                {
                    if (auto texture = scene->GetEmbeddedTexture(texturePath.C_Str())) {
                        LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                    }
                    else {
                        const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                        materials->materials[i].textures.push_back({ MaterialTextureType::Ambient, fullTexturePath });
                    }
                }

                // Specular map
                if ( material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath ) == AI_SUCCESS )
                {
                    if (auto texture = scene->GetEmbeddedTexture(texturePath.C_Str())) {
                        LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                    }
                    else {
                        const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                        materials->materials[i].textures.push_back({ MaterialTextureType::Specular, fullTexturePath });
                    }
                }

                // Height map
                if ( material->GetTexture( aiTextureType_HEIGHT, 0, &texturePath ) == AI_SUCCESS )
                {
                    if (auto texture = scene->GetEmbeddedTexture(texturePath.C_Str())) {
                        LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                    }
                    else {
                        const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                        materials->materials[i].textures.push_back({ MaterialTextureType::Height, fullTexturePath });
                    }
                }

                // Displacement map
                if ( material->GetTexture( aiTextureType_DISPLACEMENT, 0, &texturePath ) == AI_SUCCESS )
                {
                    if (auto texture = scene->GetEmbeddedTexture(texturePath.C_Str())) {
                        LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                    }
                    else {
                        const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                        materials->materials[i].textures.push_back({ MaterialTextureType::Displacement, fullTexturePath });
                    }
                }

                // Displacement map
                if (material->GetTexture( aiTextureType_EMISSIVE, 0, &texturePath ) == AI_SUCCESS )
                {
                    if (auto texture = scene->GetEmbeddedTexture(texturePath.C_Str())) {
                        LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                    }
                    else {
                        const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                        materials->materials[i].textures.push_back({ MaterialTextureType::Emissive, fullTexturePath });
                    }
                }

                // Set Material-Properties
                aiColor3D color( 0.f, 0.f, 0.f );
                if ( material->Get( AI_MATKEY_COLOR_DIFFUSE, color ) == AI_SUCCESS )
                    materials->materials[i].diffuseColor = Color( (Byte)(color.r * 255), (Byte)(color.g * 255), (Byte)(color.b * 255));

                F32 opacity = 1.0f;
                if (material->Get( AI_MATKEY_OPACITY, opacity ) == AI_SUCCESS)
                    materials->materials[i].diffuseColor.setAlpha( Byte(opacity * 255) );
            }
        }

        return mesh;
    }


} // End namespaces