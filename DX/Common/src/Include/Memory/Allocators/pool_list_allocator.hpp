#pragma once

/**********************************************************************
    class: PoolListAllocator (pool_list_allocator.hpp)

    author: S. Hau
    date: October 8, 2017

**********************************************************************/

#include "pool_allocator.hpp"

namespace Memory {

    //**********************************************************************
    // A PoolListAllocator manages several PoolAllocators of different sizes.
    // Allocations will be fetched from a PoolAllocator closest to the size
    // of the allocation.
    // Features:
    //  [+] No Fragmentation
    //  [+] Optimal memory usage
    //  [-] No array allocation, only single blocks of data
    //**********************************************************************
    class PoolListAllocator
    {
    public:
        //----------------------------------------------------------------------
        // Creates a PoolListAllocator along 1 - N PoolAllocators.
        // @Params:
        // "sizesInBytes": The size for each pool allocator in ascending order.
        //                 Minimum size is sizeof(Size) Bytes.
        // "amountOfChunks": Amount of chunks to allocate for each Pool.
        // "parentAllocator": The ParentAllocator from which all PoolAllocators
        //                    will get their memory from.
        //----------------------------------------------------------------------
        explicit PoolListAllocator(const std::vector<int>& sizesInBytes,
                                   Size amountOfChunks,
                                   _IParentAllocator* parentAllocator = nullptr);
        ~PoolListAllocator();

        //----------------------------------------------------------------------
        // Allocate specified amount of bytes. If possible use typed allocation,
        // because the typed deallocation is a bit faster than raw deallocation.
        // @Params:
        // "amountOfBytes": Amount of bytes to allocate
        // "alignment":     Alignment to use. MUST be power of two.
        //----------------------------------------------------------------------
        void* allocateRaw(Size amountOfBytes, Size alignment = 1);

        //----------------------------------------------------------------------
        // Allocate an object of type T.
        // @Params:
        // "args": Constructor arguments from the class T
        //----------------------------------------------------------------------
        template <typename T, typename... Args>
        T* allocate(Args&&... args);

        //----------------------------------------------------------------------
        // Deallocate the given memory. Does not call any destructor.
        // @Params:
        // "mem": The memory previously allocated from this allocator.
        //----------------------------------------------------------------------
        void deallocate(void* data);

        //----------------------------------------------------------------------
        // Deallocates and deconstructs the given object.
        // @Params:
        // "data": The object previously allocated from this allocator.
        //----------------------------------------------------------------------
        template <typename T>
        void deallocate(T* data);

    private:
        // Contains all PoolAllocators sorted by chunk-size in ascending order.
        std::vector<PoolAllocator*> m_poolAllocators;

        // Return the nearest PoolAllocator for a given amount of bytes
        PoolAllocator* _getNearestPool(Size amtOfBytes);

        PoolListAllocator(const PoolListAllocator& other)               = delete;
        PoolListAllocator& operator = (const PoolListAllocator& other)  = delete;
        PoolListAllocator(PoolListAllocator&& other)                    = delete;
        PoolListAllocator& operator = (PoolListAllocator&& other)       = delete;
    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

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
    template <typename T, typename... Args>
    T* PoolListAllocator::allocate( Args&&... args )
    {
        return _getNearestPool( sizeof(T) ) -> allocate<T>( args... );
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
    template <typename T>
    void PoolListAllocator::deallocate( T* data )
    {
        _getNearestPool( sizeof(T) ) -> deallocate<T, void>( data );
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


} // end namespaces