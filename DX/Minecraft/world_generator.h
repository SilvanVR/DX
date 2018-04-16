#pragma once
/**********************************************************************
    class: WorldGeneration (world_generator.h)

    author: S. Hau
    date: April 16, 2018

    Script which generates chunks of block data using perlin noise and
    the polyvox library to generate polygon meshes for rendering.
**********************************************************************/
#include <DX.h>
#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/LargeVolume.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Material.h"

#define STB_PERLIN_IMPLEMENTATION 
#include "stb_perlin.hpp"

class NoiseMap
{
public:
    NoiseMap(I32 width, I32 height, F32 scale = 0.3f)
        : m_width(width), m_height(height)
    {
        m_noiseMap = new F32[width * height];

        if (scale < 0.0f)
            scale = 0.0001f;

        for (I32 y = 0; y < height; y++)
        {
            for (I32 x = 0; x < width; x++)
            {
                F32 sampleX = x / scale;
                F32 sampleY = y / scale;
                m_noiseMap[x + y * width] = stb_perlin_noise3(sampleX, sampleY, 0, 0, 0, 0);
            }
        }
    }
    ~NoiseMap() { delete m_noiseMap; }

    F32 getValue(I32 x, I32 y) const { return m_noiseMap[x + y * m_width]; }

private:
    F32*    m_noiseMap = nullptr;
    I32     m_width, m_height;
};


#define NUM_MATERIALS 3
using Block = PolyVox::Material8;

void createSphereInVolume(PolyVox::LargeVolume<PolyVox::Material8>& volData, float fRadius)
{
    //This vector hold the position of the center of the volume
    PolyVox::Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);
    //Vector3DFloat v3dVolCenter(0,0,0);

    //This three-level for loop iterates over every voxel in the volume
    for (int z = 0; z < volData.getDepth(); z++)
    {
        for (int y = 0; y < volData.getHeight(); y++)
        {
            for (int x = 0; x < volData.getWidth(); x++)
            {
                //Store our current position as a vector...
                PolyVox::Vector3DFloat v3dCurrentPos(x, y, z);
                //And compute how far the current position is from the center of the volume
                float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

                uint8_t uVoxelValue = 0;

                //If the current voxel is less than 'radius' units from the center then we make it solid.
                if (fDistToCenter <= fRadius)
                {
                    //Our new voxel value
                    uVoxelValue = Math::Random::Int(1, NUM_MATERIALS);
                }

                //Wrte the voxel value into the volume	
                volData.setVoxelAt(x, y, z, uVoxelValue);
            }
        }
    }
}

//**********************************************************************
class WorldGeneration : public Components::IComponent
{
    MaterialPtr                 m_chunkMaterial;
    PolyVox::LargeVolume<Block> m_volData;

    I32 size = 128;
    F32 scale = 0.3f;
    F32 speed = 5.0f;
    Texture2DPtr m_noiseMap;

public:
    WorldGeneration() : m_volData(PolyVox::Region(PolyVox::Vector3DInt32(0, 0, 0), PolyVox::Vector3DInt32(63, 63, 63))) {}

