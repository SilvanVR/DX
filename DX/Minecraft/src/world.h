#pragma once
/**********************************************************************
    class: World (world.h)

    author: S. Hau
    date: April 20, 2018

    Represents the 3d voxel-world. Because PolyVox allows only accessing
    the LargeVolume on one thread every request must be buffered...
**********************************************************************/
#include <DX.h>
#include "PolyVoxCore/LargeVolume.h"
#include "PolyVoxCore/Raycast.h"
#include "Physics/ray.h"
#include "block.h"
#include <list>

#define CHUNK_SIZE      16
#define CHUNK_HEIGHT    64
#define BLOCK_SIZE      1

inline Math::Vec3               ConvertVector(const PolyVox::Vector3DFloat& v) { return Math::Vec3(v.getX(), v.getY(), v.getZ()); }
inline Math::Vec3               ConvertVector(const PolyVox::Vector3DInt32& v) { return Math::Vec3((F32)v.getX(), (F32)v.getY(), (F32)v.getZ()); }
inline PolyVox::Vector3DFloat   ConvertVector(const Math::Vec3& v) { return { v.x, v.y, v.z }; }

namespace std {
    // For using the vector as a key in std::map
    template <>
    struct hash<Math::Vec2Int>
    {
        std::size_t operator()(const Math::Vec2Int& v) const
        {
            using std::size_t;
            using std::hash;
            using std::string;

            return ((hash<I32>()(v.x) ^ (hash<I32>()(v.y) << 1)) >> 1);
        }
    };
}

//----------------------------------------------------------------------
struct ChunkRayCastResult
{
    Math::Vec3  hitPoint    = Math::Vec3(0, 0, 0);
    Math::Vec3  blockCenter = Math::Vec3(0, 0, 0);
    Block       block       = Block::Air;
};

typedef std::function<void(const ChunkRayCastResult&)> RaycastCallback;

//**********************************************************************
class Chunk
{
public:
    GameObject*     go;
    Math::Vec2Int   position;
    Math::AABB      bounds;

    Chunk(const Math::Vec2Int& tilePos)
        : position( tilePos * CHUNK_SIZE )
    {
        Math::Vec3 posV3( (F32)position.x, -CHUNK_HEIGHT, (F32)position.y );

        bounds.getMin() = posV3;
        bounds.getMax() = bounds.getMin() + Math::Vec3( CHUNK_SIZE, 2 * CHUNK_HEIGHT, CHUNK_SIZE );

        go = SCENE.createGameObject("CHUNK");
        go->getTransform()->position = posV3;
        go->addComponent<Components::MeshRenderer>();
    }

    void setActive(bool b) const { go->setActive( b ); }
};

using ChunkPtr = std::shared_ptr<Chunk>;
typedef std::function<void(PolyVox::LargeVolume<Block>&, const ChunkPtr&)> ChunkCallback;

//**********************************************************************
class World
{
public:
    static I32          CHUNK_VIEW_DISTANCE;
    static MaterialPtr  CHUNK_MATERIAL;

    World();
    static World& Get() { static World world; return world; }

    // Requests a raycast being casted into the world. The callback will be called when a block was hit.
    void RayCast(const Physics::Ray& ray, const RaycastCallback& cb) { m_raycastRequestQueue.emplace(ray, cb); }

    // Sets a voxel and regenerates the corresponding chunk(s).
    void SetVoxelAt(I32 x, I32 y, I32 z, Block block) { m_blockUpdates.emplace_back(x, y, z, block); }
    void SetVoxelAt(const Math::Vec3& v, Block block) { m_blockUpdates.emplace_back((I32)v.x, (I32)v.y, (I32)v.z, block); }

    // Whenever the viewer reaches a point where a new chunk has to be generated,
    // this callback will be called and should initialize the data e.g. from a noise map.
    // Be aware that this function is called on another thread.
    void setChunkCallback(const ChunkCallback& cb) { m_chunkCallback = cb; }

private:
    PolyVox::LargeVolume<Block>                 m_volData;              // The voxel volume
    std::unordered_map<Math::Vec2Int, ChunkPtr> m_terrainChunks;        // Stores the generated terrain chunks
    std::list<ChunkPtr>                         m_chunkGenerationList;  // Contains chunks which should be generated for the first time

    // This list is similar to above, but is 1. prioritized e.g. gets executed before the list above AND 2. gets executed in a batch
    // This is required for destroying edge blocks, so several chunks have to be regenerated and replaced at the SAME TIME.
    std::list<ChunkPtr>                         m_chunkUpdateBatchList; 

    //----------------------------------------------------------------------
    struct RayCastRequest
    {
        Physics::Ray    ray;
        RaycastCallback callback;

        RayCastRequest(const Physics::Ray& ray, const RaycastCallback& cb) : ray(ray), callback(cb) {}
    };
    std::queue<RayCastRequest> m_raycastRequestQueue;  // Contains raycast requests

    //----------------------------------------------------------------------
    struct ChunkUpdateComplete
    {
        ChunkPtr chunk;
        MeshPtr  mesh;
    };
    std::list<ChunkUpdateComplete> m_chunkUpdateCompleteList; // Stores the resulting mesh and the chunk to update

    //----------------------------------------------------------------------
    struct BlockUpdate
    {
        PolyVox::Vector3DInt32  position;
        Block                   block;
        BlockUpdate(I32 x, I32 y, I32 z, Block block) : position(x, y, z), block(block) {}
    };
    std::vector<BlockUpdate> m_blockUpdates; // Stores single block updates

    // True whenever a chunk is currently generating (cause the LargeVolume can be only used by one thread at a time)
    bool m_generating = false;

    // Will be called whenever a new chunk should be filled with data
    ChunkCallback m_chunkCallback;

    friend class WorldGeneration;
    void update(F32 delta);
    void shutdown();

    // Create a mesh which contains the given region
    MeshPtr _GenerateMesh(const Math::AABB& region);
    bool    _RayCast(const Physics::Ray& ray, ChunkRayCastResult* result);
    void    _UpdateChunkInBatch(const Math::Vec2Int& coords);


    inline void _ExecuteBlockUpdates();
    inline void _CalculateChunkVisibility();
    inline void _PerformRayCasts();
    inline void _ExecuteChunkBatchUpdates();
    inline void _ExecuteChunkUpdates();
    inline void _ApplyChunkUpdates();
};