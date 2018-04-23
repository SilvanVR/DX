#pragma once
#include "../chunk.h"

class TerrainGenerator
{
public:
    virtual ~TerrainGenerator() = default;

    virtual void generateTerrainFor(Chunk& chunk) = 0;
};