    //----------------------------------------------------------------------
    void addedToGameObject(GameObject* go) override
    {
        //_SetupShaderAndMaterial();

        //createSphereInVolume(m_volData, 30);

        //// A mesh object to hold the result of surface extraction
        //PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> mesh;

        //// Create a surface extractor. 
        //PolyVox::CubicSurfaceExtractorWithNormals< PolyVox::LargeVolume<Block> > surfaceExtractor(&m_volData, m_volData.getEnclosingRegion(), &mesh);
        ////PolyVox::MarchingCubesSurfaceExtractor< PolyVox::LargeVolume<Block> > surfaceExtractor(&m_volData, m_volData.getEnclosingRegion(), &mesh);

        //// Execute the surface extractor.
        //surfaceExtractor.execute();

        //// BUILD FIRST CHUNK
        //auto chunkMesh = _BuildMeshForRendering(mesh);
        //auto chunkGO = getGameObject()->getScene()->createGameObject();
        //chunkGO->addComponent<Components::MeshRenderer>(chunkMesh, m_chunkMaterial);
        //chunkGO->getComponent<Components::Transform>()->position = (-m_volData.getWidth() / 2.0f, -m_volData.getHeight() / 2.0f, -m_volData.getDepth() / 2.0f);

        // VISUALIZATION OF PERLIN NOISE      
        m_noiseMap = RESOURCES.createTexture2D(size, size, Graphics::TextureFormat::BGRA32);
        Color col = Color::Lerp(Color::BLACK, Color::WHITE, 0.5f);

        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");
        auto mat = RESOURCES.createMaterial(texShader);
        mat->setTexture("tex", m_noiseMap);

        auto planeGO = getGameObject()->getScene()->createGameObject();
        planeGO->addComponent<Components::MeshRenderer>(Core::Assets::MeshGenerator::CreatePlane(1), mat);
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        static F64 yaw = 0;
        yaw += 45.0 * delta.value;
        auto fw = Math::Quat::FromEulerAngles({ 0, (F32)yaw, 0}).getForward();
        //material->setVec4("dir", Math::Vec4(fw.x, -fw.y, fw.z, 0));

        static F32 lastScale = 0.0f;
        if (KEYBOARD.isKeyDown(Key::Up))
            scale += delta.value * speed;
        if (KEYBOARD.isKeyDown(Key::Down))
            scale -= delta.value * speed;

        if (lastScale != scale)
        {
            lastScale = scale;
            NoiseMap noiseMap(size, size, scale);
            for (U32 x = 0; x < m_noiseMap->getWidth(); x++)
                for (U32 y = 0; y < m_noiseMap->getHeight(); y++)
                {
                    F32 perlinValue = noiseMap.getValue(x, y);
                    F32 col = perlinValue * 255.0f;
                    m_noiseMap->setPixel(x, y, Color::Lerp(Color::BLACK, Color::WHITE, perlinValue));
                }
            m_noiseMap->apply(true, true);
        }

        //Math::Vec3 start{0,15,0};
        //DEBUG.drawLine(start, start + fw * 10, Color::RED, 0);

        // Calculate which chunks to generate
        //Components::Camera* main = SCENE.getMainCamera();
    }

    //----------------------------------------------------------------------
    void _SetupShaderAndMaterial()
    {
        auto shader = RESOURCES.createShader( "ChunkShader", "/shaders/chunkVS.hlsl", "/shaders/chunkPS.hlsl" );

        auto tex  = ASSETS.getTexture2D( "/textures/blocks/dirt.png" );
        auto tex2 = ASSETS.getTexture2D( "/textures/blocks/brick.png" );
        auto tex3 = ASSETS.getTexture2D( "/textures/blocks/stone.png" );

        auto texArr = RESOURCES.createTexture2DArray( tex->getWidth(), tex->getHeight(), NUM_MATERIALS, Graphics::TextureFormat::RGBA32, false );
        texArr->setPixels( 0, tex );
        texArr->setPixels( 1, tex2 );
        texArr->setPixels( 2, tex3 );
        texArr->setAnisoLevel( 1 );
        texArr->setFilter( Graphics::TextureFilter::Point );
        texArr->apply();

        m_chunkMaterial = RESOURCES.createMaterial( shader );
        m_chunkMaterial->setColor( "color", Color::WHITE );
        m_chunkMaterial->setVec4( "dir", Math::Vec4( 0, -1, 1, 0 ) );
        m_chunkMaterial->setFloat( "intensity", 1.0f );
        m_chunkMaterial->setTexture( "texArray", texArr );
    }

    //----------------------------------------------------------------------
    MeshPtr _BuildMeshForRendering(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& polyvoxMesh)
    {
        auto chunk = RESOURCES.createMesh();

        ArrayList<Math::Vec3>   vertices;
        ArrayList<Math::Vec3>   normals;
        ArrayList<Math::Vec2>   materialID;
        ArrayList<Color>        colors;
        for ( auto& vertex : polyvoxMesh.getVertices() )
        {
            vertices.emplace_back( vertex.getPosition().getX(), vertex.getPosition().getY(), vertex.getPosition().getZ() );
            normals.emplace_back( vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ() );

            // Subtract minus 1, so the material range starts at 0 (cause 0 meant, there is no block)
            U8 material = static_cast<U8>( vertex.getMaterial() + 0.5 ) - 1;
            materialID.emplace_back( material );

            colors.emplace_back( Math::Random::Color() );
        }

        chunk->setVertices( vertices );
        chunk->setIndices( polyvoxMesh.getIndices() );
        chunk->setColors( colors );
        chunk->setNormals( normals );
        chunk->setUVs( materialID );

        return chunk;
    }
};
