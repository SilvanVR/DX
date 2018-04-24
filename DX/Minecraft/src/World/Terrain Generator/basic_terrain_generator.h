#pragma once

#include "terrain_generator.h"
#include "noise_map.h"

#define BLOCK_OAK_LEAVES    Block("oak_leaves")
#define BLOCK_OAK           Block("oak")
#define BLOCK_GRASS         Block("grass")

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
    BasicTerrainGenerator(I32 seed, NoiseMapParams params, F32 height) : m_seed(seed), m_noiseParams(params), m_terrainHeight(height)
    {
        //m_regions.push_back(TerrainType{ "Water", 0.3f, Color(0x4286f4), BlockType::Water });
        m_regions.push_back(TerrainType{ "Sand", 0.1f, Block("sand") });
        m_regions.push_back(TerrainType{ "Gravel", 0.15f, Block("gravel") });
        m_regions.push_back(TerrainType{ "Grass", 0.65f, Block("grass") });
        m_regions.push_back(TerrainType{ "Stone", 0.8f, Block("stone") });
        m_regions.push_back(TerrainType{ "Snow", 1.0f, Block("snow") });
    }

    void generateTerrainFor(Chunk& chunk) override
    {
        //DEBUG.drawCube(chunk.bounds, Color::GREEN, 20000);

        // Noise must be larger, so it fills the boundary chunks aswell, otherwise the mesh will contain holes
        NoiseMap noiseMap( m_seed, m_noiseParams );
        for (I32 x = 0; x < CHUNK_SIZE + 1; x++)
        {
            for (I32 z = 0; z < CHUNK_SIZE + 1; z++)
            {
                F32 noiseValue = noiseMap.get(x, z, chunk.position);
                I32 curHeight = I32(noiseValue * m_terrainHeight);

                for (I32 y = -CHUNK_HEIGHT; y < CHUNK_HEIGHT; y++)
                {
                    Block block = _GetBlockFromHeight(noiseValue);

                    // Placing trees at chunk edges does not work properly, so just dont add them there :)
                    bool notAtEdge = x > 2 && x < CHUNK_SIZE - 2 && z > 2 && z < CHUNK_SIZE - 2;
                    if (y == curHeight && block == BLOCK_GRASS && notAtEdge )
                    {
                        if (Math::Random::Int(0,30) == 0)
                            _MakeOakTree(chunk, x, y, z);
                    }

                    if (y < curHeight)
                    {
                        if (block == Block("grass"))
                        {
                            if (y == (curHeight - 1)) // Place grass only on the top layer
                                chunk.setVoxelAt(x, y, z, block);
                            else
                                chunk.setVoxelAt(x, y, z, Block("dirt"));
                        }
                        else
                        {
                            chunk.setVoxelAt( x, y, z, block );
                        }
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

        return Block("air");
    }

    //----------------------------------------------------------------------
    void _MakeOakTree(Chunk& chunk, I32 x, I32 y, I32 z)
    {
        I32 h = Math::Random::Int(4, 7);
        I32 leafSize = 2;
        I32 newY = h + y;

        _Fill(chunk, newY, x - leafSize, x + leafSize, z - leafSize, z + leafSize, BLOCK_OAK_LEAVES);
        _Fill(chunk, newY - 1, x - leafSize, x + leafSize, z - leafSize, z + leafSize, BLOCK_OAK_LEAVES);

        for (I32 zLeaf = -leafSize + 1; zLeaf <= leafSize - 1; zLeaf++)
            chunk.setVoxelAt(x, newY + 1, z + zLeaf, BLOCK_OAK_LEAVES);

        for (I32 xLeaf = -leafSize + 1; xLeaf <= leafSize - 1; xLeaf++)
            chunk.setVoxelAt(x + xLeaf, newY + 1, z, BLOCK_OAK_LEAVES);

        for (I32 yy = y; yy < y + h; yy++)
            chunk.setVoxelAt(x, yy, z, BLOCK_OAK);
    }

    //----------------------------------------------------------------------
    void _Fill(Chunk& chunk, I32 y, I32 xBegin, I32 xEnd, I32 zBegin, I32 zEnd, Block block)
    {
        for (I32 x = xBegin; x <= xEnd; ++x)
            for (I32 z = zBegin; z <= zEnd; ++z)
                chunk.setVoxelAt(x, y, z, block);
    }
};