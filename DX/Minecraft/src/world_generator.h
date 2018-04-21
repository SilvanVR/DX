#pragma once
/**********************************************************************
    class: WorldGeneration (world_generator.h)

    author: S. Hau
    date: April 16, 2018

    Script which generates chunks of block data using perlin noise and
    the polyvox library to generate polygon meshes for rendering.
**********************************************************************/
#include <DX.h>
#include "noise_map.h"
#include "Math/aabb.h"
#include "world.h"

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
    ArrayList<TerrainType>      m_regions;

    MaterialPtr                 m_chunkMaterial;
    MaterialPtr                 m_noiseMapMaterial;

    F32     m_speed             = 10.0f;
    F32     m_terrainHeight     = 30.0f;

    std::unordered_map<Math::Vec2Int, ChunkPtr> m_terrainChunks;

    NoiseMapParams m_noiseParams = {
        50.0f, 2.0f, 0.3f, 4
    };

    enum class DrawMode
    {
        NoiseMap,
        ColorMap
    } m_drawMode = DrawMode::ColorMap;

public:
    WorldGeneration(I32 chunkViewDistance = 10) { World::CHUNK_VIEW_DISTANCE = chunkViewDistance; }
    ~WorldGeneration(){ World::CHUNK_MATERIAL.reset(); }

    //----------------------------------------------------------------------
    void init() override
    {
        //m_regions.push_back(TerrainType{ "Water", 0.3f, Color(0x4286f4), BlockType::Water });
        //m_regions.push_back(TerrainType{ "WaterShallow", 0.4f, Color(0x82cdff), BlockType::Water });
        m_regions.push_back(TerrainType{ "Sand", 0.2f, Color(0xf9f39a), Block::Sand });
        m_regions.push_back(TerrainType{ "Gravel", 0.3f, Color(0x99ff6b), Block::Gravel });
        m_regions.push_back(TerrainType{ "Dirt", 0.7f, Color(0x5eaa3b), Block::Dirt });
        m_regions.push_back(TerrainType{ "Stone", 0.9f, Color(0x4f4a32), Block::Stone });
        m_regions.push_back(TerrainType{ "Snow", 1.0f, Color(0xffffff), Block::Snow });

        _SetupShaderAndMaterial();

        // VISUALIZATION OF PERLIN NOISE ON A FLAT PLANE
        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");
        m_noiseMapMaterial = RESOURCES.createMaterial(texShader);

        SCENE.createGameObject()->addComponent<Components::MeshRenderer>(Core::Assets::MeshGenerator::CreatePlane(1, Color::GREEN), m_noiseMapMaterial);

        World::Get().setChunkCallback( BIND_THIS_FUNC_2_ARGS( &WorldGeneration::ChunkUpdateCallback ) );
    }

    F32 lastScale = 0.0f;
    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        //static F64 yaw = 0;
        //yaw += 45.0 * delta.value;
        //auto fw = Math::Quat::FromEulerAngles({ 0, (F32)yaw, 0}).getForward();
        //m_chunkMaterial->setVec4("dir", Math::Vec4(fw.x, -fw.y, fw.z, 0));

        if (KEYBOARD.isKeyDown(Key::Up))
            m_noiseParams.scale += (F32)delta.value * m_speed * 10.0f;
        if (KEYBOARD.isKeyDown(Key::Down))
            m_noiseParams.scale -= (F32)delta.value * m_speed * 10.0f;

        static F32 lastLacunarity = 0.0f;
        if (KEYBOARD.isKeyDown(Key::T))
            m_noiseParams.lacunarity += (F32)delta.value * m_speed;
        if (KEYBOARD.isKeyDown(Key::G))
            m_noiseParams.lacunarity -= (F32)delta.value * m_speed;

        static F32 lastGain = 0.0f;
        if (KEYBOARD.isKeyDown(Key::Z))
            m_noiseParams.gain += (F32)delta.value * m_speed * 0.1f;
        if (KEYBOARD.isKeyDown(Key::H))
            m_noiseParams.gain -= (F32)delta.value * m_speed * 0.1f;

        static I32 lastOctaves = 0;
        if (KEYBOARD.wasKeyPressed(Key::U))
            m_noiseParams.octaves++;
        if (KEYBOARD.wasKeyPressed(Key::J))
            m_noiseParams.octaves--;

        static DrawMode lastDrawMode;
        if (KEYBOARD.wasKeyPressed(Key::F5))
            m_drawMode = DrawMode::NoiseMap;
        if (KEYBOARD.wasKeyPressed(Key::F6))
            m_drawMode = DrawMode::ColorMap;

        static F32 lastTerrainHeight = 0.0f;
        if (KEYBOARD.isKeyDown(Key::Left))
            m_terrainHeight -= (F32)delta.value * m_speed;
        if (KEYBOARD.isKeyDown(Key::Right))
            m_terrainHeight += (F32)delta.value * m_speed;

        if ( (lastScale != m_noiseParams.scale || lastLacunarity != m_noiseParams.lacunarity || lastGain != m_noiseParams.gain
            || lastOctaves != m_noiseParams.octaves || lastDrawMode != m_drawMode || lastTerrainHeight != m_terrainHeight))
        {
            lastScale = m_noiseParams.scale; lastLacunarity = m_noiseParams.lacunarity; lastGain = m_noiseParams.gain;
            lastOctaves = m_noiseParams.octaves; lastDrawMode = m_drawMode; lastTerrainHeight = m_terrainHeight;

            ASYNC_JOB([&] {
                NoiseMap noiseMap(CHUNK_SIZE * 10, CHUNK_SIZE * 10, m_noiseParams);
                switch (m_drawMode)
                {
                case DrawMode::NoiseMap: m_noiseMapMaterial->setTexture("tex", _GenerateNoiseTextureFromNoiseMap(noiseMap)); break;
                case DrawMode::ColorMap: m_noiseMapMaterial->setTexture("tex", _GenerateColorTextureFromNoiseMap(noiseMap)); break;
                }
            });

            LOG("Scale: " + TS(m_noiseParams.scale));
            LOG("Lacunarity: " + TS(m_noiseParams.lacunarity));
            LOG("Gain: " + TS(m_noiseParams.gain));
            LOG("Octaves: " + TS(m_noiseParams.octaves));
            LOG("Terrain Height: " + TS(m_terrainHeight));
        }

        World::Get().update((F32)delta.value);
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
                                                      (U32)blockTextures.size(), Graphics::TextureFormat::RGBA32, false );
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

        World::CHUNK_MATERIAL = m_chunkMaterial;
    }

    //----------------------------------------------------------------------
    Texture2DPtr _GenerateColorTextureFromNoiseMap(const NoiseMap& noiseMap)
    {
        auto tex2D = RESOURCES.createTexture2D(noiseMap.getWidth(), noiseMap.getHeight(), Graphics::TextureFormat::BGRA32);
        for (U32 x = 0; x < tex2D->getWidth(); x++)
        {
            for (U32 y = 0; y < tex2D->getHeight(); y++)
            {
                F32 curHeight = noiseMap.get(x, y);
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
                F32 curHeight = noiseMap.get(x, y);
                tex2D->setPixel(x, y, Color::Lerp(Color::BLACK, Color::WHITE, curHeight));
            }
        }
        tex2D->setAnisoLevel(1);
        tex2D->setFilter(Graphics::TextureFilter::Point);
        tex2D->apply();

        return tex2D;
    }

    //----------------------------------------------------------------------
    void ChunkUpdateCallback(PolyVox::LargeVolume<Block>& volume, const ChunkPtr& chunk )
    {
        DEBUG.drawCube(chunk->bounds, Color::GREEN, 20000);

        // Noise map must be larger, so it fills the boundary chunks aswell, otherwise the mesh will contain holes
        NoiseMap noiseMap( CHUNK_SIZE + 1, CHUNK_SIZE + 1, m_noiseParams, chunk->position );
        for (I32 x = 0; x < noiseMap.getWidth(); x++)
        {
            for (I32 z = 0; z < noiseMap.getHeight(); z++)
            {
                F32 noiseValue = noiseMap.get(x, z);
                F32 curHeight = noiseValue * m_terrainHeight;

                for (I32 y = -CHUNK_HEIGHT; y < CHUNK_HEIGHT; y++)
                {
                    if (y < curHeight)
                    {
                        Block block = _GetBlockFromHeight(noiseValue);
                        volume.setVoxelAt( chunk->position.x + x, y, chunk->position.y + z, block );
                    }
                }
            }
        }
    }

    //----------------------------------------------------------------------
    Block _GetBlockFromHeight(F32 y)
    {
        for (auto region : m_regions)
            if (y <= region.height)
                return region.block;

        ASSERT(false);
        return Block::Air;
    }

};
