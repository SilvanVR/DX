#include "universal_allocator_defragmented.h"
/**********************************************************************
    class: UniversalAllocatorDefragmented

    author: S. Hau
    date: October 5, 2017
**********************************************************************/

#include "universal_allocator.h"
#include "Logging/logging.h"

namespace Memory {

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    //----------------------------------------------------------------------
    UniversalAllocatorDefragmented::UniversalAllocatorDefragmented( Size amountOfBytes, Size _HandleTableSize, _IParentAllocator* parentAllocator )
        : m_universalAllocator( amountOfBytes, parentAllocator ), 
          m_handleTable( _HandleTableSize, m_universalAllocator.getParentAllocator() ) // Order important!
    {
        m_usedChunks.reserve( INITIAL_USED_CHUNK_LIST_CAPACITY );
    }

    //----------------------------------------------------------------------
    void UniversalAllocatorDefragmented::defragment()
    {
        while ( canBeDefragmented() )
        {
            defragmentOnce();
        }
    }

    //----------------------------------------------------------------------
    bool UniversalAllocatorDefragmented::defragmentOnce()
    {
        if ( not canBeDefragmented() )
            return false;

        // Get the first chunk, which address is where we will move to.
        UniversalAllocator::FreeChunk& freeChunk = m_universalAllocator.m_freeChunks[0];

        // Find first UsedChunk right from the first FreeChunk
        UsedChunk* chunkToRelocate = nullptr;
        for (UsedChunk& chunk : m_usedChunks)
        {
            if (freeChunk.m_address < chunk.getAddress())
            {
                chunkToRelocate = &chunk;
                break;
            }
        }

        // Move the chunk to the new position
        chunkToRelocate->relocate( freeChunk.m_address );

        // Update FreeChunk information
        Byte* newFreeChunkAddress = ( chunkToRelocate->getAddress() + chunkToRelocate->getSizeInBytes() );
        freeChunk.m_address = newFreeChunkAddress;

        Size amountOfBytes = _UVAllocatorGetAmountOfBytes( chunkToRelocate->getAddress() );
        Byte newOffset = _UVAllocatorGetOffset( chunkToRelocate->getAddress() );
        freeChunk.m_sizeInBytes = (amountOfBytes + newOffset);

        // If the updated chunk touches now the following one, merge them
        if (m_universalAllocator.m_freeChunks.size() > 1)
        {
            UniversalAllocator::FreeChunk& nextFreeChunk = m_universalAllocator.m_freeChunks[1];
            if (freeChunk.touches( nextFreeChunk ))
            {
                freeChunk.m_sizeInBytes += nextFreeChunk.m_sizeInBytes;
                m_universalAllocator._RemoveFreeChunk( nextFreeChunk );
            }
        }

        return true;
    }

    //----------------------------------------------------------------------
    UAPtr<Byte> UniversalAllocatorDefragmented::allocateRaw( Size amountOfBytes, Size alignment )
    {
        void* mem = m_universalAllocator.allocateRaw( amountOfBytes, alignment );

        // Add entry in _HandleTable
        Size nextFreeHandle = m_handleTable.nextFreeHandle();
        m_handleTable[nextFreeHandle] = mem;

        _AddUsedChunk( nextFreeHandle, amountOfBytes, static_cast<Byte*>(mem) );

        return UAPtr<Byte>( &m_handleTable, nextFreeHandle );
    }

    //----------------------------------------------------------------------
    void UniversalAllocatorDefragmented::deallocate( UAPtr<Byte>& data )
    {
        ASSERT( data.isValid() && "Given data was already deallocated or were never allocated." );

        m_universalAllocator.deallocate( data.getRaw() );

        _RemoveUsedChunk( data._GetHandle() );

        m_handleTable.freeHandle( data._GetHandle() );

        data.invalidate();
    }

    //----------------------------------------------------------------------
    bool UniversalAllocatorDefragmented::canBeDefragmented()
    {
        bool hasOnlyOneFreeChunk = (m_universalAllocator.m_freeChunks.size() == 1);

        bool isFreeChunkAtEnd = false;
        if (hasOnlyOneFreeChunk)
        {
            UniversalAllocator::FreeChunk& freeChunk = m_universalAllocator.m_freeChunks[0];

            Byte* freeChunkEndAddress = (freeChunk.m_address + freeChunk.m_sizeInBytes);
            Byte* uaEndAddress = (m_universalAllocator.m_data + m_universalAllocator.m_amountOfBytes);

            isFreeChunkAtEnd = (freeChunkEndAddress == uaEndAddress);
        }

        bool freeChunkAvailable = (m_universalAllocator.m_freeChunks.size() != 0);
        if (not freeChunkAvailable || isFreeChunkAtEnd)
        {
            return false;
        }

        return true;
    }

    //----------------------------------------------------------------------
    void UniversalAllocatorDefragmented::_RemoveUsedChunk( Size handle )
    {
        UsedChunk chunkToRemove( handle, &m_handleTable );
        m_usedChunks.erase( std::remove( m_usedChunks.begin(), m_usedChunks.end(), chunkToRemove ), m_usedChunks.end() );
    }

    //**********************************************************************
    // HandleTable
    //**********************************************************************

    //----------------------------------------------------------------------
    _HandleTable::_HandleTable( Size sizeOfTable, _IParentAllocator* allocator )
        : m_allocator( allocator ), m_amtOfHandles( sizeOfTable + 1 )
    {
        m_table = static_cast<Handle*>( m_allocator->allocateRaw( m_amtOfHandles * sizeof(void*), alignof(void*) ) );
        m_head = std::addressof( m_table[m_amtOfHandles - 1] );

        for (Size i = (m_amtOfHandles - 1); i > 0; i--)
        {
            m_table[i].nextHandle = std::addressof( m_table[i - 1] );
        }
        m_table[1].nextHandle = nullptr;
    }

    //----------------------------------------------------------------------
    _HandleTable::~_HandleTable()
    {
        m_allocator->deallocate(m_table);
    }

    //----------------------------------------------------------------------
    Size _HandleTable::nextFreeHandle()
    {
        ASSERT( m_head != nullptr && "Out of Handles!" );

        Size nextFreeHandle = (m_head - m_table);
        m_head = m_head->nextHandle;

        return nextFreeHandle;
    }

    //----------------------------------------------------------------------
    void _HandleTable::freeHandle( Size handle )
    {
        ASSERT( handle > 0 && handle <= m_amtOfHandles );

        Handle* newHandle = std::addressof( m_table[handle] );
        newHandle->nextHandle = m_head;
        m_head = newHandle;
    }

    //----------------------------------------------------------------------
    void _HandleTable::_PrintTable()
    {
        LOG("");
        for (Size i = 0; i < m_amtOfHandles; i++)
        {
            String line = TS(i) + ": " + TS( (Size)(&m_table[i]) ) + " -> " + TS( (Size)m_table[i].nextHandle );
            if ( m_head == (&m_table[i]) )
                line += " <-- m_head";
            LOG(line + "\n");
        }
    }

} // end namespaces