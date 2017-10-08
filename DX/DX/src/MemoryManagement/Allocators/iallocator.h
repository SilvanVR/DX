#pragma once

/**********************************************************************
    class: IParentAllocator + _IAllocator + DefaultAllocator (_IAllocator.hpp)

    author: S. Hau
    date: October 5, 2017

    IParentAllocator:
        Interface for a parent allocator.
    _IAllocator:
        Interface for a basic allocator. Keeps track of allocations/
        deallocations and serves as a common hub amongst all allocators.
    DefaultAllocator:
        Will be used as a parent-allocator if no other allocator was 
        specified. It allocates using the global new + delete scheme.
**********************************************************************/

#include "../memory_structs.hpp"

namespace MemoryManagement
{

    //*********************************************************************
    // Every allocator inheriting from this class and overriding the two
    // methods below can act as an parent allocator.
    //*********************************************************************
    class _IParentAllocator
    {
    public:
        //----------------------------------------------------------------------
        // Allocate specified amount of bytes.
        // @Params:
        // "amountOfBytes": Amount of bytes to allocate.
        // "alignment":     Alignment to use. MUST be power of two.
        //----------------------------------------------------------------------
        virtual void* allocateRaw(Size amountOfBytes, Size alignment = 1) = 0;

        //----------------------------------------------------------------------
        // Deallocate the given memory.
        // @Params:
        // "mem": The memory previously allocated from this allocator.
        //----------------------------------------------------------------------
        virtual void deallocate(void* mem) = 0;
    };

    //*********************************************************************
    // Default-Allocator used when no parentAllocator was specified
    // during construction of an allocator (e.g. Pool, Stack...).
    // The Default-Allocator allocates memory using global new and delete.
    //*********************************************************************
    class _DefaultAllocator : public _IParentAllocator
    {
    public:
        _DefaultAllocator() : _IParentAllocator() {}

        static _DefaultAllocator* get(){ static _DefaultAllocator sInstance; return &sInstance; }

        void* allocateRaw(Size amountOfBytes, Size alignment) override;
        void  deallocate(void* mem) override;
    };

    //*********************************************************************
    // _IAllocator interface. Common functionality for all allocators.
    //*********************************************************************
    class _IAllocator
    {
        friend class PoolListAllocator; // Access to _InMemoryRange()

    public:
        //----------------------------------------------------------------------
        const AllocationMemoryInfo&  getAllocationMemoryInfo() const { return m_allocationMemoryInfo; }
        bool hasAllocatedBytes() const { return m_allocationMemoryInfo.currentBytesAllocated == 0; }

    protected:
        _IAllocator(Size amountOfBytes, _IParentAllocator* parentAllocator);
        virtual ~_IAllocator();

        void _OutOfMemory() const { ASSERT( false && "OutOfMemory" ); }
        bool _InMemoryRange(void* data) const { return (data >= m_data) && ( data < (m_data + m_amountOfBytes) ); }
        void _LogAllocatedBytes(Size amtOfBytes) { m_allocationMemoryInfo.addAllocation(amtOfBytes); }
        void _LogDeallocatedBytes(Size amtOfBytes) { m_allocationMemoryInfo.removeAllocation(amtOfBytes); }

        _IParentAllocator*      m_parentAllocator;
        Size                    m_amountOfBytes;
        Byte*                   m_data;

    private:
        AllocationMemoryInfo    m_allocationMemoryInfo;
    };

    //----------------------------------------------------------------------
    // Aligns an address to the next multiple of "alignment".
    // The alignment MUST be a power of two. Example:
    // > Alignment 0x08:
    // 0x00 = 0x00
    // 0x06 = 0x08
    // 0x08 = 0x08 etc.
    //----------------------------------------------------------------------
    template <class T>
    T* alignAddress(T* address, Size alignment)
    {
        ASSERT( (alignment & (alignment - 1) ) == 0); // alignment must be power of 2

        // Interpret address as a normal value
        Size rawAddress = reinterpret_cast<Size>( address );

        // Calculated the misalignment
        Size mask = (alignment - 1);
        Size misalignment = (rawAddress & mask);
        Size adjustment = misalignment == 0 ? 0 : (alignment - misalignment);

        // Calculate the aligned address
        Size alignedAddress = (rawAddress + adjustment);
        return reinterpret_cast<T*>( alignedAddress );
    }
}
