#pragma once

#include "terrain_generator.h"
#include "perlin_noise.h"

#define BLOCK_OAK_LEAVES    Block("oak_leaves")
#define BLOCK_OAK           Block("oak")
#define BLOCK_BIRCH_LEAVES  Block("birch_leaves")
#define BLOCK_BIRCH         Block("birch")
#define BLOCK_GRASS         Block("grass")

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
    Biome(I32 seed, NoiseParams params, F32 height) : m_seed(seed), m_noiseParams(params), m_height(height) {}

    //virtual Block getBlock(I32 x, I32 y, I32 z, const Math::Vec2Int& pos) = 0;
    //virtual F32 getHeight(I32 x, I32 z, const Math::Vec2Int& pos) = 0;
    virtual void generateTerrainFor(Chunk& chunk, I32 x, I32 z) = 0;

protected:
    I32                     m_seed;
    F32                     m_height;
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
        m_regions.push_back(TerrainType{ 0.15f, Block("gravel") });
        m_regions.push_back(TerrainType{ 0.65f, Block("dirt") });
        m_regions.push_back(TerrainType{ 0.8f, Block("stone") });
        m_regions.push_back(TerrainType{ 1.0f, Block("snow") });
    }

    void generateTerrainFor(Chunk& chunk, I32 x, I32 z) override
    {
        PerlinNoise noiseMap(m_seed, m_noiseParams);
        F32 noiseValue = noiseMap.get(x, z, chunk.position);
        I32 curHeight = I32(noiseValue * m_height);

        for (I32 y = -CHUNK_HEIGHT; y < CHUNK_HEIGHT; y++)
        {
            Block block = _GetBlockFromHeight(noiseValue);

            // Placing trees at chunk edges does not work properly, so just dont add them there :)
            bool notAtEdge = x > 2 && x < CHUNK_SIZE - 2 && z > 2 && z < CHUNK_SIZE - 2;
            if (y == curHeight+1 && block == Block("dirt") && notAtEdge )
            {
                if (Math::Random::Int(0, 30) == 0)
                {
                    if (Math::Random::Int(1) == 0)
                        _MakeTree(chunk, x, y, z, BLOCK_OAK, BLOCK_OAK_LEAVES);
                    else
                        _MakeTree(chunk, x, y, z, BLOCK_BIRCH, BLOCK_BIRCH_LEAVES);
                }
            }

            if (y == curHeight)
            {
                if (block == Block("dirt")) // Replace top layer dirt by grass
                    chunk.setVoxelAt(x, y, z, Block("grass"));
                else
                    chunk.setVoxelAt(x, y, z, block);
            }
            if (y < curHeight)
            {
                chunk.setVoxelAt(x, y, z, block);
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

    void generateTerrainFor(Chunk& chunk, I32 x, I32 z) override
    {
        PerlinNoise noiseMap(m_seed, m_noiseParams);
        F32 noiseValue = noiseMap.get(x, z, chunk.position);
        I32 curHeight = I32(noiseValue * m_height);

        for (I32 y = -CHUNK_HEIGHT; y < CHUNK_HEIGHT; y++)
        {
            // Placing cactus at chunk edges does not work properly, so just dont add them there :)
            bool notAtEdge = x > 1 && x < CHUNK_SIZE - 1 && z > 1 && z < CHUNK_SIZE - 1;
            if (y == curHeight && notAtEdge && y > 10.0f)
            {
                if (Math::Random::Int(0, 500) == 0)
                {
                    I32 cactusHeight = Math::Random::Int(3,5);
                    for (I32 cacY = y; cacY < y + cactusHeight; cacY++)
                        chunk.setVoxelAt(x, cacY, z, Block("cactus"));
                }
            }

            if (y < curHeight)
            {
                Block block = _GetBlockFromHeight(noiseValue);
                chunk.setVoxelAt(x, y, z, block);
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
        m_regions.push_back(TerrainType{ 0.35f, Block("dirt") });
        m_regions.push_back(TerrainType{ 0.7f, Block("stone") });
        m_regions.push_back(TerrainType{ 1.0f, Block("snow") });
    }

    void generateTerrainFor(Chunk& chunk, I32 x, I32 z) override
    {
        PerlinNoise noiseMap(m_seed, m_noiseParams);
        F32 noiseValue = noiseMap.get(x, z, chunk.position);
        I32 curHeight = I32(noiseValue * m_height);

        for (I32 y = -CHUNK_HEIGHT; y < CHUNK_HEIGHT; y++)
        {
            if (y < curHeight)
            {
                Block block = _GetBlockFromHeight(noiseValue);
                chunk.setVoxelAt(x, y, z, block);
            }
        }
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
        m_biomes.push_back(std::make_shared<ForestBiome>(m_seed, forestParams, terrain["Forest"]["height"]));

        NoiseParams desertParams;
        desertParams.scale       = terrain["Desert"]["scale"];
        desertParams.lacunarity  = terrain["Desert"]["lacunarity"];
        desertParams.gain        = terrain["Desert"]["gain"];
        desertParams.octaves     = terrain["Desert"]["octaves"];
        m_biomes.push_back(std::make_shared<DesertBiome>(m_seed, desertParams, terrain["Desert"]["height"]));

        NoiseParams hillParams;
        hillParams.scale      = terrain["Hills"]["scale"];
        hillParams.lacunarity = terrain["Hills"]["lacunarity"];
        hillParams.gain       = terrain["Hills"]["gain"];
        hillParams.octaves    = terrain["Hills"]["octaves"];
        m_biomes.push_back(std::make_shared<HillsBiome>(m_seed, hillParams, terrain["Hills"]["height"]));
    }

    void generateTerrainFor(Chunk& chunk) override
    {
        //DEBUG.drawCube(chunk.bounds, Color::GREEN, 20000);

        // Noise must be larger, so it fills the boundary blocks aswell, otherwise the mesh will contain holes
        PerlinNoise noiseMap( m_seed, m_biomeParams );
        for (I32 x = -1; x < CHUNK_SIZE + 1; x++)
        {
            for (I32 z = -1; z < CHUNK_SIZE + 1; z++)
            {
                auto& biome = _GetBiome( x, z, chunk.position );
                biome.generateTerrainFor(chunk, x, z);

                //F32 noiseValue = noiseMap.get(x, z, chunk.position);
                //I32 curHeight = I32(noiseValue * m_terrainHeight);

                //for (I32 y = -CHUNK_HEIGHT; y < CHUNK_HEIGHT; y++)
                //{
                //    Block block = _GetBlockFromHeight(noiseValue);

                //    // Placing trees at chunk edges does not work properly, so just dont add them there :)
                //    bool notAtEdge = x > 2 && x < CHUNK_SIZE - 2 && z > 2 && z < CHUNK_SIZE - 2;
                //    if (y == curHeight+1 && block == BLOCK_GRASS && notAtEdge )
                //    {
                //        if (Math::Random::Int(0,30) == 0)
                //            _MakeOakTree(chunk, x, y, z);
                //    }

                //    if (y == curHeight)
                //    {
                //        if (block == Block("dirt"))
                //            chunk.setVoxelAt(x, y, z, Block("grass"));
                //        else
                //            chunk.setVoxelAt(x, y, z, block);
                //    }
                //    else if (y < curHeight)
                //    {
                //        chunk.setVoxelAt(x, y, z, block);
                //    }
                //}
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