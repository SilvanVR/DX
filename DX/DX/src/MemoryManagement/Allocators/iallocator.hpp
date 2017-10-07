#pragma once

/**********************************************************************
    class: IAllocator + DefaultAllocator (iallocator.hpp)

    author: S. Hau
    date: October 5, 2017

    IAllocator interface.
    DefaultAllocator will be used as a parent-allocator
    if no other allocator was specified. It allocates using
    the global new + delete scheme.
**********************************************************************/

namespace MemoryManagement
{

    class IAllocator
    {
    public:
        //----------------------------------------------------------------------
        // Allocate specified amount of bytes.
        // @Params:
        // "amountOfBytes": Amount of bytes to allocate
        // "alignment":     Alignment of the bytes
        //----------------------------------------------------------------------
        virtual void* allocateRaw(Size amountOfBytes, Size alignment = 1) { return nullptr; }

        //----------------------------------------------------------------------
        // Deallocate the given memory.
        // @Params:
        // "mem": The memory previously allocated from this allocator.
        //----------------------------------------------------------------------
        virtual void deallocate(void* mem) {}

        //----------------------------------------------------------------------
        const MemoryInfo& getMemoryInfo(){ return m_memoryInfo; }

    protected:
        IAllocator() {}
        IAllocator(IAllocator* parentAllocator);

        virtual void _OutOfMemory() { ASSERT(false && "OutOfMemory"); }

        IAllocator*     m_parentAllocator;
        MemoryInfo      m_memoryInfo;
    };

    //*********************************************************************
    // Default-Allocator used when no parentAllocator was specified
    // during construction of an allocator (e.g. Pool, Stack...).
    // The Default-Allocator allocates memory using global new and delete.
    //*********************************************************************
    class DefaultAllocator : public IAllocator
    {
    public:
        DefaultAllocator() : IAllocator() {}

        void* allocateRaw(Size amountOfBytes, Size alignment) override
        {
            // Ignore alignment here
            return new Byte[amountOfBytes];
        }

        void  deallocate(void* mem) override
        {
            delete[] mem;
        }
    };

    // Static instance of the default-allocator.
    static DefaultAllocator defaultAllocator;

    //----------------------------------------------------------------------
    IAllocator::IAllocator(IAllocator* parentAllocator)
        : m_parentAllocator(parentAllocator)
    {
        if (m_parentAllocator == nullptr)
            m_parentAllocator = &defaultAllocator;
    }

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
        ASSERT((alignment & (alignment - 1)) == 0); // alignment must be power of 2

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
