#pragma once

#include "terrain_generator.h"
#include "perlin_noise.h"

#define BLOCK_OAK_LEAVES    Block("oak_leaves")
#define BLOCK_OAK           Block("oak")
#define BLOCK_BIRCH_LEAVES  Block("birch_leaves")
#define BLOCK_BIRCH         Block("birch")
#define BLOCK_GRASS         Block("grass")

#define BIOME_TRANSITION_WIDTH 3


//----------------------------------------------------------------------
void _Fill(Chunk& chunk, I32 y, I32 xBegin, I32 xEnd, I32 zBegin, I32 zEnd, Block block)
{
    for (I32 x = xBegin; x <= xEnd; ++x)
        for (I32 z = zBegin; z <= zEnd; ++z)
            chunk.setVoxelAt(x, y, z, block);
}
//----------------------------------------------------------------------
void _MakeTree(Chunk& chunk, I32 x, I32 y, I32 z, Block log, Block leaves)
{
    I32 h = Math::Random::Int(4, 7);
    I32 leafSize = 2;
    I32 newY = h + y;

    _Fill(chunk, newY, x - leafSize, x + leafSize, z - leafSize, z + leafSize, leaves);
    _Fill(chunk, newY - 1, x - leafSize, x + leafSize, z - leafSize, z + leafSize, leaves);

    for (I32 zLeaf = -leafSize + 1; zLeaf <= leafSize - 1; zLeaf++)
        chunk.setVoxelAt(x, newY + 1, z + zLeaf, leaves);

    for (I32 xLeaf = -leafSize + 1; xLeaf <= leafSize - 1; xLeaf++)
        chunk.setVoxelAt(x + xLeaf, newY + 1, z, leaves);

    for (I32 yy = y; yy < y + h; yy++)
        chunk.setVoxelAt(x, yy, z, log);
}

struct TerrainType
{
    F32     height;
    Block   block;
};

//**********************************************************************
class Biome
{
public:
    Biome(I32 seed, NoiseParams params, F32 elevation) : m_seed(seed), m_noiseParams(params), m_elevation(elevation) {}

    virtual void generateTerrainFor(Chunk& chunk, I32 x, I32 height, I32 z) = 0;

    I32 getHeight(I32 x, I32 z, const Math::Vec2Int& pos)
    {
        PerlinNoise noiseMap(m_seed, m_noiseParams);
        F32 noiseValue = noiseMap.get(x, z, pos);
        return I32(noiseValue * m_elevation);
    }

protected:
    I32                     m_seed;
    F32                     m_elevation;
    NoiseParams             m_noiseParams;
    ArrayList<TerrainType>  m_regions;

    //----------------------------------------------------------------------
    Block _GetBlockFromHeight(F32 y)
    {
        for (auto region : m_regions)
            if (y <= region.height)
                return region.block;

        ASSERT(false);
        return Block("air");
    }
};

//**********************************************************************
class ForestBiome : public Biome
{
public:
    ForestBiome(I32 seed, NoiseParams params, F32 height) : Biome(seed, params, height)
    {
        m_regions.push_back(TerrainType{ 0.1f, Block("sand") });
        m_regions.push_back(TerrainType{ 0.12f, Block("gravel") });
        m_regions.push_back(TerrainType{ 1.0f, Block("dirt") });
    }

    void generateTerrainFor(Chunk& chunk, I32 x, I32 height, I32 z) override
    {
        F32 noiseValue = height / m_elevation;
        Block block = _GetBlockFromHeight(noiseValue);

        for (I32 y = -CHUNK_HEIGHT; y < height; y++)
            chunk.setVoxelAt(x, y, z, block);

        // Replace top layer dirt by grass
        if (block == Block("dirt"))
            chunk.setVoxelAt(x, height, z, Block("grass"));
        else
            chunk.setVoxelAt(x, height, z, block);

        // Placing trees at chunk edges does not work properly, so just dont add them there :)
        bool notAtEdge = x > 2 && x < CHUNK_SIZE - 2 && z > 2 && z < CHUNK_SIZE - 2;
        if (block == Block("dirt") && notAtEdge)
        {
            if (Math::Random::Int(0, 30) == 0)
            {
                if (Math::Random::Int(1) == 0)
                    _MakeTree(chunk, x, height + 1, z, BLOCK_OAK, BLOCK_OAK_LEAVES);
                else
                    _MakeTree(chunk, x, height + 1, z, BLOCK_BIRCH, BLOCK_BIRCH_LEAVES);
            }
        }
    }
};

//**********************************************************************
class DesertBiome : public Biome
{
public:
    DesertBiome(I32 seed, NoiseParams params, F32 height) : Biome(seed, params, height)
    {
        m_regions.push_back(TerrainType{ 1.0f, Block("sand") });
    }

    void generateTerrainFor(Chunk& chunk, I32 x, I32 height, I32 z) override
    {
        F32 noiseValue = height / m_elevation;
        Block block = _GetBlockFromHeight(noiseValue);
        for (I32 y = -CHUNK_HEIGHT; y < height; y++)
            chunk.setVoxelAt(x, y, z, block);

        // Placing cactus at chunk edges does not work properly, so just dont add them there :)
        bool notAtEdge = x > 1 && x < CHUNK_SIZE - 1 && z > 1 && z < CHUNK_SIZE - 1;
        if (notAtEdge && height > 10.0f)
        {
            if (Math::Random::Int(0, 500) == 0)
            {
                I32 cactusHeight = Math::Random::Int(3,5);
                for (I32 cacY = height; cacY < height + cactusHeight; cacY++)
                    chunk.setVoxelAt(x, cacY, z, Block("cactus"));
            }
        }
    }
};

