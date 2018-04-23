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


public:
    BasicTerrainGenerator()
    {
        //m_regions.push_back(TerrainType{ "Water", 0.3f, Color(0x4286f4), BlockType::Water });
        //m_regions.push_back(TerrainType{ "WaterShallow", 0.4f, Color(0x82cdff), BlockType::Water });
        m_regions.push_back(TerrainType{ "Sand", 0.3f, Block::Sand });
        m_regions.push_back(TerrainType{ "Gravel", 0.35f, Block::Gravel });
        m_regions.push_back(TerrainType{ "Grass", 0.7f, Block::Grass });
        m_regions.push_back(TerrainType{ "Stone", 0.9f, Block::Stone });
        m_regions.push_back(TerrainType{ "Snow", 1.0f, Block::Snow });
    }

    void generateTerrainFor(Chunk& chunk) override
    {
        //DEBUG.drawCube(chunk->bounds, Color::GREEN, 20000);

        // Noise map must be larger, so it fills the boundary chunks aswell, otherwise the mesh will contain holes
        NoiseMap noiseMap( CHUNK_SIZE + 1, CHUNK_SIZE + 1, m_noiseParams, chunk.position );
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
        return Block::Air;
    }
};