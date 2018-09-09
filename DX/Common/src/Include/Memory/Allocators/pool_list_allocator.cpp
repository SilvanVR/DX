#include "pool_list_allocator.h"
/**********************************************************************
    class: PoolListAllocator

    author: S. Hau
    date: October 4, 2017
**********************************************************************/

namespace Memory
{

    //----------------------------------------------------------------------
    PoolListAllocator::PoolListAllocator( const std::vector<int>& sizesInBytes, Size amountOfChunks, _IParentAllocator* parentAllocator )
    {
        for (auto& poolSize : sizesInBytes)
        {
            PoolAllocator* newPoolAllocator = new PoolAllocator( poolSize, amountOfChunks, parentAllocator );
            m_poolAllocators.push_back( newPoolAllocator );
        }

        // Sort by chunk-size (just to be safe) :-)
        std::sort( m_poolAllocators.begin(), m_poolAllocators.end(), []( PoolAllocator* alloc1, PoolAllocator* alloc2 ) -> bool {
            return alloc1->getChunkSize() < alloc2->getChunkSize();
        } );
    }

    //----------------------------------------------------------------------
    PoolListAllocator::~PoolListAllocator()
    {
        for (auto& poolAllocator : m_poolAllocators)
            delete poolAllocator;
    }

    //----------------------------------------------------------------------
    void* PoolListAllocator::allocateRaw( Size amountOfBytes, Size alignment )
    {
        return _getNearestPool( amountOfBytes ) -> allocateRaw( amountOfBytes, alignment );
    }

    //----------------------------------------------------------------------
    void PoolListAllocator::deallocate( void* data )
    {
        // Have no information about the data, that way check every pool allocator
        for (auto& poolAllocator : m_poolAllocators)
        {
            if (poolAllocator->_InMemoryRange( data ))
            {
                poolAllocator->deallocate( data );
                return;
            }
        }
        ASSERT (false && "Given Memory is not from this allocator!");
    }

    //----------------------------------------------------------------------
    PoolAllocator* PoolListAllocator::_getNearestPool( Size amtOfBytes )
    {
        for (Size i = 0; i < m_poolAllocators.size(); i++)
        {
            Size chunkSize = m_poolAllocators[i]->getChunkSize();

            bool hasEnoughSpace = (amtOfBytes <= chunkSize);
            if (hasEnoughSpace)
            {
                return m_poolAllocators[i];
                break;
            }
        }

        Size largestChunkSize = m_poolAllocators.back()->getChunkSize();
        ASSERT( false && "Not a single pool is large enough for the requested amount of bytes!" );
        return nullptr;
    }

}