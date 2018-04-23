#include "world.h"
#include "block_database.h"

#define CHUNK_COORD(x,y) Math::Vec2Int(static_cast<I32>(std::floorf((F32)(x) / CHUNK_SIZE)), static_cast<I32>(std::floorf((F32)(y) / CHUNK_SIZE)))

MeshPtr CreateMeshForRendering(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& polyvoxMesh);

//----------------------------------------------------------------------
I32         World::CHUNK_VIEW_DISTANCE = 4;
MaterialPtr World::CHUNK_MATERIAL = nullptr;

//----------------------------------------------------------------------
World::World() : m_volData( PolyVox::Region( PolyVox::Vector3DInt32( INT_MIN, INT_MIN, INT_MIN ),
                                             PolyVox::Vector3DInt32( INT_MAX, INT_MAX, INT_MAX ) ) )
{


}

//----------------------------------------------------------------------
void World::shutdown()
{
    m_chunkGenerationList.clear();
    m_chunkUpdateCompleteList.clear();
    m_terrainChunks.clear();
    CHUNK_MATERIAL.reset();
}

//----------------------------------------------------------------------
void World::update( F32 delta )
{
    // ORDER OF THIS FUNCTIONS IS IMPORTANT
    _ExecuteBlockUpdates();

    _CalculateChunkVisibility();

    _PerformRayCasts();

    _ExecuteChunkBatchUpdates();

    _ExecuteChunkUpdates();

    _ApplyChunkUpdates();
}

//**********************************************************************
// PUBLIC
//**********************************************************************

//**********************************************************************
// PRIVATE
//**********************************************************************

//----------------------------------------------------------------------
void World::_UpdateChunkInBatch( const Math::Vec2Int& coords )
{
    if ( m_terrainChunks.find( coords ) == m_terrainChunks.end() )
        ASSERT( false ); // This should never happen

    // Queue chunk for generating if not already in for it
    auto chunk = m_terrainChunks[coords];
    if ( std::find( m_chunkUpdateBatchList.begin(), m_chunkUpdateBatchList.end(), chunk ) == m_chunkUpdateBatchList.end() )
        m_chunkUpdateBatchList.emplace_front( chunk );
}

