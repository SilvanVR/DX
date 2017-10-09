#pragma once

/**********************************************************************
    class: UniversalAllocatorDefragmented (universal_allocator_defragmented.hpp)

    author: S. Hau
    date: October 5, 2017

    Features:
     [+] Allocations/Deallocations of any size in any order.
     [+] Defragmentation is possible via a method.
     [-] Pointers are encapsulated in a class which uses a HandleTable
         in the background to ensure updated pointers.
     [-] Less performance and bigger memory footprint than the basic
         UniversalAllocator due to the encapsulation.

    @Considerations:
     - Copied UAPtr may be point to a another object, if the previous
       object gets deleted and a new one takes this place.
**********************************************************************/

#include "universal_allocator.hpp"
#include <stack>

namespace MemoryManagement
{

    #define INITIAL_USED_CHUNK_LIST_CAPACITY    32

    //**********************************************************************
    // Stores all handles and manages free indices.
    //**********************************************************************
    class _HandleTable
    {
        using Handle = void*;

    public:
        _HandleTable(Size sizeOfTable, _IParentAllocator* allocator)
            : m_allocator(allocator), m_amtOfHandles(sizeOfTable)
        {
            m_table = (Handle*) m_allocator->allocateRaw( sizeOfTable * sizeof(void*), alignof(void*) );

            for (Size i = (sizeOfTable - 1); i > 0; i--)
                m_unusedHandleStack.push(i);
        }
        ~_HandleTable()
        {
            m_allocator->deallocate( m_table );
        }

        void*& operator[](Size index) { return m_table[index]; }
        void*& get(Size index){ return m_table[index]; }

        // @TODO
        Size nextFreeHandle()
        {
            ASSERT( !m_unusedHandleStack.empty() );
            Size nextFreeHandle = m_unusedHandleStack.top();
            m_unusedHandleStack.pop();

            return nextFreeHandle;
        }
        void freeHandle(Size handle)
        {
            ASSERT( handle < m_amtOfHandles );
            m_unusedHandleStack.push(handle);
        }

    private:
        _IParentAllocator*  m_allocator;
        Handle*             m_table;
        Size                m_amtOfHandles;

        // @TODO: Store unused handles in handle table itself
        std::stack<Size>        m_unusedHandleStack;
    };


    //**********************************************************************
    // UniversalAllocationPointer (UAPtr). Encapsulates an object of type T
    // and acts as a normal pointer. Utilizes a HandleTable behind the scenes.
    //**********************************************************************
    template <class T>
    class UAPtr
    {
        friend class UniversalAllocatorDefragmented; // Access to m_handle

    public:
        UAPtr() : m_handleTable(nullptr), m_handle(0) {}
        UAPtr(std::nullptr_t null) : m_handleTable(nullptr), m_handle(0) {}

        T*          getRaw()                        { return _Get(); }
        T*          operator -> ()                  { return _Get(); }
        const T*    operator -> ()          const   { return _Get(); }
        T&          operator *  ()                  { return *_Get(); }
        const T&    operator *  ()          const   { return *_Get(); }
        T&          operator [] (int index)         { _Get() + index; }
        const T&    operator [] (int index) const   { _Get() + index; }
        bool        operator == (void* ptr)         { return _Get() == ptr; }
        bool        operator == (void* ptr) const   { return _Get() == ptr; }
        bool        operator != (void* ptr)         { return _Get() != ptr; }
        bool        operator != (void* ptr) const   { return _Get() != ptr; }

        // Returns whether this pointer points to a valid object.
        bool        isValid() const { return (m_handleTable != nullptr) && (m_handle != 0); }
        void        invalidate(){ m_handleTable = nullptr; m_handle = 0; }

        // Conversion to a UAPtr with a subclass can happen implicitly
        template<class T2, class = std::enable_if<std::is_convertible<T2, T>::value>::type>
        UAPtr(const UAPtr<T2>& other) 
            : m_handleTable( other._GetHandleTable() ), m_handle( other._GetHandle() ) {}

        template<class T2, class = std::enable_if<std::is_convertible<T2, T>::value>::type>
        UAPtr<T>& operator = (const UAPtr<T2>& other)
        {
            m_handleTable = other._GetHandleTable();
            m_handle = other._GetHandle();
            return *this;
        }

        // Still allow explicit typecasting for e.g. casting to a superclass
        template<class T2>
        explicit UAPtr(const UAPtr<T2>& other)
            : m_handleTable( other._GetHandleTable() ), m_handle( other._GetHandle() ) {}

        // Both methods actually should not accessible from the outside world, 
        // but has to be for conversion to a UAPtr of a different type.
        _HandleTable*   _GetHandleTable() const { return m_handleTable; }
        Size            _GetHandle() const { return m_handle; }

