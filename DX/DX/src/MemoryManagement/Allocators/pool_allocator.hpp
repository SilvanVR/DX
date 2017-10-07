#pragma once

/**********************************************************************
    class: PoolAllocator (pool_allocator.hpp)

    author: S. Hau
    date: October 4, 2017

    PoolAllocator interface. A pool-allocator preallocates a
    fixed amount of memory beforehand to AVOID dynamic
    memory allocation from the OS, which is in general very costly.
    Features:
    - The allocated memory is divided into equally sized blocks
    - Only memory blocks of those size can be allocated
    - This allows to deallocate arbitrarily
    - Fragmentation free
    Be careful about pointers pointing to memory in this allocator :-)
    @Considerations
      - PoolAllocator based on size, not on class
        -> Build upon that a FreeList allocator 
**********************************************************************/

#include "iallocator.hpp"

namespace MemoryManagement
{

    template <class T>
    class PoolAllocator : public IAllocator
    {
        //----------------------------------------------------------------------
        // Represents one allocatable chunk. If the chunk is not allocated it
        // contains a pointer to the next free chunk.
        //----------------------------------------------------------------------
        template <class T>
        union PoolChunk
        {
            T               value;
            PoolChunk<T>*   nextFreeChunk;

            PoolChunk() {};
            ~PoolChunk() {};
        };

    public:
        //----------------------------------------------------------------------
        // @Params:
        // "amountOfChunks": Maximum number of chunks allocatable
        // "parentAllocator": Parent allocator from which this allocator pulls
        //                    his memory out
        //----------------------------------------------------------------------
        explicit PoolAllocator(Size amountOfChunks, IAllocator* parentAllocator = nullptr);
        ~PoolAllocator();

        //----------------------------------------------------------------------
        // Allocates and constructs a new object of type T in this PoolAllocator.
        // @Params:
        // "args": Constructor arguments from the class T
        //----------------------------------------------------------------------
        template<class... Args>
        T* allocate(Args&&... args);

        //----------------------------------------------------------------------
        // Deallocates and deconstructs the given object in this PoolAllocator.
        // @Params:
        // "data": The object previously allocated from this pool.
        //----------------------------------------------------------------------
        void deallocate(T* data);

    private:
        PoolChunk<T>*   m_data = nullptr;
        PoolChunk<T>*   m_head = nullptr;

        Size            m_amountOfChunks = 0;
        Size            m_amountOfAllocatedChunks = 0;

        bool _InMemoryRange(void* data){ return (data >= m_data) && (data < (m_data + m_amountOfChunks)); }

        PoolAllocator (const PoolAllocator& other)              = delete;
        PoolAllocator& operator = (const PoolAllocator& other)  = delete;
        PoolAllocator (PoolAllocator&& other)                   = delete;
        PoolAllocator& operator = (PoolAllocator&& other)       = delete;
    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    //----------------------------------------------------------------------
    template<class T>
    PoolAllocator<T>::PoolAllocator(Size amountOfChunks, IAllocator* parentAllocator)
        : IAllocator( parentAllocator ), m_amountOfChunks( amountOfChunks )
    {
        ASSERT( m_amountOfChunks > 0 );

        m_data = reinterpret_cast<PoolChunk<T>*>( m_parentAllocator->allocateRaw( m_amountOfChunks * sizeof(T) ) );
        m_head = m_data;

        // Each block should point to its following block, except the last one
        for (Size i = 0; i < (m_amountOfChunks - 1); i++)
        {
            m_data[i].nextFreeChunk = std::addressof( m_data[i + 1] );
        }
        m_data[m_amountOfChunks - 1].nextFreeChunk = nullptr;
    }

    //----------------------------------------------------------------------
    template<class T>
    PoolAllocator<T>::~PoolAllocator()
    {
        if (m_amountOfAllocatedChunks != 0)
            ASSERT( false && "Not all chunks were deallocated!" );

        m_parentAllocator->deallocate( m_data );

        m_data = nullptr;
        m_head = nullptr;
    }

    //----------------------------------------------------------------------
    template<class T>
    template<class... Args>
    T* PoolAllocator<T>::allocate(Args&&... args)
    {
        if (m_head == nullptr)
        {
            _OutOfMemory();
            return nullptr;
        }

        PoolChunk<T>* newChunk = m_head;
        m_head = m_head->nextFreeChunk;

        // Create a new object of type T using placement new and forward the constructor arguments
        T* objectLocation = std::addressof( newChunk->value );
        T* retVal = new (objectLocation) T( std::forward<Args>(args)... );

        m_amountOfAllocatedChunks++;

        return retVal;
    }

    //----------------------------------------------------------------------
    template<class T>
    void PoolAllocator<T>::deallocate(T* data)
    {
        // Check if given data was really allocated from this allocator
        PoolChunk<T>* chunkToDelete = reinterpret_cast<PoolChunk<T>*>( data );
        ASSERT( _InMemoryRange(chunkToDelete) );

        // Check if given chunk was already deallocated
        // The way this works is to check if the "nextFreeChunk" of the chunkToDelete points in
        // the valid memory range within this allocator
        ASSERT( !_InMemoryRange(chunkToDelete->nextFreeChunk) );

        data->~T();
        chunkToDelete->nextFreeChunk = m_head;
        m_head = chunkToDelete;

        m_amountOfAllocatedChunks--;
    }



}