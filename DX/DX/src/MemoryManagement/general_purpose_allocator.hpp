#pragma once

/**********************************************************************
    class: GeneralPurposeAllocator (general_purpose_allocator.hpp)

    author: S. Hau
    date: October 5, 2017

    GeneralPurposeAllocator interface. This allocator preallocates a
    fixed amount of memory beforehand to AVOID dynamic
    memory allocation from the OS, which is in general very costly.
    Features:
    - Allocations can be made of arbitrary size and amounts (arrays)
    - Deallocations can be made in any order
    - (---) Memory will become fragmented
    Be careful about pointers pointing to memory in this allocator :-)
**********************************************************************/

#include "iallocator.hpp"

namespace MemoryManagement
{
    // Amount of bytes additionally allocated to store necessary information needed for deallocation
    static const U8 AMOUNT_OF_BYTES_FOR_SIZE    = 2;
    static const U8 AMOUNT_OF_BYTES_FOR_OFFSET  = 1;

    class GeneralPurposeAllocator : public IAllocator
    {
        //**********************************************************************
        // Represents one contiguous chunk of bytes in this allocator.
        //**********************************************************************
        struct FreeChunk
        {
            Byte*   m_address;
            Size    m_sizeInBytes;

            FreeChunk(Byte* address, Size size)
                : m_address(address), m_sizeInBytes(size) {}

            // Try to allocate the given amount of objects. Nullptr if not enough space in this chunk.
            template <class T, class... Arguments>
            T* allocate(Size amountOfObjects = 1, Arguments&&... args);

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
        explicit GeneralPurposeAllocator(Size amountOfBytes);
        ~GeneralPurposeAllocator();

        //----------------------------------------------------------------------
        // Allocate "amountOfObjects" objects of type T.
        // @Params:
        // "amountOfObjects": Amount of objects to allocate (array-allocation)
        // "args": Constructor arguments from the class T
        //----------------------------------------------------------------------
        template <class T, class... Args>
        T* allocate(Size amountOfObjects = 1, Args&&... args);

        //----------------------------------------------------------------------
        // Deallocates and deconstructs the given object(s).
        // @Params:
        // "data": The object(s) previously allocated from this allocator.
        //----------------------------------------------------------------------
        template <class T>
        void deallocate(T* data);

    private:
        Byte*   m_data;
        Size    m_sizeInBytes;

        // All chunks are sorted by memory address.
        std::vector<FreeChunk> m_freeChunks;

        // Find neighbor-chunks for the given chunk. Left or Right can be nullptr.
        void _FindNeighborChunks(const FreeChunk* newChunk, FreeChunk*& left, FreeChunk*& right);

        // Add the given chunk to "m_freeChunks". Merges chunks together if possible.
        void _MergeChunk(FreeChunk* newChunk);

        bool _InMemoryRange(void* data) { return (data >= m_data) && (data < (m_data + m_sizeInBytes)); }

        GeneralPurposeAllocator (const GeneralPurposeAllocator& other)              = delete;
        GeneralPurposeAllocator& operator = (const GeneralPurposeAllocator& other)  = delete;
        GeneralPurposeAllocator (GeneralPurposeAllocator&& other)                   = delete;
        GeneralPurposeAllocator& operator = (GeneralPurposeAllocator&& other)       = delete;
    };


    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    //----------------------------------------------------------------------
    GeneralPurposeAllocator::GeneralPurposeAllocator(Size amountOfBytes)
        : m_sizeInBytes(amountOfBytes)
    {
        ASSERT( m_sizeInBytes > 0 );

        m_data = new Byte[m_sizeInBytes];
        m_freeChunks.push_back( FreeChunk( m_data, m_sizeInBytes ) );
    }

    //----------------------------------------------------------------------
    GeneralPurposeAllocator::~GeneralPurposeAllocator()
    {
        ASSERT( m_freeChunks.front().m_sizeInBytes == m_sizeInBytes && "Not everything was deallocated!" );
        delete[] m_data;

        m_data = nullptr;
    }

    //----------------------------------------------------------------------
    template <class T, class... Args>
    T* GeneralPurposeAllocator::allocate(Size amountOfObjects, Args&&... args)
    {
        static_assert(alignof(T) <= 128, "GeneralPurposeAllocator: Max alignment of 128 was exceeded.");

        for (FreeChunk& freeChunk : m_freeChunks)
        {
            T* data = freeChunk.allocate<T>( amountOfObjects, std::forward<Args>(args)... );

            bool allocationWasSuccessful = (data != nullptr);
            if (allocationWasSuccessful)
            {
                bool freeChunkIsEmptyNow = (freeChunk.m_sizeInBytes == 0);
                if (freeChunkIsEmptyNow)
                {
                    m_freeChunks.erase( std::remove( m_freeChunks.begin(), m_freeChunks.end(), freeChunk ), m_freeChunks.end() );
                }
                return data;
            }
        }

        // TODO: Handle out of memory gracefully e.g. allocate more
        _OutOfMemory();
        return nullptr;
    }