    private:
        explicit UAPtr(_HandleTable* _HandleTable, Size handle)
            : m_handleTable(_HandleTable), m_handle(handle)
        {}

        _HandleTable*   m_handleTable;
        Size            m_handle;

        inline T* _Get(){ return m_handleTable == nullptr ? nullptr : static_cast<T*>( m_handleTable->get( m_handle ) ); }
    };

    //**********************************************************************
    // Features:
    // [+] Allocations / Deallocations of any size in any order.
    // [+] Defragmentation is possible via a method.
    // [-] Pointers are encapsulated in a class which uses a HandleTable
    //     in the background to ensure updated pointers.
    // [-] Less performance and bigger memory footprint than the basic
    //     UniversalAllocator due to the encapsulation.
    //**********************************************************************
    class UniversalAllocatorDefragmented
    {
        //**********************************************************************
        // Represents a used chunk, which can be moved in memory
        //**********************************************************************
        class UsedChunk
        {
        public:
            // Constructor used for a chunk which should be removed (via std::remove)
            UsedChunk(Size handle, _HandleTable* handleTable)
                : m_handle(handle), m_handleTable(handleTable) {}

            template <class T>
            UsedChunk(Size handle, Size sizeInBytes, _HandleTable* handleTable, T* type)
                : m_handle(handle), m_sizeInBytes(sizeInBytes), m_handleTable(handleTable)
            {
                m_relocate = &UsedChunk::relocateTemplate<T>;
            }

            void  relocate(Byte* newAddress){ (this->*m_relocate)( newAddress ); }
            Byte* getAddress() const { return reinterpret_cast<Byte*>( m_handleTable->get( m_handle ) ); }
            Size  getSizeInBytes() const { return m_sizeInBytes; }

            bool operator <  (const UsedChunk& other) const { return getAddress() < other.getAddress(); }
            bool operator >  (const UsedChunk& other) const { return getAddress() > other.getAddress(); }
            bool operator <= (const UsedChunk& other) const { return getAddress() <= other.getAddress(); }
            bool operator >= (const UsedChunk& other) const { return getAddress() >= other.getAddress(); }
            bool operator == (const UsedChunk& other) const { return getAddress() == other.getAddress(); }
            bool operator != (const UsedChunk& other) const { return getAddress() != other.getAddress(); }

        private:
            void (UsedChunk::*m_relocate)(Byte*);
            _HandleTable*           m_handleTable;
            Size                    m_handle;
            Size                    m_sizeInBytes;

            // Relocates this block to the given address
            template <class T>
            void relocateTemplate(Byte* newAddr)
            {
                static_assert( alignof(T) <= 128, "Max Alignment of 128 was exceeded!" );

                Byte* currentAddress = getAddress();

                // Determine new aligned address
                U8 additionalBytes = AMOUNT_OF_BYTES_FOR_OFFSET + AMOUNT_OF_BYTES_FOR_SIZE;
                Byte* alignedAddress = alignAddress( newAddr + additionalBytes, alignof(T) );

                // Save offset and amountOfBytes
                Byte offset = static_cast<Byte>( alignedAddress - newAddr);
                _UVAllocatorWriteAdditionalBytes( alignedAddress, m_sizeInBytes, offset );

                T* oldData = reinterpret_cast<T*>( currentAddress );
                T* newData = reinterpret_cast<T*>( alignedAddress );
                Size amtOfObjects = ( m_sizeInBytes / sizeof(T) );

                // Move memory to new location using move semantics if necessary
                bool memoryOverlaps = (alignedAddress + m_sizeInBytes) >= currentAddress;
                if (std::is_trivially_move_constructible<T>::value)
                {
                    std::memmove( alignedAddress, currentAddress, m_sizeInBytes );
                }
                else if (not memoryOverlaps)
                {
                    // Directly move objects to new location
                    for (Size i = 0; i < amtOfObjects; i++)
                    {
                        new (std::addressof( newData[i] )) T( std::move( oldData[i] ) );
                        std::addressof( oldData[i] ) -> ~T();
                    }
                }
                else
                {
                    Byte tempByteArr[sizeof(T) + alignof(T)];
                    T* tempObj = reinterpret_cast<T*>( alignAddress( &tempByteArr, alignof(T) ) );

                    // Move object first to temp location, then to final position
                    for (Size i = 0; i < amtOfObjects; i++)
                    {
                        new (tempObj) T( std::move(oldData[i] ) );
                        std::addressof( oldData[i] ) -> ~T();

                        new ( std::addressof( newData[i] ) ) T( std::move( *tempObj ) );
                        tempObj -> ~T();
                    }
                }

                // Update table with new location
                m_handleTable->get( m_handle ) = alignedAddress;
            }
        };

