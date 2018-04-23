#pragma once
/**********************************************************************
    class: Block (block.h)

    author: S. Hau
    date: April 20, 2018
**********************************************************************/

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
        Snow,
        Grass,
        Oak,
        OakLeaves
    };

    TBlock() : m_uMaterial(Block::Air) {}
    TBlock(Type blockType) : m_uMaterial((BlockType)blockType) { }
    TBlock(BlockType blockType) : m_uMaterial(blockType) { }

    bool operator==(const TBlock& rhs) const { return (m_uMaterial == rhs.m_uMaterial); };
    bool operator!=(const TBlock& rhs) const { return !(*this == rhs); }
    bool operator<(const TBlock& rhs) const { return m_uMaterial < rhs.m_uMaterial; }
    bool operator>(const TBlock& rhs) const { return m_uMaterial > rhs.m_uMaterial; }

    BlockType getMaterial() const { return m_uMaterial; }
    void setMaterial(BlockType uMaterial) { m_uMaterial = uMaterial; }

    String toString() const {
        switch (m_uMaterial)
        {
        case Air: return "Air";
        case Sand: return "Sand";
        case Gravel: return "Gravel";
        case Dirt: return "Dirt";
        case Stone: return "Stone";
        case Snow: return "Snow";
        case Grass: return "Grass";
        case Oak: return "Oak";
        case OakLeaves: return "OakLeaves";
        }
        return "UNKNOWN";
    }

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