//----------------------------------------------------------------------
bool World::_RayCast( const Physics::Ray& ray, ChunkRayCastResult* result )
{
    auto cb = [&]( const PolyVox::LargeVolume<Block>::Sampler& sampler ) -> bool
    {
        auto voxel = sampler.getVoxel();
        if ( voxel == AIR_BLOCK )
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

    ArrayList<Math::Vec3> vertices;
    ArrayList<Math::Vec3> normals;
    ArrayList<Math::Vec2> materials;
    for ( auto& vertex : polyvoxMesh.getVertices() )
    {
        vertices.emplace_back( vertex.getPosition().getX(), vertex.getPosition().getY(), vertex.getPosition().getZ() );
        normals.emplace_back( vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ() );

        U8 material = static_cast<U8>( vertex.getMaterial() );
        materials.emplace_back( BlockDatabase::Get().getBlockInfo( material ).texIndices );
    }

    chunk->setVertices( vertices );
    chunk->setIndices( polyvoxMesh.getIndices() );
    chunk->setNormals( normals );
    chunk->setUVs( materials );

    return chunk;
}


//----------------------------------------------------------------------
void World::_ExecuteBlockUpdates()
{
    // Execute single block updates and determine which chunks were affected to regenerate them
    if ( not m_blockUpdates.empty() && not m_generating )
    {
        for (auto& blockUpdate : m_blockUpdates)
        {
            m_volData.setVoxelAt( blockUpdate.position, blockUpdate.block );

            // Queue corresponding chunk for update
            auto chunkCoord = CHUNK_COORD( blockUpdate.position.getX(), blockUpdate.position.getZ() );
            _UpdateChunkInBatch( chunkCoord );

            // Check if neighbour chunk(s) has to be regenerated aswell
            auto chunkCoordRight    = CHUNK_COORD( blockUpdate.position.getX() + 1, blockUpdate.position.getZ() );
            auto chunkCoordLeft     = CHUNK_COORD( blockUpdate.position.getX() - 1, blockUpdate.position.getZ() );
            auto chunkCoordForward  = CHUNK_COORD( blockUpdate.position.getX(), blockUpdate.position.getZ() + 1 );
            auto chunkCoordBack     = CHUNK_COORD( blockUpdate.position.getX(), blockUpdate.position.getZ() - 1 );

            if (chunkCoord != chunkCoordRight)
                _UpdateChunkInBatch( chunkCoordRight );
            else if (chunkCoord != chunkCoordLeft)
                _UpdateChunkInBatch( chunkCoordLeft );

            if (chunkCoord != chunkCoordForward)
                _UpdateChunkInBatch( chunkCoordForward );
            else if (chunkCoord != chunkCoordBack)
                _UpdateChunkInBatch( chunkCoordBack );
        }
        m_blockUpdates.clear();
    }
}

//----------------------------------------------------------------------
void World::_CalculateChunkVisibility()
{
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
                        // Create new chunk and queue it for generating
                        auto newChunk = std::make_shared<Chunk>( &m_volData, chunkCoords );
                        m_terrainChunks[chunkCoords] = newChunk;
                        m_chunkGenerationList.emplace_back( newChunk );
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------------------
void World::_PerformRayCasts()
{
    // Program rarely crashes when "m_generating" is uncommented, but otherwise raycasts can be delayed
    // which causes a stuttering for the raycast physics system right now
    if ( not m_raycastRequestQueue.empty() /* && not m_generating */ )
    {
        while ( not m_raycastRequestQueue.empty() )
        {
            auto& req = m_raycastRequestQueue.front();

            ChunkRayCastResult result;
            if ( _RayCast( req.ray, &result ) )
                req.callback( result );

            m_raycastRequestQueue.pop();
        }
    }
}

//----------------------------------------------------------------------
void World::_ExecuteChunkBatchUpdates()
{
    if ( not m_chunkUpdateBatchList.empty() && not m_generating )
    {
        m_generating = true;

        auto chunkList = m_chunkUpdateBatchList;
        ASYNC_JOB([=] {
            std::list<ChunkUpdateComplete> updateCompleteList;
            for (auto& chunk : chunkList)
            {
                auto mesh = _GenerateMesh( chunk->bounds );
                updateCompleteList.push_back( { chunk, mesh } );
            }
            m_chunkUpdateCompleteList.insert( m_chunkUpdateCompleteList.end(), updateCompleteList.begin(), updateCompleteList.end() );
            m_generating = false;
        });
        m_chunkUpdateBatchList.clear();
    }
}

//----------------------------------------------------------------------
void World::_ExecuteChunkUpdates()
{
    // Generate new chunk if requested and not currently generating one
    if ( not m_chunkGenerationList.empty() && not m_generating )
    {
        m_generating = true;

        // Can only generate one chunk here, cause if the player changes chunks, those chunks must be rebuild immediately
        auto nextChunk = m_chunkGenerationList.front();
        ASYNC_JOB([=] {
            m_chunkCallback( *nextChunk.get() );
            auto mesh = _GenerateMesh( nextChunk->bounds );
            m_chunkUpdateCompleteList.push_back({ nextChunk, mesh });
            m_generating = false;
        });

        m_chunkGenerationList.pop_front();
    }
}

//----------------------------------------------------------------------
void World::_ApplyChunkUpdates()
{
    // Update chunk with newly generated data
    for (auto& chunkGen : m_chunkUpdateCompleteList)
    {
        auto mr = chunkGen.chunk->go->getComponent<Components::MeshRenderer>();
        mr->setMesh( chunkGen.mesh );
        mr->setMaterial( CHUNK_MATERIAL );
    }
    m_chunkUpdateCompleteList.clear();
}