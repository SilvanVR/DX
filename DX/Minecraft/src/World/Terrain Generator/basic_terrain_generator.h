#pragma once

#include "terrain_generator.h"
#include "noise_map.h"

struct TerrainType
{
    String  name;
    F32     height;
    Block   block;
};

class BasicTerrainGenerator : public TerrainGenerator
{
    NoiseMapParams m_noiseParams = {
        50.0f, 2.0f, 0.3f, 4
    };

    F32 m_terrainHeight = 32.0f;
    ArrayList<TerrainType>  m_regions;
    I32 m_seed;

public:
    BasicTerrainGenerator(I32 seed) : m_seed(seed)
    {
        //m_regions.push_back(TerrainType{ "Water", 0.3f, Color(0x4286f4), BlockType::Water });
        //m_regions.push_back(TerrainType{ "WaterShallow", 0.4f, Color(0x82cdff), BlockType::Water });
        m_regions.push_back(TerrainType{ "Sand", 0.3f, Block("sand") });
        m_regions.push_back(TerrainType{ "Gravel", 0.35f, Block("gravel") });
        m_regions.push_back(TerrainType{ "Grass", 0.7f, Block("grass") });
        m_regions.push_back(TerrainType{ "Stone", 0.8f, Block("stone") });
        m_regions.push_back(TerrainType{ "Snow", 1.0f, Block("snow") });
    }

    void generateTerrainFor(Chunk& chunk) override
    {
        //DEBUG.drawCube(chunk->bounds, Color::GREEN, 20000);

        // Noise must be larger, so it fills the boundary chunks aswell, otherwise the mesh will contain holes
        NoiseMap noiseMap( m_seed, m_noiseParams );
        for (I32 x = 0; x < CHUNK_SIZE + 1; x++)
        {
            for (I32 z = 0; z < CHUNK_SIZE + 1; z++)
            {
                F32 noiseValue = noiseMap.get(x, z, chunk.position);
                F32 curHeight = noiseValue * m_terrainHeight;

                for (I32 y = -CHUNK_HEIGHT; y < CHUNK_HEIGHT; y++)
                {
                    if (y < curHeight)
                    {
                        Block block = _GetBlockFromHeight(noiseValue);
                        chunk.setVoxelAt( x, y, z, block );
                    }
                }
            }
        }
    }

private:
    //----------------------------------------------------------------------
    Block _GetBlockFromHeight(F32 y)
    {
        for (auto region : m_regions)
            if (y <= region.height)
                return region.block;

        ASSERT(false);
        return Block("air");
    }

    #define BLOCK_OAK_LEAVES Block("oak_leaves")
    #define BLOCK_OAK           Block("oak")

    void makeOakTree(Chunk& chunk, I32 x, I32 y, I32 z)
    {
        I32 h = Math::Random::Int(4, 7);
        I32 leafSize = 2;
        I32 newY = h + y;

        for (I32 x = x - leafSize; x < x + leafSize; ++x)
            for (I32 z = z - leafSize; z < z + leafSize; ++z) 
                chunk.setVoxelAt(x, newY, z, BLOCK_OAK_LEAVES);

        for (I32 x = x - leafSize; x < x + leafSize; ++x)
            for (I32 z = z - leafSize; z < z + leafSize; ++z)
                chunk.setVoxelAt(x, newY - 1, z, BLOCK_OAK_LEAVES);

        for (I32 zLeaf = -leafSize + 1; zLeaf <= leafSize - 1; zLeaf++)
            chunk.setVoxelAt(x, newY + 1, z + zLeaf, BLOCK_OAK_LEAVES);

        for (I32 xLeaf = -leafSize + 1; xLeaf <= leafSize - 1; xLeaf++)
            chunk.setVoxelAt(x + xLeaf, newY + 1, z, BLOCK_OAK_LEAVES);

        for (I32 y = y; y < y + h; y++)
            chunk.setVoxelAt(x, y, z, BLOCK_OAK);
    }

    //----------------------------------------------------------------------
    void makePalmTree(Chunk& chunk, I32 x, I32 y, I32 z)
    {
        I32 h = Math::Random::Int(7, 9);
        I32 diameter = Math::Random::Int(4, 6);

        for (I32 xLeaf = -diameter; xLeaf < diameter; xLeaf++)
            chunk.setVoxelAt(xLeaf + x, y + h, z, BLOCK_OAK_LEAVES);

        for (I32 zLeaf = -diameter; zLeaf < diameter; zLeaf++)
            chunk.setVoxelAt(x, y + h, zLeaf + z, BLOCK_OAK_LEAVES);

        chunk.setVoxelAt(x, y + h - 1, z + diameter, BLOCK_OAK_LEAVES);
        chunk.setVoxelAt(x, y + h - 1, z - diameter, BLOCK_OAK_LEAVES);
        chunk.setVoxelAt(x + diameter, y + h - 1, z, BLOCK_OAK_LEAVES);
        chunk.setVoxelAt(x - diameter, y + h - 1, z, BLOCK_OAK_LEAVES);
        chunk.setVoxelAt(x, y + h + 1, z, BLOCK_OAK_LEAVES);

        for (I32 y = y; y < y + h; y++)
            chunk.setVoxelAt(x, y, z, BLOCK_OAK);
    }

};