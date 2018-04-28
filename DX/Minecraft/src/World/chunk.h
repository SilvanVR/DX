#pragma once
#include "block.hpp"
#include "PolyVoxCore/LargeVolume.h"
#include "world_constants.h"

//**********************************************************************
class Chunk
{
public:
    GameObject*                     go;
    Math::Vec2Int                   position;
    Math::AABB                      bounds;
    PolyVox::LargeVolume<Block>*    volume;

    Chunk(PolyVox::LargeVolume<Block>* vol, const Math::Vec2Int& tilePos)
        : volume( vol ), position( tilePos * CHUNK_SIZE )
    {
        Math::Vec3 posV3( (F32)position.x, -CHUNK_HEIGHT, (F32)position.y );

        bounds.getMin() = posV3;
        bounds.getMax() = bounds.getMin() + Math::Vec3( CHUNK_SIZE, 2 * CHUNK_HEIGHT, CHUNK_SIZE );

        go = SCENE.createGameObject("CHUNK");
        go->getTransform()->position = posV3;
        go->addComponent<Components::MeshRenderer>();
    }

    void setActive(bool b) const { go->setActive( b ); }
    void setVoxelAt(I32 x, I32 y, I32 z, Block block) { volume->setVoxelAt(position.x + x, y, position.y + z, block); }
    void setVoxelAt(const Math::Vec3& v, Block block) { setVoxelAt((I32)v.x, (I32)v.y, (I32)v.z, block); }

    void drawBoundingBox()
    {
        auto bounds = go->getComponent<Components::MeshRenderer>()->getMesh()->getBounds();
        bounds.getMin() += Math::Vec3(position.x, -CHUNK_HEIGHT, position.y);
        bounds.getMax() += Math::Vec3(position.x, -CHUNK_HEIGHT, position.y);
        DEBUG.drawCube(bounds, Color::GREEN, 10000, true);
    }
};

using ChunkPtr = std::shared_ptr<Chunk>;