    public:
        //----------------------------------------------------------------------
        // @Params:
        // "amountOfBytes": Amount of bytes to allocate.
        // "_HandleTableSize": Maximum amount of handles possible.
        // "parentAllocator": Allocator to which allocate memory from.
        //----------------------------------------------------------------------
        explicit UniversalAllocatorDefragmented(Size amountOfBytes, Size _HandleTableSize, _IParentAllocator* parentAllocator = nullptr);

        //----------------------------------------------------------------------
        // Returns whether an defragmentation is necessary.
        //----------------------------------------------------------------------
        bool canBeDefragmented();

        //----------------------------------------------------------------------
        // Defragment the universal allocator.
        //----------------------------------------------------------------------
        void defragment();

        //----------------------------------------------------------------------
        // Defragment the universal allocator once.
        // @Return:
        //  Whether a defragmentation was executed or not.
        //----------------------------------------------------------------------
        bool defragmentOnce();

        //----------------------------------------------------------------------
        // Allocate specified amount of bytes.
        // @Params:
        // "amountOfBytes": Amount of bytes to allocate
        // "alignment":     Alignment to use. MUST be power of two.
        //----------------------------------------------------------------------
        UAPtr<Byte> allocateRaw(Size amountOfBytes, Size alignment = 1);

        //----------------------------------------------------------------------
        // Allocate "amountOfObjects" objects of type T.
        // @Params:
        // "amountOfObjects": Amount of objects to allocate (array-allocation)
        // "args": Constructor arguments from the class T
        //----------------------------------------------------------------------
        template <class T, class... Args>
        UAPtr<T> allocate(Size amountOfObjects = 1, Args&&... args);

        //----------------------------------------------------------------------
        // Deallocate the given memory. Does not call any destructor.
        // @Params:
        // "mem": The memory previously allocated from this allocator.
        //----------------------------------------------------------------------
        void deallocate(UAPtr<Byte>& data);

        //----------------------------------------------------------------------
        // Deallocates and deconstructs the given object(s).
        // @Params:
        // "data": The object(s) previously allocated from this allocator.
        //----------------------------------------------------------------------
        template <class T>
        void deallocate(UAPtr<T>& data);

    private:
        UniversalAllocator      m_universalAllocator;
        _HandleTable            m_handleTable;
        std::vector<UsedChunk>  m_usedChunks;

        template <class T>
        void _AddUsedChunk(Size handle, Size sizeInBytes, T* type);
        void _RemoveUsedChunk(Size handle);

        UniversalAllocatorDefragmented(const UniversalAllocatorDefragmented& other)                 = delete;
        UniversalAllocatorDefragmented& operator = (const UniversalAllocatorDefragmented& other)    = delete;
        UniversalAllocatorDefragmented(UniversalAllocatorDefragmented&& other)                      = delete;
        UniversalAllocatorDefragmented& operator = (UniversalAllocatorDefragmented&& other)         = delete;
    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    UniversalAllocatorDefragmented::UniversalAllocatorDefragmented(Size amountOfBytes, Size _HandleTableSize, _IParentAllocator* parentAllocator)
        : m_universalAllocator( amountOfBytes, parentAllocator ), 
          m_handleTable( _HandleTableSize, m_universalAllocator.getParentAllocator() ) // Order important!
    {
        m_usedChunks.reserve( INITIAL_USED_CHUNK_LIST_CAPACITY );
    }

    //----------------------------------------------------------------------
    void UniversalAllocatorDefragmented::defragment()
    {
        while ( canBeDefragmented() )
        {
            defragmentOnce();
        }
    }

    //----------------------------------------------------------------------
    bool UniversalAllocatorDefragmented::defragmentOnce()
    {
        if ( not canBeDefragmented() )
            return false;

        // Get the first chunk, which address is where we will move to.
        UniversalAllocator::FreeChunk& freeChunk = m_universalAllocator.m_freeChunks[0];

        // Find first UsedChunk right from the first FreeChunk
        UsedChunk* chunkToRelocate = nullptr;
        for (UsedChunk& chunk : m_usedChunks)
        {
            if (freeChunk.m_address < chunk.getAddress())
            {
                chunkToRelocate = &chunk;
                break;
            }
        }

        // Move the chunk to the new position
        chunkToRelocate->relocate( freeChunk.m_address );

        // Update FreeChunk information
        Byte* newFreeChunkAddress = ( chunkToRelocate->getAddress() + chunkToRelocate->getSizeInBytes() );
        freeChunk.m_address = newFreeChunkAddress;

        Size amountOfBytes = _UVAllocatorGetAmountOfBytes( chunkToRelocate->getAddress() );
        Byte newOffset = _UVAllocatorGetOffset( chunkToRelocate->getAddress() );
        freeChunk.m_sizeInBytes = (amountOfBytes + newOffset);

        // If the updated chunk touches now the following one, merge them
        if (m_universalAllocator.m_freeChunks.size() > 1)
        {
            UniversalAllocator::FreeChunk& nextFreeChunk = m_universalAllocator.m_freeChunks[1];
            if (freeChunk.touches( nextFreeChunk ))
            {
                freeChunk.m_sizeInBytes += nextFreeChunk.m_sizeInBytes;
                m_universalAllocator._RemoveFreeChunk( nextFreeChunk );
            }
        }

        return true;
    }

