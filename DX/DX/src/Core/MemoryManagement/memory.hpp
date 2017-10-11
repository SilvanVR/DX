#pragma once

/**********************************************************************
    class: None (memory.h)

    author: S. Hau
    date: October 7, 2017

    Overloads the global new/delete operator to 
    keep track of all allocated memory.
**********************************************************************/

void* operator new( Size size )
{
    return Core::MemoryManagement::_GlobalNewAndDeleteAllocator::allocate( size );
}

void operator delete( void* mem )
{
    Core::MemoryManagement::_GlobalNewAndDeleteAllocator::deallocate( mem );
}

void* operator new[]( Size size )
{
    return Core::MemoryManagement::_GlobalNewAndDeleteAllocator::allocate( size );
}

void operator delete[]( void* mem )
{
    Core::MemoryManagement::_GlobalNewAndDeleteAllocator::deallocate( mem );
}

//----------------------------------------------------------------------

void* operator new( size_t size, const char* file, U32 line )
{
    return Core::MemoryManagement::_GlobalNewAndDeleteAllocator::allocateDebug( size, file, line );
}

void operator delete( void* mem, const char* file, U32 line )
{
    Core::MemoryManagement::_GlobalNewAndDeleteAllocator::deallocateDebug( mem, file, line );
}

void* operator new[]( size_t size, const char* file, U32 line )
{
    return Core::MemoryManagement::_GlobalNewAndDeleteAllocator::allocateDebug( size, file, line );
}

void operator delete[]( void* mem, const char* file, U32 line )
{
    Core::MemoryManagement::_GlobalNewAndDeleteAllocator::deallocateDebug( mem, file, line );
}