    //----------------------------------------------------------------------
    template <class T>
    void GeneralPurposeAllocator::deallocate(T* data)
    {
        Byte* alignedAddress = reinterpret_cast<Byte*>(data);
        ASSERT ( _InMemoryRange(data) && "Given memory was not from this allocator!" );

        // Retrieve amountOfObjects + Offset
        Size amountOfObjects = 0;
        Byte* amtOfObjectsAddress = (alignedAddress - AMOUNT_OF_BYTES_FOR_SIZE);
        memcpy( &amountOfObjects, amtOfObjectsAddress, AMOUNT_OF_BYTES_FOR_SIZE );

        ASSERT( amountOfObjects > 0 && "Given memory was already deallocated!" );

        Byte offset = 0;
        Byte* offsetAddress = amtOfObjectsAddress - AMOUNT_OF_BYTES_FOR_OFFSET;
        memcpy( &offset, offsetAddress, AMOUNT_OF_BYTES_FOR_OFFSET );

        // Call destructor for every object
        for (Size i = 0; i < amountOfObjects; i++)
            std::addressof( data[i] ) -> ~T();

        Size realBytes = (amountOfObjects * sizeof(T)) + offset;
        Byte* newChunkAddress = (alignedAddress - offset);

        // Zero out memory to detect if this memory was already deallocated
        memset(newChunkAddress, 0, realBytes);

        // Add a new chunk of free bytes, possible merge it
        FreeChunk newFreeChunk( newChunkAddress, realBytes );
        _MergeChunk(&newFreeChunk);
    }

    //----------------------------------------------------------------------
    void GeneralPurposeAllocator::_MergeChunk(FreeChunk* newChunk)
    {
        FreeChunk* left = nullptr;
        FreeChunk* right = nullptr;
        _FindNeighborChunks(newChunk, left, right);

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
                    m_freeChunks.erase( std::remove( m_freeChunks.begin(), m_freeChunks.end(), *right ), m_freeChunks.end() );
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
        {
            m_freeChunks.push_back( *newChunk );
            std::sort( m_freeChunks.begin(), m_freeChunks.end() );
        }
    }

    //----------------------------------------------------------------------
    void GeneralPurposeAllocator::_FindNeighborChunks(const FreeChunk* newChunk, FreeChunk*& left, FreeChunk*& right)
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
    template <class T, class... Arguments>
    T* GeneralPurposeAllocator::FreeChunk::allocate(Size amountOfObjects, Arguments&&... args)
    {
        ASSERT( amountOfObjects < ((Size)1 << (AMOUNT_OF_BYTES_FOR_SIZE * 8)) );

        // Allocate always additional bytes to store the amount of 
        // alignment-correction and the amount of objects to allocate.
        U8 additionalBytes = AMOUNT_OF_BYTES_FOR_OFFSET + AMOUNT_OF_BYTES_FOR_SIZE;
        Byte* alignedAddress = alignAddress( m_address + additionalBytes, alignof(T) );

        Size bytesToAllocate = amountOfObjects * sizeof(T);
        Byte* newChunkAddress = alignedAddress + bytesToAllocate;

        Size realBytes = (newChunkAddress - m_address);

        bool hasEnoughSpace = (realBytes <= m_sizeInBytes);
        if (hasEnoughSpace)
        {
            // Save offset and amountOfObject-size
            // [ -(Alignment) - offset - amountOfObjects - alignedAddress ]
            Byte* amtOfObjectsAddress = (alignedAddress - AMOUNT_OF_BYTES_FOR_SIZE);
            memcpy( amtOfObjectsAddress, &amountOfObjects, AMOUNT_OF_BYTES_FOR_SIZE );

            Byte offset = static_cast<Byte>(alignedAddress - m_address);
            Byte* offsetAddress = amtOfObjectsAddress - AMOUNT_OF_BYTES_FOR_OFFSET;
            memcpy( offsetAddress, &offset, AMOUNT_OF_BYTES_FOR_OFFSET );

            // Call constructor on every object
            T* returnPointer = reinterpret_cast<T*>( alignedAddress );
            for (Size i = 0; i < amountOfObjects; i++)
            {
                T* objectLocation = std::addressof( returnPointer[i] );
                new (objectLocation) T( std::forward<Arguments>(args)... );
            }

            m_sizeInBytes -= realBytes;
            m_address = newChunkAddress;

            return returnPointer;
        }
        return nullptr;
    }


}