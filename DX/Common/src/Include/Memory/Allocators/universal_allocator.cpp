#include "universal_allocator.h"
/**********************************************************************
    class: UniversalAllocator

    author: S. Hau
    date: October 5, 2017
**********************************************************************/

namespace Memory {

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

} // end namespaces