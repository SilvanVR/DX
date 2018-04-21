#include "world.h"
#include "noise_map.h"

#define CHUNK_COORD(x,y) Math::Vec2Int(static_cast<I32>(std::floorf((F32)(x) / CHUNK_SIZE)), static_cast<I32>(std::floorf((F32)(y) / CHUNK_SIZE)))

MeshPtr CreateMeshForRendering(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& polyvoxMesh);

//----------------------------------------------------------------------
I32         World::CHUNK_VIEW_DISTANCE = 4;
MaterialPtr World::CHUNK_MATERIAL = nullptr;

//----------------------------------------------------------------------
World::World() : m_volData( PolyVox::Region( PolyVox::Vector3DInt32( INT_MIN, INT_MIN, INT_MIN ),
                                             PolyVox::Vector3DInt32( INT_MAX, INT_MAX, INT_MAX ) ) )
{}

//----------------------------------------------------------------------
void World::update( F32 delta )
{
    if ( not m_blockUpdates.empty() && not m_generating )
    {
        for (auto& blockUpdate : m_blockUpdates)
        {
            m_volData.setVoxelAt( blockUpdate.position, blockUpdate.block );

            // Update corresponding chunk and possibly neighbour chunks
            auto chunkCoord = CHUNK_COORD( blockUpdate.position.getX(), blockUpdate.position.getZ() );

            auto chunkCoordRight    = CHUNK_COORD( blockUpdate.position.getX() + 1, blockUpdate.position.getZ() );
            auto chunkCoordLeft     = CHUNK_COORD( blockUpdate.position.getX() - 1, blockUpdate.position.getZ() );
            auto chunkCoordForward  = CHUNK_COORD( blockUpdate.position.getX(), blockUpdate.position.getZ() + 1 );
            auto chunkCoordBack     = CHUNK_COORD( blockUpdate.position.getX(), blockUpdate.position.getZ() - 1 );

            // Check if neighbour chunk has to be regenerated aswell
            if ( chunkCoord != chunkCoordRight )
                _AddChunk( chunkCoordRight );
            else if (chunkCoord != chunkCoordLeft)
                _AddChunk( chunkCoordLeft );

            if (chunkCoord != chunkCoordForward)
                _AddChunk( chunkCoordForward );
            else if (chunkCoord != chunkCoordBack)
                _AddChunk( chunkCoordBack );

            _AddChunk( chunkCoord );
        }
        m_blockUpdates.clear();
    }

    // Disable all chunks
    for (auto& pair : m_terrainChunks)
        pair.second->setActive( false );

    // Calculate which chunks are visible
    Components::Camera* viewer = SCENE.getMainCamera();
    auto transform = viewer->getComponent<Components::Transform>();

    I32 currentChunkCoordX = static_cast<I32>( std::floorf( transform->position.x / CHUNK_SIZE ) );
    I32 currentChunkCoordY = static_cast<I32>( std::floorf( transform->position.z / CHUNK_SIZE) );

    for (I32 ring = 0; ring < CHUNK_VIEW_DISTANCE; ring++)
    {
        for (I32 x = -ring; x <= ring; x++)
        {
            for (I32 y = -ring; y <= ring; y++)
            {
                if ( std::abs(x) == ring || std::abs(y) == ring)
                {
                    Math::Vec2Int chunkCoords( currentChunkCoordX + x, currentChunkCoordY + y );

                    if ( m_terrainChunks.find(chunkCoords) != m_terrainChunks.end() )
                    {
                        // Chunk already exists, so just enable it
                        m_terrainChunks[chunkCoords]->setActive( true );

                    }
                    else
                    {
                        _CreateChunk( chunkCoords );
                    }
                }
            }
        }
    }

    // Do raycast
    if ( not m_raycastRequestQueue.empty() && not m_generating )
    {
        while (not m_raycastRequestQueue.empty())
        {
            auto& req = m_raycastRequestQueue.front();

            ChunkRayCastResult result;
            _RayCast(req.ray, &result);
            req.callback(result);

            m_raycastRequestQueue.pop();
        }
    }

    // Update/Generate new chunk if requested and not currently generating one
    if ( not m_chunkUpdateList.empty() && not m_generating )
    {
        m_generating = true;
        auto nextChunk = m_chunkUpdateList.front();

        ASYNC_JOB([=] {
            if ( not nextChunk->generated )
            {
                m_chunkCallback( m_volData, nextChunk );
                nextChunk->generated = true;
            }
            auto mesh = _GenerateMesh( nextChunk->bounds );
            m_chunkUpdateCompleteList.push_back({ nextChunk, mesh });
            m_generating = false;
        });

        m_chunkUpdateList.pop_front();
    }

    // Update chunk with newly generated data
    for (auto& chunkGen : m_chunkUpdateCompleteList)
    {
        auto mr = chunkGen.chunk->go->getComponent<Components::MeshRenderer>();
        mr->setMesh( chunkGen.mesh );
        mr->setMaterial( CHUNK_MATERIAL );
    }
    m_chunkUpdateCompleteList.clear();
}

