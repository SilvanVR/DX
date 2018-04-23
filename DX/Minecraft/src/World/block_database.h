#pragma once

#include <DX.h>
#include "block.hpp"

struct BlockInfo
{
    Math::Vec2  indices;
    Block       block;
    OS::Path    topBottom;
    OS::Path    side;

    BlockInfo() = default;
    BlockInfo(Block block, const OS::Path& path) : block(block), topBottom(path), side(path) {}
    BlockInfo(Block block, const OS::Path& topBottom, const OS::Path& side) : block(block), topBottom(topBottom), side(side) {}
};

class BlockDatabase
{
    static HashMap<Block, BlockInfo> blockInfos;

public:
    static const BlockInfo& GetBlockInfo(Block block) { return blockInfos[block]; }
    static void AddBlockInfo(const BlockInfo& blockInfo) { blockInfos[blockInfo.block] = blockInfo; }
};