#include "stack_allocator.h"
/**********************************************************************
    class: StackAllocator

    author: S. Hau
    date: October 4, 2017
**********************************************************************/

namespace Memory
{

    //----------------------------------------------------------------------
    StackAllocator::StackAllocator( U32 amountOfBytes, _IParentAllocator* parentAllocator )
        : _IAllocator( amountOfBytes, parentAllocator )
    {
        ASSERT( m_amountOfBytes > 0 );

        m_data = reinterpret_cast<Byte*>( m_parentAllocator->allocateRaw( m_amountOfBytes ) );
        ASSERT( m_data != nullptr );

        m_head = m_data;

        m_destructors.reserve( INITIAL_DESTRUCTOR_LIST_CAPACITY );
    }

    //----------------------------------------------------------------------
    StackAllocator::~StackAllocator()
    {
        m_head = nullptr;
    }

    //----------------------------------------------------------------------
    void* StackAllocator::allocateRaw( Size amountOfBytes, Size alignment )
    {
        Byte* alignedAddress = alignAddress( m_head, alignment );
        Byte* newHeadPointer = alignedAddress + amountOfBytes;

        bool hasEnoughSpace = ( newHeadPointer <= (m_data + m_amountOfBytes) );
        if (hasEnoughSpace)
        {
            m_head = newHeadPointer;

            _LogAllocatedBytes( amountOfBytes );
            return alignedAddress;
        }

        _OutOfMemory();
        return nullptr;
    }

    //----------------------------------------------------------------------
    void StackAllocator::clear()
    {
        m_head = m_data;
        for (auto& destructor : m_destructors)
            destructor();
        m_destructors.clear();

        if (getAllocationMemoryInfo().bytesAllocated > 0)
            _LogDeallocatedBytes( getAllocationMemoryInfo().bytesAllocated );
    }

    //----------------------------------------------------------------------
    void StackAllocator::clearToMarker( StackAllocatorMarker marker )
    {
        // Marker address could be null OR the stack was already cleared behind the marker
        ASSERT( marker.m_address != nullptr && marker.m_address < m_head && "Marker was invalid" );
        Size amountOfBytes = (m_head - marker.m_address);

        m_head = marker.m_address;
        while (m_destructors.size() > marker.m_amountOfDestructors)
        {
            m_destructors.back()();
            m_destructors.pop_back();
        }

        _LogDeallocatedBytes( amountOfBytes );
    }

}