//**********************************************************************
// PUBLIC
//**********************************************************************

//**********************************************************************
// PRIVATE
//**********************************************************************

//----------------------------------------------------------------------
void World::_AddChunk( const Math::Vec2Int& coords )
{
    if ( m_terrainChunks.find( coords ) == m_terrainChunks.end() )
        ASSERT(false);

    // Queue chunk for generating if not already in for it
    auto chunk = m_terrainChunks[coords];
    if ( std::find(m_chunkUpdateList.begin(), m_chunkUpdateList.end(), chunk) == m_chunkUpdateList.end() )
    m_chunkUpdateList.emplace_front( chunk );
}

//----------------------------------------------------------------------
void World::_CreateChunk( const Math::Vec2Int& coords )
{
    auto newChunk = std::make_shared<Chunk>( coords );
    m_terrainChunks[coords] = newChunk;

    // Queue chunk for generating
    m_chunkUpdateList.emplace_back( newChunk );
}

//----------------------------------------------------------------------
bool World::_RayCast( const Physics::Ray& ray, ChunkRayCastResult* result )
{
    auto cb = [&]( const PolyVox::LargeVolume<Block>::Sampler& sampler ) -> bool
    {
        auto voxel = sampler.getVoxel();
        if ( voxel == Block::Air )
            return true; // Continues raycast

        result->block = voxel;
        result->blockCenter = ConvertVector( sampler.getPosition() );

        // Since polyvox raycast does not deliever the exact hit-point but rather the block, it must be calculated separately
        Math::Vec3 blockSize( BLOCK_SIZE * 0.5f );
        Math::AABB blockBounds( result->blockCenter - blockSize, result->blockCenter + blockSize );

        Physics::RayCastResult rayResult;
        if ( ray.intersects( blockBounds, &rayResult ) )
            result->hitPoint = rayResult.hitPoint;

        return false; // Stops raycast
    };

    PolyVox::Vector3DFloat start( ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z );
    PolyVox::Vector3DFloat dir( ray.getDirection().x, ray.getDirection().y, ray.getDirection().z );

    auto polyVoxResult = PolyVox::raycastWithDirection( &m_volData, start, dir, cb );
    return (polyVoxResult == PolyVox::RaycastResult::Interupted);
}

//----------------------------------------------------------------------
MeshPtr World::_GenerateMesh( const Math::AABB& region )
{
    PolyVox::Region chunkDim( PolyVox::Vector3DInt32( (I32)region.getMin().x, (I32)region.getMin().y, (I32)region.getMin().z ),
                              PolyVox::Vector3DInt32( (I32)region.getMax().x, (I32)region.getMax().y, (I32)region.getMax().z ) );

    PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> mesh;
    PolyVox::CubicSurfaceExtractorWithNormals<PolyVox::LargeVolume<Block>> surfaceExtractor( &m_volData, chunkDim, &mesh );
    surfaceExtractor.execute();

   return CreateMeshForRendering( mesh );
}

//----------------------------------------------------------------------
MeshPtr CreateMeshForRendering( const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& polyvoxMesh )
{
    auto chunk = RESOURCES.createMesh();

    ArrayList<Math::Vec3>   vertices;
    ArrayList<Math::Vec3>   normals;
    ArrayList<Math::Vec2>   materials;
    for ( auto& vertex : polyvoxMesh.getVertices() )
    {
        vertices.emplace_back( vertex.getPosition().getX(), vertex.getPosition().getY(), vertex.getPosition().getZ() );
        normals.emplace_back( vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ() );

        // Subtract minus 1, so the material range starts at 0 (cause 0 meant, there is no block)
        // @TODO: GET MATERIAL INDICES FROM A DATABASE
        U8 material = static_cast<U8>( vertex.getMaterial() + 0.5 ) - 1;
        materials.emplace_back(Math::Vec2(material, material));
    }

    chunk->setVertices( vertices );
    chunk->setIndices( polyvoxMesh.getIndices() );
    chunk->setNormals( normals );
    chunk->setUVs( materials );

    return chunk;
}
