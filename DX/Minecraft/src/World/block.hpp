#pragma once
/**********************************************************************
    class: Block (block.h)

    author: S. Hau
    date: April 20, 2018
**********************************************************************/

#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "block_database.h"

#define AIR_BLOCK Block("air")

//----------------------------------------------------------------------
template <typename Type>
class TBlock
{
public:
    TBlock() : m_uMaterial(0) {}
    TBlock(Type blockType) : m_uMaterial(blockType) { }
    TBlock(const String& name) : m_uMaterial( (U8)BlockDatabase::Get().getBlockInfo(name).index ) {}

    bool operator==(const TBlock& rhs) const { return (m_uMaterial == rhs.m_uMaterial); };
    bool operator!=(const TBlock& rhs) const { return !(*this == rhs); }
    bool operator<(const TBlock& rhs) const { return m_uMaterial < rhs.m_uMaterial; }
    bool operator>(const TBlock& rhs) const { return m_uMaterial > rhs.m_uMaterial; }

    Type getMaterial() const { return m_uMaterial; }
    void setMaterial(Type uMaterial) { m_uMaterial = uMaterial; }

private:
    Type m_uMaterial;
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