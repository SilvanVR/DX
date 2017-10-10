#include "IAllocator.h"

/**********************************************************************
    class: IParentAllocator + _IAllocator + DefaultAllocator (_IAllocator.cpp)

    author: S. Hau
    date: October 5, 2017
**********************************************************************/


namespace Core { namespace MemoryManagement {

    //----------------------------------------------------------------------
    _IAllocator::_IAllocator( Size amountOfBytes, _IParentAllocator* parentAllocator )
        : m_amountOfBytes( amountOfBytes ), m_parentAllocator( parentAllocator )
    {
        if (m_parentAllocator == nullptr)
            m_parentAllocator = _DefaultAllocator::get();
    }

    //----------------------------------------------------------------------
    _IAllocator::~_IAllocator()
    {
        auto& memInfo = getAllocationMemoryInfo();
        ASSERT( hasAllocatedBytes() && "There is still allocated memory somewhere!" );

        m_parentAllocator->deallocate( m_data );
        m_data = nullptr;
    }

    //----------------------------------------------------------------------
    void* _DefaultAllocator::allocateRaw( Size amountOfBytes, Size alignment )
    {
        // Ignore alignment here
        return new Byte[amountOfBytes];
    }

    //----------------------------------------------------------------------
    void _DefaultAllocator::deallocate( void* mem )
    {
        delete[] mem;
    }

} } // end namespaces