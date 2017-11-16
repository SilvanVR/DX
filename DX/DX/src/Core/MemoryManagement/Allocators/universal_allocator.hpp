#pragma once

/**********************************************************************
    class: UniversalAllocator (universal_allocator.hpp)

    author: S. Hau
    date: October 5, 2017

    A custom allocator preallocates a fixed amount of memory beforehand
    to AVOID dynamic memory allocation from the OS, which is in
    general very costly. See below for a class description.
**********************************************************************/

#include "iallocator.h"

namespace Core { namespace MemoryManagement {

    //----------------------------------------------------------------------
    // Defines
    //----------------------------------------------------------------------

    #define INITIAL_FREE_CHUNK_LIST_CAPACITY    32

    // Amount of bytes additionally allocated to store necessary information needed for deallocation
    #define AMOUNT_OF_BYTES_FOR_SIZE            4
    #define AMOUNT_OF_BYTES_FOR_OFFSET          1 // DO NOT change this value

    //----------------------------------------------------------------------
    // Forward Declarations
    //----------------------------------------------------------------------

    void _UVAllocatorWriteAdditionalBytes(Byte* alignedAddress, Size amtOfBytes, Byte offset);

    Byte _UVAllocatorGetOffset(Byte* alignedAddress);
    Size _UVAllocatorGetAmountOfBytes(Byte* alignedAddress);

    //**********************************************************************
    // Features:
    //  [+] Allocations can be made in any size/amounts and order
    //  [+] Deallocations can be made in any order
    //  [-] Memory will become fragmented
    //  [-] At least N-Bytes for SIZE + OFFSET has to be additionally
    //     allocated for each request. (Default: 5 Bytes)
    // Be careful about pointers pointing to memory in this allocator :-)
    //**********************************************************************
    class UniversalAllocator : public _IAllocator, public _IParentAllocator
    {
        friend class UniversalAllocatorDefragmented; // Allow this class to defragment the memory

        //**********************************************************************
        // Represents one contiguous chunk of bytes in this allocator.
        //**********************************************************************
        struct FreeChunk
        {
            Byte*   m_address;
            Size    m_sizeInBytes;

            FreeChunk(Byte* address, Size size)
                : m_address(address), m_sizeInBytes(size) {}

            // Try to allocate the given amount of bytes. Nullptr if not enough space in this chunk.
            void* allocateRaw(Size amountOfBytes, Size alignment);

            bool touches(const FreeChunk& other) const {
                return (other.m_address + other.m_sizeInBytes) == m_address || (m_address + m_sizeInBytes) == other.m_address;
            }

            bool operator <  (const FreeChunk& other) const { return m_address < other.m_address; }
            bool operator >  (const FreeChunk& other) const { return m_address > other.m_address; }
            bool operator <= (const FreeChunk& other) const { return m_address <= other.m_address; }
            bool operator >= (const FreeChunk& other) const { return m_address >= other.m_address; }
            bool operator == (const FreeChunk& other) const { return m_address == other.m_address; }
            bool operator != (const FreeChunk& other) const { return m_address != other.m_address; }
        };

    public:
        //----------------------------------------------------------------------
        // @Params:
        // "amountOfBytes": Amount of bytes to allocate.
        // "parentAllocator": Allocator to which allocate memory from.
        //----------------------------------------------------------------------
        explicit UniversalAllocator(Size amountOfBytes, _IParentAllocator* parentAllocator = nullptr);
        ~UniversalAllocator() {}

        //----------------------------------------------------------------------
        // Allocate specified amount of bytes.
        // @Params:
        // "amountOfBytes": Amount of bytes to allocate.
        // "alignment":     Alignment to use. MUST be power of two.
        //----------------------------------------------------------------------
        void* allocateRaw(Size amountOfBytes, Size alignment = 1) override;

        //----------------------------------------------------------------------
        // Allocate "amountOfObjects" objects of type T.
        // @Params:
        // "amountOfObjects": Amount of objects to allocate (array-allocation).
        // "args": Constructor arguments from the class T.
        //----------------------------------------------------------------------
        template <typename T, typename... Args>
        T* allocate(Size amountOfObjects = 1, Args&&... args);

        //----------------------------------------------------------------------
        // Deallocate the given memory. Does not call any destructor.
        // @Params:
        // "mem": The memory previously allocated from this allocator.
        //----------------------------------------------------------------------
        void deallocate(void* data) override { _Deallocate( reinterpret_cast<Byte*>( data ), false ); }

        //----------------------------------------------------------------------
        // Deallocates and deconstructs the given object(s).
        // @Params:
        // "data": The object(s) previously allocated from this allocator.
        //----------------------------------------------------------------------
        template <typename T, typename T2 = std::enable_if<!std::is_trivially_destructible<T>::value>::type>
        void deallocate(T* data) { _Deallocate( data, true ); }

