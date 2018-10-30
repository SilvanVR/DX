#include "assimp_loader.h"
/**********************************************************************
    class: AssimpLoader (assimp_loader.cpp)

    author: S. Hau
    date: May 29, 2018
**********************************************************************/

#include "Core/locator.h"
#include "Animation/skeleton.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

namespace Assets {

    //----------------------------------------------------------------------
    void ExtractVertexBoneWeights(const aiScene* scene, const MeshPtr& mesh, U32 numVertices);
    void ExtractSkeleton(const aiScene* scene, Animation::Skeleton* skeleton);
    void ExtractAnimation(const aiScene* scene, ArrayList<Animation::AnimationClip>* animationClips);
    void LoadMaterials(const aiScene* scene, const OS::Path& path, MeshMaterialInfo* materials);

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
    MeshPtr AssimpLoader::LoadMesh( const OS::Path& path, MeshMaterialInfo* materials, 
                                    Animation::Skeleton* skeleton, ArrayList<Animation::AnimationClip>* animations )
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
                materials->_AddMaterialIndex( aMesh->mMaterialIndex );

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
                    LOG_WARN( "Mesh contains other primitives than triangles, which is not supported" );
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

        ExtractVertexBoneWeights( scene, mesh, static_cast<U32>( vertices.size() ) );
        if (skeleton)
            ExtractSkeleton( scene, skeleton );
        if (animations)
            ExtractAnimation( scene, animations );

        // Load material information from the scene if requested.
        // Because Mesh-Files without a material file still have one material, i have to check it manually.
        // This can cause problems if the first material is named "DefaultMaterial".
        bool hasOnlyDefaultMaterial = scene->mNumMaterials == 1 && isDefaultMaterial( scene->mMaterials[0] );
        if ( materials != nullptr && scene->HasMaterials() && not hasOnlyDefaultMaterial )
            LoadMaterials( scene, path, materials );

