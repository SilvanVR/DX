#pragma once

/**********************************************************************
    class: PoolListAllocator (pool_list_allocator.hpp)

    author: S. Hau
    date: October 8, 2017

**********************************************************************/

#include "pool_allocator.h"

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
    template <typename T, typename... Args>
    T* PoolListAllocator::allocate( Args&&... args )
    {
        return _getNearestPool( sizeof(T) ) -> allocate<T>( args... );
    }

    //----------------------------------------------------------------------
    template <typename T>
    void PoolListAllocator::deallocate( T* data )
    {
        _getNearestPool( sizeof(T) ) -> deallocate<T, void>( data );
    }

} // end namespaces