    private:
        // All chunks are sorted by memory address.
        std::vector<FreeChunk> m_freeChunks;

        // Find neighbor-chunks for the given chunk. Left or Right can be nullptr.
        void _FindNeighborChunks(const FreeChunk* newChunk, FreeChunk*& left, FreeChunk*& right);

        // Add the given chunk to "m_freeChunks". Merges chunks together if possible.
        void _MergeChunk(FreeChunk* newChunk);

        inline void _RemoveFreeChunk(FreeChunk& freeChunk);
        inline void _AddNewChunk(FreeChunk& newChunk);

        template <typename T>
        inline void _Deallocate(T* mem, bool callDestructors);

        UniversalAllocator (const UniversalAllocator& other)              = delete;
        UniversalAllocator& operator = (const UniversalAllocator& other)  = delete;
        UniversalAllocator (UniversalAllocator&& other)                   = delete;
        UniversalAllocator& operator = (UniversalAllocator&& other)       = delete;
    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    //----------------------------------------------------------------------
    UniversalAllocator::UniversalAllocator( Size amountOfBytes, _IParentAllocator* parentAllocator )
        : _IAllocator( amountOfBytes, parentAllocator )
    {
        ASSERT( m_amountOfBytes > 0 );

        m_data = reinterpret_cast<Byte*>( m_parentAllocator->allocateRaw( m_amountOfBytes) );
        ASSERT( m_data != nullptr );

        m_freeChunks.reserve( INITIAL_FREE_CHUNK_LIST_CAPACITY );
        m_freeChunks.push_back( FreeChunk( m_data, m_amountOfBytes ) );
    }

    //----------------------------------------------------------------------
    template <typename T, typename... Args>
    T* UniversalAllocator::allocate( Size amountOfObjects, Args&&... args )
    {
        static_assert( alignof(T) <= 128, "UniversalAllocator: Max alignment of 128 was exceeded." );

        Size bytesToAllocate = amountOfObjects * sizeof(T);
        T* alignedAddress = reinterpret_cast<T*>( allocateRaw( bytesToAllocate, alignof(T) ) );

        if (alignedAddress != nullptr)
        {
            // Call constructor on every object manually
            for (Size i = 0; i < amountOfObjects; i++)
            {
                T* objectLocation = std::addressof( alignedAddress[i] );
                new (objectLocation) T( std::forward<Arguments>( args )... );
            }

            return alignedAddress;
        }

        _OutOfMemory();
        return nullptr;
    }

    //----------------------------------------------------------------------
    void* UniversalAllocator::allocateRaw( Size amountOfBytes, Size alignment )
    {
        ASSERT( alignment <= 128 && "UniversalAllocator: Max alignment of 128 was exceeded." );

        for (FreeChunk& freeChunk : m_freeChunks)
        {
            void* data = freeChunk.allocateRaw( amountOfBytes, alignment );

            bool allocationWasSuccessful = (data != nullptr);
            if (allocationWasSuccessful)
            {
                bool freeChunkIsEmptyNow = (freeChunk.m_sizeInBytes == 0);
                if (freeChunkIsEmptyNow)
                {
                    _RemoveFreeChunk( freeChunk );
                }

                _LogAllocatedBytes( amountOfBytes );

                return data;
            }
        }

        _OutOfMemory();
        return nullptr;
    }

    //----------------------------------------------------------------------
    template <typename T>
    void UniversalAllocator::_Deallocate( T* mem, bool callDestructors )
    {
        Byte* alignedAddress = reinterpret_cast<Byte*>(mem);
        ASSERT( _InMemoryRange(mem) && "Given memory was not from this allocator!" );

        Size amountOfBytes = _UVAllocatorGetAmountOfBytes( alignedAddress );
        ASSERT( amountOfBytes > 0 && "Given memory was already deallocated!" );

        if (callDestructors)
        {
            Size amountOfObjects = amountOfBytes / sizeof(T);
            T* objStartAddress = reinterpret_cast<T*>(alignedAddress);

            // Call destructor for every object manually
            for (Size i = 0; i < amountOfObjects; i++)
                std::addressof( objStartAddress[i] ) -> ~T();
        }

        Byte offset = _UVAllocatorGetOffset( alignedAddress );
        Size realBytes = amountOfBytes + offset;
        Byte* newChunkAddress = (alignedAddress - offset);

        // Zero out memory to detect if this memory was already deallocated
        memset( newChunkAddress, 0, realBytes );

        // Add a new chunk of free bytes, possible merge it
        FreeChunk newFreeChunk( newChunkAddress, realBytes );
        _MergeChunk( &newFreeChunk );

        _LogDeallocatedBytes( amountOfBytes );
    }

