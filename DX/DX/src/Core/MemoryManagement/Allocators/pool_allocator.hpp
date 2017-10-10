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


namespace Core { namespace MemoryManagement {

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
        template<class T, class... Args>
        T* allocate(Args&&... args);

        //----------------------------------------------------------------------
        // Deallocates and deconstructs the given object in this PoolAllocator.
        // @Params:
        // "data": The object previously allocated from this pool.
        //----------------------------------------------------------------------
        template<class T, class T2 = std::enable_if<!std::is_trivially_destructible<T>::value>::type>
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
    PoolAllocator::PoolAllocator(Size bytesPerChunk, Size amountOfChunks, _IParentAllocator* parentAllocator)
        : _IAllocator(bytesPerChunk * amountOfChunks, parentAllocator), m_bytesPerChunk(bytesPerChunk), m_amountOfChunks(amountOfChunks)
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
    void* PoolAllocator::allocateRaw(Size amountOfBytes, Size alignment)
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
    void PoolAllocator::deallocate(void* data)
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
    template<class T, class... Args>
    T* PoolAllocator::allocate(Args&&... args)
    {
        void* location = allocateRaw( sizeof(T), alignof(T) );

        // Call constructor using placement new
        T* retVal = new (location) T( std::forward<Args>( args )... );

        return retVal;
    }

    //----------------------------------------------------------------------
    template<class T, class T2>
    void PoolAllocator::deallocate(T* data)
    {
        data->~T();
        deallocate( reinterpret_cast<void*>(data) );
    }

    //----------------------------------------------------------------------
    void PoolAllocator::_PrintChunks()
    {
        std::cout << "-------------------------------------" << std::endl;
        PoolChunk* headChunk = reinterpret_cast<PoolChunk*>(m_head);
        for (Size i = 0; i < (m_amountOfChunks); i++)
        {
            Size currentChunkIndex = i * m_bytesPerChunk;
            PoolChunk* chunk = reinterpret_cast<PoolChunk*>(m_data + currentChunkIndex);

            std::cout << i << ": " << (void*)chunk << " -> " << (void*)chunk->nextFreeChunk;
            if (headChunk == chunk)
                std::cout << " <-- m_head";
            std::cout << std::endl;
        }
    }

    void  PoolAllocator::_WarnBadAlignment(Size misalignment, Size actualAlignment)
    {
        std::cout << "PoolAllocator: Bad alignment detected: Required Alignment is "
            "(" + TS(actualAlignment) + "), but is (" + TS(misalignment) + "). Performance may be degraded." << std::endl;
    }

} } // end namespaces