        return mesh;
    }

    //----------------------------------------------------------------------
    void ExtractVertexBoneWeights( const aiScene* scene, const MeshPtr& mesh,  U32 numVertices )
    {
        // Extract Bone information from Assimps weird format to my own
        struct BoneWeight
        {
            I32 id = -1;
            F32 weight = 0.0f;
        };
        ArrayList<ArrayList<BoneWeight>> vertexBoneWeights( numVertices ); // Maps to every vertex a list of bone indices + weights
        for (U32 m = 0; m < scene->mNumMeshes; m++)
        {
            aiMesh* aMesh = scene->mMeshes[m];
            for (U32 b = 0; b < aMesh->mNumBones; b++)
            {
                auto& bone = aMesh->mBones[b];
                for (U32 w = 0; w < bone->mNumWeights; w++)
                {
                    auto& weight = bone->mWeights[w];
                    vertexBoneWeights[ weight.mVertexId + mesh->getBaseVertex( m ) ].push_back( { (I32)b, weight.mWeight } );
                }
            }
        }

        // Extract data from own format above in two separate arrays suitable for the mesh class
        ArrayList<Math::Vec4Int> boneIDs( numVertices );
        ArrayList<Math::Vec4>    boneWeights( numVertices );
        for (I32 vert = 0; vert < vertexBoneWeights.size(); vert++)
        {
            // Sort weights from max to min
            std::sort( vertexBoneWeights[vert].begin(), vertexBoneWeights[vert].end(), [](const BoneWeight& a, const BoneWeight& b) {
                return a.weight > b.weight;
            } );

            // Get first MAX_BONE_WEIGHTS and add data to the array
            F32 sum = 0;
            for (I32 i = 0; i < std::min( (I32)vertexBoneWeights[vert].size(), Animation::MAX_BONE_WEIGHTS ); i++)
            {
                boneIDs[vert][i] = vertexBoneWeights[vert][i].id;
                boneWeights[vert][i] = vertexBoneWeights[vert][i].weight;
                sum += boneWeights[vert][i];
            }

            // Normalize weights, so they always sum up to 1
            for (I32 i = 0; i < Animation::MAX_BONE_WEIGHTS; i++)
                boneWeights[vert][i] /= sum;
        }
        if (not boneIDs.empty())
            mesh->setBoneIDs( boneIDs );
        if (not boneWeights.empty())
            mesh->setBoneWeights( boneWeights );
    }

    //----------------------------------------------------------------------
    void ExtractSkeleton( const aiScene* scene, Animation::Skeleton* skeleton )
    {
        // Loop through all meshes and add unique joints to the skeleton
        HashMap<StringID, I32> jointIndexMap;
        for (U32 m = 0; m < scene->mNumMeshes; m++)
        {
            aiMesh* aMesh = scene->mMeshes[m];
            for (U32 b = 0; b < aMesh->mNumBones; b++)
            {
                auto& bone = aMesh->mBones[b];
                auto boneName = SID( bone->mName.C_Str() );
                if ( jointIndexMap.find( boneName ) == jointIndexMap.end() )
                {
                    Animation::SkeletonJoint joint;
                    joint.name = boneName;
                    joint.invBindPose = DirectX::XMMatrixTranspose( DirectX::XMMATRIX( &bone->mOffsetMatrix.a1 ) );
                    joint.parentIndex = -1;

                    jointIndexMap[boneName] = static_cast<I32>( jointIndexMap.size() );

                    skeleton->joints.push_back( joint );
                }
            }
        }

        // Now fix parent index for every joint
        for (auto& joint : skeleton->joints)
        {
            auto node = scene->mRootNode->FindNode( joint.name.c_str() );
            auto parentName = SID( node->mParent->mName.C_Str() );

            if (jointIndexMap.find( parentName ) != jointIndexMap.end())
                joint.parentIndex = jointIndexMap[parentName];
        }
    }

    //----------------------------------------------------------------------
    void ExtractAnimation( const aiScene* scene, ArrayList<Animation::AnimationClip>* animationClips )
    {
        for (U32 i = 0; i < scene->mNumAnimations; i++)
        {
            auto& anim = scene->mAnimations[i];

            Animation::AnimationClip clip;
            clip.name = SID( anim->mName.C_Str() );
            clip.duration = anim->mDuration * anim->mTicksPerSecond;

            for (U32 ch = 0; ch < anim->mNumChannels; ch++)
            {
                auto& channel = anim->mChannels[ch];

                Animation::JointSamples jointSamples;
                jointSamples.name = SID( channel->mNodeName.C_Str() );
                for (U32 pos = 0; pos < channel->mNumPositionKeys; pos++)
                {
                    auto& positionKey = channel->mPositionKeys[pos];

                    Animation::TranslationKey key;
                    key.time = positionKey.mTime;
                    key.translation = Math::Vec3{ positionKey.mValue.x, positionKey.mValue.y, positionKey.mValue.z };
                    jointSamples.translationKeys.push_back( key );
                }

                for (U32 rot = 0; rot < channel->mNumRotationKeys; rot++)
                {
                    auto rotationKey = channel->mRotationKeys[rot];

                    Animation::RotationKey key;
                    key.time = rotationKey.mTime;
                    key.rotation = Math::Quat{ rotationKey.mValue.x, rotationKey.mValue.y, rotationKey.mValue.z, rotationKey.mValue.w };
                    jointSamples.rotationKeys.push_back( key );
                }

                for (U32 sc = 0; sc < channel->mNumScalingKeys; sc++)
                {
                    auto scaleKey = channel->mScalingKeys[sc];

                    Animation::ScalingKey key;
                    key.time = scaleKey.mTime;
                    key.scale = Math::Vec3{ scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z };
                    jointSamples.scalingKeys.push_back( key );
                }
                clip.jointSamples.push_back( jointSamples );
            }

            animationClips->push_back( clip );
        }
    }

    //----------------------------------------------------------------------
    void LoadMaterials( const aiScene* scene, const OS::Path& path, MeshMaterialInfo* materials )
    {
        for (U32 i = 0; i < scene->mNumMaterials; i++)
        {
            aiString texturePath;
            const aiMaterial* aiMaterial = scene->mMaterials[i];

            auto& material = materials->_AddMaterial();

            // Albedo map
            if ( aiMaterial->GetTexture( aiTextureType_DIFFUSE, 0, &texturePath ) == AI_SUCCESS )
            {
                if (auto texture = scene->GetEmbeddedTexture( texturePath.C_Str() )) {
                    LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                }
                else {
                    const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                    material.textures.push_back({ MaterialTextureType::Albedo, fullTexturePath });
                }
            }

            // Normal map
            if ( aiMaterial->GetTexture( aiTextureType_NORMALS, 0, &texturePath ) == AI_SUCCESS )
            {
                if (auto texture = scene->GetEmbeddedTexture( texturePath.C_Str() )) {
                    LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                }
                else {
                    const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                    material.textures.push_back({ MaterialTextureType::Normal, fullTexturePath });
                }
            }

            // Shininess map
            if ( aiMaterial->GetTexture( aiTextureType_SHININESS, 0, &texturePath ) == AI_SUCCESS )
            {
                if (auto texture = scene->GetEmbeddedTexture( texturePath.C_Str() )) {
                    LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                }
                else {
                    const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                    material.textures.push_back({ MaterialTextureType::Shininess, fullTexturePath });
                }
            }
                
            // AO map
            if ( aiMaterial->GetTexture( aiTextureType_AMBIENT, 0, &texturePath ) == AI_SUCCESS )
            {
                if (auto texture = scene->GetEmbeddedTexture( texturePath.C_Str() )) {
                    LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                }
                else {
                    const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                    material.textures.push_back({ MaterialTextureType::Ambient, fullTexturePath });
                }
            }

            // Specular map
            if ( aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texturePath ) == AI_SUCCESS )
            {
                if (auto texture = scene->GetEmbeddedTexture( texturePath.C_Str() )) {
                    LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                }
                else {
                    const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                    material.textures.push_back({ MaterialTextureType::Specular, fullTexturePath });
                }
            }

            // Height map
            if ( aiMaterial->GetTexture( aiTextureType_HEIGHT, 0, &texturePath ) == AI_SUCCESS )
            {
                if (auto texture = scene->GetEmbeddedTexture( texturePath.C_Str() )) {
                    LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                }
                else {
                    const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                    material.textures.push_back({ MaterialTextureType::Height, fullTexturePath });
                }
            }

            // Displacement map
            if ( aiMaterial->GetTexture( aiTextureType_DISPLACEMENT, 0, &texturePath ) == AI_SUCCESS )
            {
                if (auto texture = scene->GetEmbeddedTexture( texturePath.C_Str() )) {
                    LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                }
                else {
                    const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                    material.textures.push_back({ MaterialTextureType::Displacement, fullTexturePath });
                }
            }

            // Displacement map
            if ( aiMaterial->GetTexture( aiTextureType_EMISSIVE, 0, &texturePath ) == AI_SUCCESS )
            {
                if (auto texture = scene->GetEmbeddedTexture( texturePath.C_Str() )) {
                    LOG_WARN( "Embedded texture found, but i have no clue how to load them :-)" );
                }
                else {
                    const String fullTexturePath = path.getDirectoryPath() + texturePath.C_Str();
                    material.textures.push_back({ MaterialTextureType::Emissive, fullTexturePath });
                }
            }

            // Set Material-Properties
            aiColor3D color( 0.f, 0.f, 0.f );
            if ( aiMaterial->Get( AI_MATKEY_COLOR_DIFFUSE, color ) == AI_SUCCESS )
                material.diffuseColor = Color( (Byte)(color.r * 255), (Byte)(color.g * 255), (Byte)(color.b * 255));

            F32 opacity = 1.0f;
            if ( aiMaterial->Get( AI_MATKEY_OPACITY, opacity ) == AI_SUCCESS)
                material.diffuseColor.setAlpha( Byte(opacity * 255) );
        }
    }

} // End namespaces