//**********************************************************************
class HillsBiome : public Biome
{
public:
    HillsBiome(I32 seed, NoiseParams params, F32 height) : Biome(seed, params, height)
    {
        m_regions.push_back(TerrainType{ 0.15f, Block("gravel") });
        m_regions.push_back(TerrainType{ 0.25f, Block("dirt") });
        m_regions.push_back(TerrainType{ 0.6f, Block("stone") });
        m_regions.push_back(TerrainType{ 1.0f, Block("snow") });
    }

    void generateTerrainFor(Chunk& chunk, I32 x, I32 height, I32 z) override
    {
        F32 noiseValue = height / m_elevation;
        Block block = _GetBlockFromHeight(noiseValue);
        for (I32 y = -CHUNK_HEIGHT; y < height; y++)
            chunk.setVoxelAt(x, y, z, block);
    }
};

//**********************************************************************
class BasicTerrainGenerator : public TerrainGenerator
{
    NoiseParams m_biomeParams;
    I32 m_seed;

    F32 m_terrainHeight = 50.0f;
    ArrayList<std::shared_ptr<Biome>> m_biomes;

public:
    BasicTerrainGenerator(I32 seed) : m_seed(seed)
    {
        Core::Config::ConfigFile terrain("res/terrain.ini");
        m_biomeParams.scale       = terrain["Biomes"]["scale"];
        m_biomeParams.lacunarity  = terrain["Biomes"]["lacunarity"];
        m_biomeParams.gain        = terrain["Biomes"]["gain"];
        m_biomeParams.octaves     = terrain["Biomes"]["octaves"];

        NoiseParams forestParams;
        forestParams.scale       = terrain["Forest"]["scale"];
        forestParams.lacunarity  = terrain["Forest"]["lacunarity"];
        forestParams.gain        = terrain["Forest"]["gain"];
        forestParams.octaves     = terrain["Forest"]["octaves"];
        m_biomes.push_back(std::make_shared<ForestBiome>(m_seed, forestParams, terrain["Forest"]["elevation"]));

        NoiseParams desertParams;
        desertParams.scale       = terrain["Desert"]["scale"];
        desertParams.lacunarity  = terrain["Desert"]["lacunarity"];
        desertParams.gain        = terrain["Desert"]["gain"];
        desertParams.octaves     = terrain["Desert"]["octaves"];
        m_biomes.push_back(std::make_shared<DesertBiome>(m_seed, desertParams, terrain["Desert"]["elevation"]));

        NoiseParams hillParams;
        hillParams.scale      = terrain["Hills"]["scale"];
        hillParams.lacunarity = terrain["Hills"]["lacunarity"];
        hillParams.gain       = terrain["Hills"]["gain"];
        hillParams.octaves    = terrain["Hills"]["octaves"];
        m_biomes.push_back(std::make_shared<HillsBiome>(m_seed, hillParams, terrain["Hills"]["elevation"]));
    }

    void generateTerrainFor(Chunk& chunk) override
    {
        static const I32 NUM_SAMPLES = (BIOME_TRANSITION_WIDTH + BIOME_TRANSITION_WIDTH + 1) *
                                       (BIOME_TRANSITION_WIDTH + BIOME_TRANSITION_WIDTH + 1);
        //DEBUG.drawCube(chunk.bounds, Color::GREEN, 20000);

        // Noise must be larger, so it fills the boundary blocks aswell, otherwise the mesh will contain holes
        PerlinNoise noiseMap( m_seed, m_biomeParams );
        for (I32 x = 0; x < CHUNK_SIZE + 1; x++)
        {
            for (I32 z = 0; z < CHUNK_SIZE + 1; z++)
            {
                // Smooth transition between biomes must be calculated here
                I32 height = 0;
                for (I32 i = x - BIOME_TRANSITION_WIDTH; i < x + BIOME_TRANSITION_WIDTH; i++)
                {
                    for (I32 j = z - BIOME_TRANSITION_WIDTH; j < z + BIOME_TRANSITION_WIDTH; j++)
                    {
                        auto& biome = _GetBiome( i, j, chunk.position );
                        height += biome.getHeight( i, j, chunk.position );
                    }
                }
                height /= NUM_SAMPLES;

                auto& biome = _GetBiome( x, z, chunk.position );
                biome.generateTerrainFor( chunk, x, height, z );
            }
        }
    }

private:
    //----------------------------------------------------------------------
    Biome& _GetBiome(I32 x, I32 z, const Math::Vec2Int& pos)
    {
        PerlinNoise noiseMap( m_seed, m_biomeParams );

        F32 noiseValue = noiseMap.get(x, z, pos);
        F32 steps = 1.0f / m_biomes.size();

        // Every biome is equally likely
        I32 index = I32(noiseValue / steps);
        index = Math::clamp(index, 0, m_biomes.size() - 1);
        return *m_biomes[index].get();
    }
};