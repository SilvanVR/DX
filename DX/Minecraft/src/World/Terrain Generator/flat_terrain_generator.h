#pragma once

#include "terrain_generator.h"

class FlatTerrainGenerator : public TerrainGenerator
{
public:
    void generateTerrainFor(Chunk& chunk) override
    {
        for (int x = 0; x < CHUNK_SIZE + 1; ++x)
            for (int z = 0; z < CHUNK_SIZE + 1; ++z) {
                chunk.setVoxelAt(x, 0, z, Block::Stone);
                chunk.setVoxelAt(x, 1, z, Block::Dirt);
                chunk.setVoxelAt(x, 2, z, Block::Dirt);
                chunk.setVoxelAt(x, 3, z, Block::Dirt);
                chunk.setVoxelAt(x, 4, z, Block::Sand);
            }
    }
};