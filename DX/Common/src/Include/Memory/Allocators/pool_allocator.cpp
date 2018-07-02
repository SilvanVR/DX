#include "pool_allocator.h"
/**********************************************************************
    class: PoolAllocator

    author: S. Hau
    date: October 4, 2017
**********************************************************************/

namespace Memory
{

    //----------------------------------------------------------------------
    PoolAllocator::PoolAllocator( Size bytesPerChunk, Size amountOfChunks, _IParentAllocator* parentAllocator )
        : _IAllocator( bytesPerChunk * amountOfChunks, parentAllocator ), 
          m_bytesPerChunk( bytesPerChunk ), m_amountOfChunks( amountOfChunks )
    {
        ASSERT( m_amountOfChunks > 0 && m_bytesPerChunk >= sizeof(Size) );

        m_data = reinterpret_cast<Byte*>( m_parentAllocator->allocateRaw( m_amountOfBytes, POOL_ALLOCATOR_DEFAULT_ALIGNMENT ) );
        ASSERT( m_data != nullptr );

        m_head = m_data;

        // Each block should point to its following block, except the last one
        for (Size i = 0; i < (m_amountOfChunks - 1); i++)
        {
            Size currentChunkIndex = i * m_bytesPerChunk;
            Size nextChunkIndex = (i + 1) * m_bytesPerChunk;

            PoolChunk* chunk = reinterpret_cast<PoolChunk*>( m_data + currentChunkIndex );
            PoolChunk* nextChunk = reinterpret_cast<PoolChunk*>( m_data + nextChunkIndex );

            chunk->nextFreeChunk = nextChunk;
        }
        PoolChunk* lastChunk = reinterpret_cast<PoolChunk*>( m_data + ( (m_amountOfChunks - 1) * m_bytesPerChunk ) );
        lastChunk->nextFreeChunk = nullptr;
    }

    //----------------------------------------------------------------------
    PoolAllocator::~PoolAllocator()
    {
        m_head = nullptr;
    }

    //----------------------------------------------------------------------
    void* PoolAllocator::allocateRaw( Size amountOfBytes, Size alignment )
    {
        if (m_head == nullptr)
        {
            _OutOfMemory();
            return nullptr;
        }
        Byte* aligned = alignAddress( m_head, alignment );
        if ( aligned != m_head )
            _WarnBadAlignment( alignment - (aligned - m_head), alignment);

        ASSERT( amountOfBytes <= m_bytesPerChunk );

        PoolChunk* newChunk = reinterpret_cast<PoolChunk*>( m_head );

        // Set m_head pointing to the next free chunk
        PoolChunk* headChunk = newChunk;
        m_head = reinterpret_cast<Byte*>( headChunk->nextFreeChunk );

        // Zero out memory for this chunk. Used to detect if a chunk was already deallocated in deallocate()
        memset( newChunk, 0, m_bytesPerChunk );

        _LogAllocatedBytes( m_bytesPerChunk );

        return newChunk;
    }

    //----------------------------------------------------------------------
    void PoolAllocator::deallocate( void* data )
    {
        // Check if given data was really allocated from this allocator
        PoolChunk* deallocatedChunk = reinterpret_cast<PoolChunk*>( data );
        ASSERT( _InMemoryRange( deallocatedChunk ) );

        // Check if given chunk was already deallocated
        // The way this works is to check if the "nextFreeChunk" of the chunkToDelete points in
        // the valid memory range within this allocator. Theoratically a false positive can happen,
        // but the chance is so low and this here is just a small safety check, so it doesnt matter.
        ASSERT( !_InMemoryRange( deallocatedChunk->nextFreeChunk ) );

        // Deallocated chunk points now to the HEAD
        PoolChunk* headChunk = reinterpret_cast<PoolChunk*>( m_head );
        deallocatedChunk->nextFreeChunk = headChunk;

        // The deallocated chunk becomes the new HEAD
        m_head = reinterpret_cast<Byte*>( deallocatedChunk );

        _LogDeallocatedBytes( m_bytesPerChunk );
    }

    //----------------------------------------------------------------------
    void PoolAllocator::_PrintChunks()
    {
        LOG( "-------------------------------------" );
        PoolChunk* headChunk = reinterpret_cast<PoolChunk*>( m_head );
        for (Size i = 0; i < (m_amountOfChunks); i++)
        {
            Size currentChunkIndex = i * m_bytesPerChunk;
            PoolChunk* chunk = reinterpret_cast<PoolChunk*>( m_data + currentChunkIndex );

            String line = TS(i) + ": " + TS( (Size)chunk ) + " -> " + TS( (Size)chunk->nextFreeChunk );
            if (headChunk == chunk)
                line += " <-- m_head";
            LOG( line + "\n" );
        }
    }

    //----------------------------------------------------------------------
    void  PoolAllocator::_WarnBadAlignment( Size misalignment, Size actualAlignment )
    {
        LOG( "PoolAllocator: Bad alignment detected: Required Alignment is "
            "(" + TS(actualAlignment) + "), but is (" + TS(misalignment) + "). Performance may be degraded." );
    }

}