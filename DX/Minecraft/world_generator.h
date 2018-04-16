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
#include "PolyVoxCore/LargeVolume.h"
#include "noise_map.h"

enum class BlockType
{
    UNKNOWN = 0,
    Sand,
    Gravel,
    Dirt,
    Stone,
    Snow
};

//class Block
//{
//public:
//    Block() : m_id(0) {}
//    Block(BlockType blockType) : m_id(static_cast<U8>(blockType)) { }
//
//    U8 getID() const { return m_id; }
//
//private:
//    U8 m_id;
//};


template <typename Type>
class TBlock
{
public:
    TBlock() : m_uMaterial(0) {}
    TBlock(Type uMaterial) : m_uMaterial(uMaterial) {}
    TBlock(BlockType blockType) : m_uMaterial(static_cast<Type>(blockType)) { }

    bool operator==(const TBlock& rhs) const
    {
        return (m_uMaterial == rhs.m_uMaterial);
    };

    bool operator!=(const TBlock& rhs) const
    {
        return !(*this == rhs);
    }

    Type getMaterial() const { return m_uMaterial; }
    void setMaterial(Type uMaterial) { m_uMaterial = uMaterial; }

private:
    Type m_uMaterial;
};

typedef TBlock<uint8_t> Block;

template<typename Type>
class PolyVox::DefaultIsQuadNeeded< TBlock<Type> >
{
public:
    bool operator()(TBlock<Type> back, TBlock<Type> front, uint32_t& materialToUse)
    {
        if ((back.getMaterial() > 0) && (front.getMaterial() == 0))
        {
            materialToUse = static_cast<uint32_t>(back.getMaterial());
            return true;
        }
        else
        {
            return false;
        }
    }
};


//void createSphereInVolume(PolyVox::LargeVolume<PolyVox::Material8>& volData, float fRadius)
//{
//    //This vector hold the position of the center of the volume
//    PolyVox::Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);
//    //Vector3DFloat v3dVolCenter(0,0,0);
//
//    //This three-level for loop iterates over every voxel in the volume
//    for (int z = 0; z < volData.getDepth(); z++)
//    {
//        for (int y = 0; y < volData.getHeight(); y++)
//        {
//            for (int x = 0; x < volData.getWidth(); x++)
//            {
//                //Store our current position as a vector...
//                PolyVox::Vector3DFloat v3dCurrentPos(x, y, z);
//                //And compute how far the current position is from the center of the volume
//                float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();
//
//                uint8_t uVoxelValue = 0;
//
//                //If the current voxel is less than 'radius' units from the center then we make it solid.
//                if (fDistToCenter <= fRadius)
//                {
//                    //Our new voxel value
//                    uVoxelValue = Math::Random::Int(1, NUM_MATERIALS);
//                }
//
//                //Wrte the voxel value into the volume	
//                volData.setVoxelAt(x, y, z, uVoxelValue);
//            }
//        }
//    }
//}

struct TerrainType
{
    String  name;
    F32     height;
    Color   color;
    Block   block;
};


//**********************************************************************
class WorldGeneration : public Components::IComponent
{
    MaterialPtr                 m_chunkMaterial;
    PolyVox::LargeVolume<Block> m_volData;
    Components::MeshRenderer*   m_meshRenderer;
    GameObject*                 m_chunkGO;

    ArrayList<TerrainType> m_regions;
    MaterialPtr m_noiseMapMaterial;

    static const I32 m_noiseMapSize = 64;
    F32 m_speed = 10.0f;

    F32 m_noiseScale        = 10.0f;
    F32 m_noiseLacunarity   = 1.0f;
    F32 m_noiseGain         = 0.3f;
    I32 m_noiseOctaves      = 4;
    F32 m_terrainHeight     = 10.0f;

    static const I32 chunkSize = m_noiseMapSize;

    enum class DrawMode
    {
        NoiseMap,
        ColorMap
    } m_drawMode = DrawMode::ColorMap;

public:
    WorldGeneration() : m_volData(PolyVox::Region(PolyVox::Vector3DInt32(0, 0, 0), PolyVox::Vector3DInt32(chunkSize-1, 64, chunkSize-1))) {}

