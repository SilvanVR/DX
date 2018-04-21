#pragma once
/**********************************************************************
    class: Block (block.h)

    author: S. Hau
    date: April 20, 2018
**********************************************************************/
#include <DX.h>
#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"

//----------------------------------------------------------------------
template <typename Type>
class TBlock
{
public:
    enum BlockType : Type
    {
        Air = 0,
        Sand,
        Gravel,
        Dirt,
        Stone,
        Snow
    };

    TBlock() : m_uMaterial(Block::Air) {}
    TBlock(BlockType blockType) : m_uMaterial(blockType) { }

    bool operator==(const TBlock& rhs) const { return (m_uMaterial == rhs.m_uMaterial); };
    bool operator!=(const TBlock& rhs) const { return !(*this == rhs); }

    BlockType getMaterial() const { return m_uMaterial; }
    void setMaterial(BlockType uMaterial) { m_uMaterial = uMaterial; }

private:
    BlockType m_uMaterial;
};


using Block = TBlock<U8>;


//----------------------------------------------------------------------
template<typename Type>
class PolyVox::DefaultIsQuadNeeded< TBlock<Type> >
{
public:
    bool operator()(TBlock<Type> back, TBlock<Type> front, uint32_t& materialToUse)
    {
        if ((back.getMaterial() > 0) && (front.getMaterial() == 0))
        {
            materialToUse = static_cast<uint32_t>(back.getMaterial());
            return true;
        }
        else
        {
            return false;
        }
    }
};