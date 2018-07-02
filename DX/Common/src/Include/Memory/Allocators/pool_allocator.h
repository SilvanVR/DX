#pragma once

/**********************************************************************
    class: PoolAllocator (pool_allocator.hpp)

    author: S. Hau
    date: October 4, 2017

    A custom allocator preallocates a fixed amount of memory beforehand
    to AVOID dynamic memory allocation from the OS, which is in
    general very costly. See below for a class description.
**********************************************************************/

#include "iallocator.h"
#include "Logging/logging.h"

namespace Memory {

    //----------------------------------------------------------------------
    // Defines
    //----------------------------------------------------------------------

    #define POOL_ALLOCATOR_DEFAULT_ALIGNMENT 16

    //**********************************************************************
    // Features:
    //  [+] The allocated memory is divided into equally sized blocks
    //  [+] Deallocate/Allocate in any order
    //  [+] Fragmentation free
    //  [-] Only memory blocks of size less/equal the blocksize can be allocated
    //  [-] No array allocations, only single blocks
    // Be careful about pointers pointing to memory in this allocator :-)
    //**********************************************************************
    class PoolAllocator : public _IAllocator, public _IParentAllocator
    {
        //----------------------------------------------------------------------
        // Represents one allocatable chunk. If the chunk is not allocated it
        // contains a pointer to the next free chunk.
        //----------------------------------------------------------------------
        union PoolChunk
        {
            PoolChunk*  nextFreeChunk;

            PoolChunk() {};
            ~PoolChunk() {};
        };

    public:
        //----------------------------------------------------------------------
        // @Params:
        // "bytesPerChunk": Bytes per chunk. Minimum is sizeof(Size) Bytes.
        // "amountOfChunks": Maximum number of chunks allocatable
        // "parentAllocator": Parent allocator from which this allocator pulls
        //                    his memory out
        //----------------------------------------------------------------------
        explicit PoolAllocator(Size bytesPerChunk, Size amountOfChunks, _IParentAllocator* parentAllocator = nullptr);
        ~PoolAllocator();

        //----------------------------------------------------------------------
        // Allocate specified amount of bytes.
        // @Params:
        // "amountOfBytes": Amount of bytes to allocate
        // "alignment":     Alignment to use. MUST be power of two.
        //----------------------------------------------------------------------
        void* allocateRaw(Size amountOfBytes, Size alignment = 1) override;

        //----------------------------------------------------------------------
        // Deallocate the given memory. Does not call any destructor.
        // @Params:
        // "mem": The memory previously allocated from this allocator.
        //----------------------------------------------------------------------
        void deallocate(void* data) override;

        //----------------------------------------------------------------------
        // Allocates and constructs a new object of type T in this PoolAllocator.
        // @Params:
        // "args": Constructor arguments from the class T
        //----------------------------------------------------------------------
        template<typename T, typename... Args>
        T* allocate(Args&&... args);

        //----------------------------------------------------------------------
        // Deallocates and deconstructs the given object in this PoolAllocator.
        // @Params:
        // "data": The object previously allocated from this pool.
        //----------------------------------------------------------------------
        template<typename T, typename T2 = std::enable_if<!std::is_trivially_destructible<T>::value>::type>
        void deallocate(T* data);

        //----------------------------------------------------------------------
        inline Size getChunkSize() const { return m_bytesPerChunk; }

    private:
        Byte*       m_head = nullptr; // Points to next free chunk

        Size        m_amountOfChunks;
        Size        m_bytesPerChunk;

        void _PrintChunks();
        void _WarnBadAlignment(Size misalignment, Size actualAlignment);

        PoolAllocator(const PoolAllocator& other)               = delete;
        PoolAllocator& operator = (const PoolAllocator& other)  = delete;
        PoolAllocator(PoolAllocator&& other)                    = delete;
        PoolAllocator& operator = (PoolAllocator&& other)       = delete;
    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    //----------------------------------------------------------------------
    template <typename T, typename... Args>
    T* PoolAllocator::allocate( Args&&... args )
    {
        void* location = allocateRaw( sizeof(T), alignof(T) );

        // Call constructor using placement new
        T* retVal = new (location) T( std::forward<Args>( args )... );

        return retVal;
    }

    //----------------------------------------------------------------------
    template <typename T, typename T2>
    void PoolAllocator::deallocate( T* data )
    {
        data->~T();
        deallocate( reinterpret_cast<void*>(data) );
    }


} // end namespaces