    //----------------------------------------------------------------------
    void UniversalAllocator::_MergeChunk( FreeChunk* newChunk )
    {
        FreeChunk* left = nullptr;
        FreeChunk* right = nullptr;
        _FindNeighborChunks( newChunk, left, right );

        bool didMerge = false;
        if (left != nullptr)
        {
            if (left->touches( *newChunk ))
            {
                left->m_sizeInBytes += newChunk->m_sizeInBytes;
                newChunk = left;
                didMerge = true;
            }
        }
        if (right != nullptr)
        {
            if (right->touches( *newChunk ))
            {
                if (didMerge)
                {
                    newChunk->m_sizeInBytes += right->m_sizeInBytes;
                    _RemoveFreeChunk( *right );
                }
                else
                {
                    right->m_sizeInBytes += newChunk->m_sizeInBytes;
                    right->m_address = newChunk->m_address;
                }
                didMerge = true;
            }
        }

        if (!didMerge)
            _AddNewChunk( *newChunk );
    }

    //----------------------------------------------------------------------
    void UniversalAllocator::_FindNeighborChunks( const FreeChunk* newChunk, FreeChunk*& left, FreeChunk*& right )
    {
        for (Size i = 0; i < m_freeChunks.size(); i++)
        {
            if ( (*newChunk) < m_freeChunks[i] )
            {
                right = &m_freeChunks[i];
                break;
            }
            left = &m_freeChunks[i];
        }
    }

    //----------------------------------------------------------------------
    void UniversalAllocator::_RemoveFreeChunk( FreeChunk& freeChunk )
    {
        m_freeChunks.erase( std::remove( m_freeChunks.begin(), m_freeChunks.end(), freeChunk ), m_freeChunks.end() ); 
    }

    //----------------------------------------------------------------------
    void UniversalAllocator::_AddNewChunk( FreeChunk& newChunk ) 
    { 
        m_freeChunks.push_back( newChunk );
        std::sort( m_freeChunks.begin(), m_freeChunks.end() );
    }

    //----------------------------------------------------------------------
    void* UniversalAllocator::FreeChunk::allocateRaw( Size amountOfBytes, Size alignment )
    {
        ASSERT( amountOfBytes < ((Size)1 << (AMOUNT_OF_BYTES_FOR_SIZE * 8)) 
            && "Not enough bytes in AMOUNT_OF_BYTES_FOR_SIZE to represent the given amountOfBytes" );

        // Allocate always additional bytes to store the amount of 
        // alignment-correction and the amount of bytes to allocate.
        U8 additionalBytes = AMOUNT_OF_BYTES_FOR_OFFSET + AMOUNT_OF_BYTES_FOR_SIZE;
        Byte* alignedAddress = alignAddress( m_address + additionalBytes, alignment );

        Byte* newChunkAddress = alignedAddress + amountOfBytes;
        Size realBytes = (newChunkAddress - m_address);

        bool hasEnoughSpace = (realBytes <= m_sizeInBytes);
        if (hasEnoughSpace)
        {
            Byte offset = static_cast<Byte>(alignedAddress - m_address);
            _UVAllocatorWriteAdditionalBytes( alignedAddress, amountOfBytes, offset );

            m_sizeInBytes -= realBytes;
            m_address = newChunkAddress;

            return alignedAddress;
        }

        return nullptr;
    }

    void _UVAllocatorWriteAdditionalBytes( Byte* alignedAddress, Size amtOfBytes, Byte offset )
    {
        // [ -(Alignment) - offset - amountOfBytes - alignedAddress ]
        Byte* amtOfBytesAddress = (alignedAddress - AMOUNT_OF_BYTES_FOR_SIZE);
        memcpy( amtOfBytesAddress, &amtOfBytes, AMOUNT_OF_BYTES_FOR_SIZE );

        Byte* offsetAddress = (amtOfBytesAddress - AMOUNT_OF_BYTES_FOR_OFFSET);
        memcpy( offsetAddress, &offset, AMOUNT_OF_BYTES_FOR_OFFSET );
    }

    Byte _UVAllocatorGetOffset( Byte* alignedAddress )
    {
        Byte offset = *(alignedAddress - AMOUNT_OF_BYTES_FOR_SIZE - AMOUNT_OF_BYTES_FOR_OFFSET);
        return offset;
    }

    Size _UVAllocatorGetAmountOfBytes( Byte* alignedAddress )
    {
        Size amountOfBytes = 0;
        Byte* amtOfBytesAddress = (alignedAddress - AMOUNT_OF_BYTES_FOR_SIZE);
        memcpy( &amountOfBytes, amtOfBytesAddress, AMOUNT_OF_BYTES_FOR_SIZE );

        return amountOfBytes;
    }

} } // end namespaces