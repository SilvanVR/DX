#pragma once

#include <DX.h>
#include "block.hpp"

struct BlockInfo
{
    Math::Vec2  texIndices;
    OS::Path    topBottom;
    OS::Path    side;
    I32         index = 0;

    BlockInfo() = default;
    BlockInfo(I32 index, const OS::Path& path) : index(index), topBottom(path), side(path) {}
    BlockInfo(I32 index, const OS::Path& topBottom, const OS::Path& side) : index(index), topBottom(topBottom), side(side) {}
};

class BlockDatabase
{
    static I32 blockIndex;
    HashMap<String, BlockInfo> blockInfos;

public:
    BlockDatabase();

    static BlockDatabase& Get() { static BlockDatabase inst; return inst; }

    HashMap<String, BlockInfo>& getBlockInfos() { return blockInfos; }

    const BlockInfo& getBlockInfo(const String& block) 
    {
        ASSERT(blockInfos.count(block) > 0);
        return blockInfos[block]; 
    }

    const BlockInfo& getBlockInfo(I32 index)
    { 
        for (auto& pair : blockInfos)
            if (pair.second.index == (F32)index)
                return pair.second;

        ASSERT(false);
        static BlockInfo NULL_BLOCK_INFO;
        return NULL_BLOCK_INFO;
    }

    String getBlockName(I32 index)
    {
        for (auto& pair : blockInfos)
            if (pair.second.index == (F32)index)
                return pair.first;
        ASSERT(false);
        return "";
    }
};