    //----------------------------------------------------------------------
    void addedToGameObject(GameObject* go) override
    {
        //m_regions.push_back(TerrainType{ "Water", 0.3f, Color(0x4286f4), BlockType::Water });
        //m_regions.push_back(TerrainType{ "WaterShallow", 0.4f, Color(0x82cdff), BlockType::Water });
        m_regions.push_back(TerrainType{ "Sand", 0.2f, Color(0xf9f39a), BlockType::Sand });
        m_regions.push_back(TerrainType{ "Gravel", 0.3f, Color(0x99ff6b), BlockType::Gravel });
        m_regions.push_back(TerrainType{ "Dirt", 0.7f, Color(0x5eaa3b), BlockType::Dirt });
        m_regions.push_back(TerrainType{ "Stone", 0.9f, Color(0x4f4a32), BlockType::Stone });
        m_regions.push_back(TerrainType{ "Snow", 1.0f, Color(0xffffff), BlockType::Snow });

        _SetupShaderAndMaterial();

        // Create gameobject and add mesh renderer component
        m_chunkGO = getGameObject()->getScene()->createGameObject();
        m_meshRenderer = m_chunkGO->addComponent<Components::MeshRenderer>(Core::Assets::MeshGenerator::CreatePlane(1, Color::RED), m_chunkMaterial);

        // VISUALIZATION OF PERLIN NOISE
        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");
        m_noiseMapMaterial = RESOURCES.createMaterial(texShader);

        auto planeGO = getGameObject()->getScene()->createGameObject();
        planeGO->addComponent<Components::MeshRenderer>(Core::Assets::MeshGenerator::CreatePlane(1, Color::GREEN), m_noiseMapMaterial);
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
            m_noiseScale += delta.value * m_speed * 10.0f;
        if (KEYBOARD.isKeyDown(Key::Down))
            m_noiseScale -= delta.value * m_speed * 10.0f;

        static F32 lastLacunarity = 0.0f;
        if (KEYBOARD.isKeyDown(Key::T))
            m_noiseLacunarity += delta.value * m_speed;
        if (KEYBOARD.isKeyDown(Key::G))
            m_noiseLacunarity -= delta.value * m_speed;

        static F32 lastGain = 0.0f;
        if (KEYBOARD.isKeyDown(Key::Z))
            m_noiseGain += delta.value * m_speed * 0.1f;
        if (KEYBOARD.isKeyDown(Key::H))
            m_noiseGain -= delta.value * m_speed * 0.1f;

        static I32 lastOctaves = 0;
        if (KEYBOARD.wasKeyPressed(Key::U))
            m_noiseOctaves++;
        if (KEYBOARD.wasKeyPressed(Key::J))
            m_noiseOctaves--;

        static DrawMode lastDrawMode;
        if (KEYBOARD.wasKeyPressed(Key::F5))
            m_drawMode = DrawMode::NoiseMap;
        if (KEYBOARD.wasKeyPressed(Key::F6))
            m_drawMode = DrawMode::ColorMap;

        static F32 lastTerrainHeight = 0.0f;
        if (KEYBOARD.isKeyDown(Key::Left))
            m_terrainHeight -= delta.value * m_speed;
        if (KEYBOARD.isKeyDown(Key::Right))
            m_terrainHeight += delta.value * m_speed;

        static bool computing = false;
        if ( (lastScale != m_noiseScale || lastLacunarity != m_noiseLacunarity || lastGain != m_noiseGain 
            || lastOctaves != m_noiseOctaves || lastDrawMode != m_drawMode || lastTerrainHeight != m_terrainHeight) && !computing)
        {
            computing = true;
            lastScale = m_noiseScale; lastLacunarity = m_noiseLacunarity; lastGain = m_noiseGain; 
            lastOctaves = m_noiseOctaves; lastDrawMode = m_drawMode; lastTerrainHeight = m_terrainHeight;

            ASYNC_JOB([&] {
                NoiseMap noiseMap(m_noiseMapSize, m_noiseMapSize, m_noiseScale, m_noiseLacunarity, m_noiseGain, m_noiseOctaves);
                switch (m_drawMode)
                {
                case DrawMode::NoiseMap: m_noiseMapMaterial->setTexture("tex", _GenerateNoiseTextureFromNoiseMap(noiseMap)); break;
                case DrawMode::ColorMap: m_noiseMapMaterial->setTexture("tex", _GenerateColorTextureFromNoiseMap(noiseMap)); break;
                }
                auto mesh = _GenerateMeshFromNoiseMap(m_volData, noiseMap, m_terrainHeight);
                m_meshRenderer->setMesh(mesh);
                m_chunkGO->getComponent<Components::Transform>()->position = { -m_volData.getWidth() / 2.0f, 5.0f, -m_volData.getDepth() / 2.0f };
                computing = false;
            });

            LOG("Scale: " + TS(m_noiseScale));
            LOG("Lacunarity: " + TS(m_noiseLacunarity));
            LOG("Gain: " + TS(m_noiseGain));
            LOG("Octaves: " + TS(m_noiseOctaves));
            LOG("Terrain Height: " + TS(m_terrainHeight));
        }

        // Calculate which chunks to generate
        //Components::Camera* main = SCENE.getMainCamera();
    }

private:
    //----------------------------------------------------------------------
    void _SetupShaderAndMaterial()
    {
        auto shader = RESOURCES.createShader( "ChunkShader", "/shaders/chunkVS.hlsl", "/shaders/chunkPS.hlsl" );

        ArrayList<Texture2DPtr> blockTextures;
        for (auto region : m_regions)
        {
            String path = "/textures/blocks/" + region.name + ".png";
            blockTextures.push_back( ASSETS.getTexture2D( path.c_str() ) );
        }

        auto texArr = RESOURCES.createTexture2DArray( blockTextures[0]->getWidth(), blockTextures[0]->getHeight(), 
                                                      blockTextures.size(), Graphics::TextureFormat::RGBA32, false );
        texArr->setAnisoLevel( 1 );
        texArr->setFilter( Graphics::TextureFilter::Point );

        for (U32 i = 0; i < blockTextures.size(); i++)
            texArr->setPixels( i, blockTextures[i] );
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

    //----------------------------------------------------------------------
    Texture2DPtr _GenerateColorTextureFromNoiseMap(const NoiseMap& noiseMap)
    {
        auto tex2D = RESOURCES.createTexture2D(noiseMap.getWidth(), noiseMap.getHeight(), Graphics::TextureFormat::BGRA32);
        for (U32 x = 0; x < tex2D->getWidth(); x++)
        {
            for (U32 y = 0; y < tex2D->getHeight(); y++)
            {
                F32 curHeight = noiseMap.getValue(x, y);
                for (auto region : m_regions)
                {
                    if (curHeight <= region.height)
                    {
                        tex2D->setPixel(x, y, region.color);
                        break;
                    }
                }
            }
        }
        tex2D->setAnisoLevel(1);
        tex2D->setFilter(Graphics::TextureFilter::Point);
        tex2D->apply();
        return tex2D;
    }

    //----------------------------------------------------------------------
    Texture2DPtr _GenerateNoiseTextureFromNoiseMap(const NoiseMap& noiseMap)
    {
        auto tex2D = RESOURCES.createTexture2D(noiseMap.getWidth(), noiseMap.getHeight(), Graphics::TextureFormat::BGRA32);
        for (U32 x = 0; x < tex2D->getWidth(); x++)
        {
            for (U32 y = 0; y < tex2D->getHeight(); y++)
            {
                F32 curHeight = noiseMap.getValue(x, y);
                tex2D->setPixel(x, y, Color::Lerp(Color::BLACK, Color::WHITE, curHeight));
            }
        }
        tex2D->setAnisoLevel(1);
        tex2D->setFilter(Graphics::TextureFilter::Point);
        tex2D->apply();

        return tex2D;
    }

    //----------------------------------------------------------------------
    MeshPtr _GenerateMeshFromNoiseMap(PolyVox::LargeVolume<Block>& volData, const NoiseMap& noiseMap, F32 maxHeight = 10.0f)
    {
        volData.flushAll();

        PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> mesh;
        PolyVox::CubicSurfaceExtractorWithNormals<PolyVox::LargeVolume<Block>> surfaceExtractor( &m_volData, m_volData.getEnclosingRegion(), &mesh );

        for (int x = 0; x < volData.getWidth(); x++)
        {
            for (int z = 0; z < volData.getDepth(); z++)
            {
                F32 noiseValue = noiseMap.getValue(x, z);
                F32 curHeight = noiseValue * maxHeight;

                for (int y = 0; y < volData.getHeight(); y++)
                {
                    if ( y < curHeight )
                    {
                        Block block = _GetBlockInRegionFromHeight( noiseValue );
                        volData.setVoxelAt( x, y, z, block );
                    }
                }
            }
        }

        surfaceExtractor.execute();

        return _BuildMeshForRendering( mesh );
    }

    //----------------------------------------------------------------------
    Block _GetBlockInRegionFromHeight(F32 y)
    {
        for (auto region : m_regions)
            if (y <= region.height)
                return region.block;

        ASSERT(false);
        return Block(BlockType::UNKNOWN);
    }
};
