#include "block_database.h"

I32 BlockDatabase::blockIndex = 0;

BlockDatabase::BlockDatabase()
{
    blockInfos["air"]           = { blockIndex++, "/textures/blocks/dirt.png" };
    blockInfos["dirt"]          = { blockIndex++, "/textures/blocks/dirt.png" };
    blockInfos["sand"]          = { blockIndex++, "/textures/blocks/sand.png"};
    blockInfos["gravel"]        = { blockIndex++, "/textures/blocks/gravel.png"};
    blockInfos["stone"]         = { blockIndex++, "/textures/blocks/stone.png"};
    blockInfos["snow"]          = { blockIndex++, "/textures/blocks/snow.png"};
    blockInfos["grass"]         = { blockIndex++, "/textures/blocks/grass_side.png", "/textures/blocks/grass_top.png" };

    blockInfos["oak"]           = { blockIndex++, "/textures/blocks/log_oak.png", "/textures/blocks/log_oak_top.png" };
    blockInfos["oak_leaves"]    = { blockIndex++, "/textures/blocks/leaves_oak.png" };

    blockInfos["birch"]         = { blockIndex++, "/textures/blocks/log_birch.png", "/textures/blocks/log_birch_top.png" };
    blockInfos["birch_leaves"]  = { blockIndex++, "/textures/blocks/leaves_birch.png" };
}