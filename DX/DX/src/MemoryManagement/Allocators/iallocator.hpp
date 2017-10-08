#pragma once

/**********************************************************************
    class: IParentAllocator + IAllocator + DefaultAllocator (iallocator.hpp)

    author: S. Hau
    date: October 5, 2017

    IParentAllocator:
        Interface for a parent allocator.
    IAllocator:
        Interface for a basic allocator. Keeps track of allocations/
        deallocations and serves as a common hub amongst all allocators.
    DefaultAllocator:
        Will be used as a parent-allocator if no other allocator was 
        specified. It allocates using the global new + delete scheme.
**********************************************************************/

namespace MemoryManagement
{

    //*********************************************************************
    // Every allocator inheriting from this class and overriding the two
    // methods below can act as an parent allocator.
    //*********************************************************************
    class IParentAllocator
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
    class DefaultAllocator : public IParentAllocator
    {
    public:
        DefaultAllocator() : IParentAllocator() {}

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

    //*********************************************************************
    // IAllocator interface. Stores the parent allocator and 
    // the memory information struct.
    //*********************************************************************
    class IAllocator
    {
    public:
        //----------------------------------------------------------------------
        const MemoryInfo&   getMemoryInfo() const { return m_memoryInfo; }
        bool                hasAllocatedBytes() const { return m_memoryInfo.currentBytesAllocated == 0; }

    protected:
        IAllocator(Size amountOfBytes, IParentAllocator* parentAllocator);
        virtual ~IAllocator();

        inline void _OutOfMemory() const { ASSERT( false && "OutOfMemory" ); }
        inline bool _InMemoryRange(void* data) const { return (data >= m_data) && ( data < (m_data + m_amountOfBytes) ); }
        inline void _LogAllocatedBytes(Size amtOfBytes);
        inline void _LogDeallocatedBytes(Size amtOfBytes);

        IParentAllocator*   m_parentAllocator;
        Size                m_amountOfBytes;
        Byte*               m_data;

    private:
        MemoryInfo          m_memoryInfo;
    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    //----------------------------------------------------------------------
    IAllocator::IAllocator(Size amountOfBytes, IParentAllocator* parentAllocator)
        : m_amountOfBytes(amountOfBytes), m_parentAllocator(parentAllocator)
    {
        if (m_parentAllocator == nullptr)
            m_parentAllocator = &defaultAllocator;
    }

    //----------------------------------------------------------------------
    IAllocator::~IAllocator()
    {
        auto& memInfo = getMemoryInfo();
        ASSERT( hasAllocatedBytes() && "There is still allocated memory somewhere!" );

        m_parentAllocator->deallocate( m_data );
        m_data = nullptr;
    }

    //----------------------------------------------------------------------
    void IAllocator::_LogAllocatedBytes(Size amtOfBytes)
    {
        m_memoryInfo.currentBytesAllocated += amtOfBytes;
        m_memoryInfo.totalBytesAllocated += amtOfBytes;
        m_memoryInfo.totalAllocations++;
    }

    //----------------------------------------------------------------------
    void IAllocator::_LogDeallocatedBytes(Size amtOfBytes)
    {
        m_memoryInfo.currentBytesAllocated -= amtOfBytes;
        m_memoryInfo.totalBytesFreed += amtOfBytes;
        m_memoryInfo.totalDeallocations++;
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
