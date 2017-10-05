#pragma once


namespace MemoryManagement
{

    class IAllocator
    {
    public:
        //virtual void* allocate() = 0;

    protected:
        virtual void _OutOfMemory() { ASSERT(false && "OutOfMemory"); }

        Size m_TotalBytes;
        Size m_TotalUsedBytes;
    };

    //**********************************************************************
    // Aligns an address to the next multiple of "alignment".
    // The alignment MUST be a power of two. Example:
    // > Alignment 0x08:
    // 0x00 = 0x00
    // 0x06 = 0x08
    // 0x08 = 0x08 etc.
    //**********************************************************************
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