    //----------------------------------------------------------------------
    UAPtr<Byte> UniversalAllocatorDefragmented::allocateRaw(Size amountOfBytes, Size alignment)
    {
        void* mem = m_universalAllocator.allocateRaw( amountOfBytes, alignment );

        // Add entry in _HandleTable
        Size nextFreeHandle = m_handleTable.nextFreeHandle();
        m_handleTable[nextFreeHandle] = mem;

        _AddUsedChunk( nextFreeHandle, amountOfBytes, static_cast<Byte*>(mem) );

        return UAPtr<Byte>( &m_handleTable, nextFreeHandle );
    }

    //----------------------------------------------------------------------
    template <class T, class... Args>
    UAPtr<T> UniversalAllocatorDefragmented::allocate(Size amountOfObjects, Args&&... args)
    {
        T* mem = m_universalAllocator.allocate<T>( amountOfObjects, args... );

        // Add entry in _HandleTable
        Size nextFreeHandle = m_handleTable.nextFreeHandle();
        m_handleTable[nextFreeHandle] = mem;

        _AddUsedChunk(nextFreeHandle, amountOfObjects * sizeof(T), mem);

        return UAPtr<T>( &m_handleTable, nextFreeHandle );
    }

    //----------------------------------------------------------------------
    void UniversalAllocatorDefragmented::deallocate(UAPtr<Byte>& data)
    {
        ASSERT( data.isValid() && "Given data was already deallocated or were never allocated." );

        m_handleTable.freeHandle( data._GetHandle() );

        _RemoveUsedChunk( data._GetHandle() );

        m_universalAllocator.deallocate( data.getRaw() );

        data.invalidate();
    }

    //----------------------------------------------------------------------
    template <class T>
    void UniversalAllocatorDefragmented::deallocate(UAPtr<T>& data)
    {
        ASSERT( data.isValid() && "Given data was already deallocated or were never allocated." );

        m_handleTable.freeHandle( data._GetHandle() );

        _RemoveUsedChunk( data._GetHandle() );

        m_universalAllocator.deallocate<T, void>( data.getRaw() );

        data.invalidate();
    }

    //----------------------------------------------------------------------
    bool UniversalAllocatorDefragmented::canBeDefragmented()
    {
        bool hasOnlyOneFreeChunk = (m_universalAllocator.m_freeChunks.size() == 1);

        bool isFreeChunkAtEnd = false;
        if (hasOnlyOneFreeChunk)
        {
            UniversalAllocator::FreeChunk& freeChunk = m_universalAllocator.m_freeChunks[0];

            Byte* freeChunkEndAddress = (freeChunk.m_address + freeChunk.m_sizeInBytes);
            Byte* uaEndAddress = (m_universalAllocator.m_data + m_universalAllocator.m_amountOfBytes);

            isFreeChunkAtEnd = (freeChunkEndAddress == uaEndAddress);
        }

        bool freeChunkAvailable = (m_universalAllocator.m_freeChunks.size() != 0);
        if (not freeChunkAvailable || isFreeChunkAtEnd)
        {
            return false;
        }

        return true;
    }

    //----------------------------------------------------------------------
    template <class T>
    void UniversalAllocatorDefragmented::_AddUsedChunk(Size handle, Size sizeInBytes, T* type)
    {
        m_usedChunks.push_back( UsedChunk(handle, sizeInBytes, &m_handleTable, type) );
        std::sort( m_usedChunks.begin(), m_usedChunks.end() );
    }

    //----------------------------------------------------------------------
    void UniversalAllocatorDefragmented::_RemoveUsedChunk(Size handle)
    {
        UsedChunk chunkToRemove( handle, &m_handleTable );
        m_usedChunks.erase( std::remove( m_usedChunks.begin(), m_usedChunks.end(), chunkToRemove ), m_usedChunks.end() );
    }

}