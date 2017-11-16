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
#include "locator.h"

namespace Core { namespace MemoryManagement {

    //----------------------------------------------------------------------
    // Defines
    //----------------------------------------------------------------------

    #define INITIAL_USED_CHUNK_LIST_CAPACITY    32

    //**********************************************************************
    // Stores the handle table and manages free indices. The indices are 
    // calculated from the values in the unused cells.
    //**********************************************************************
    class _HandleTable
    {
        union Handle
        {
            void*   handle;
            Handle* nextHandle;
        };

    public:
        _HandleTable(Size sizeOfTable, _IParentAllocator* allocator);
        ~_HandleTable();

        void*& operator[](Size index) { return m_table[index].handle; }
        void*& get(Size index){ return m_table[index].handle; }

        Size nextFreeHandle();
        void freeHandle(Size handle);

    private:
        _IParentAllocator*  m_allocator;        // Allocator where the memory for the table is from.
        Handle*             m_table;            // Array of Handles
        Size                m_amtOfHandles;     // Amount of handles. Usable are one less, because of the Invalid Index 0.
        Handle*             m_head;             // Points to the next free index

        void _PrintTable();
    };

    //**********************************************************************
    // UniversalAllocationPointer (UAPtr). Encapsulates an object of type T
    // and acts as a normal pointer. Utilizes a HandleTable behind the scenes.
    //**********************************************************************
    template <typename T>
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
        template<typename T2, typename = std::enable_if<std::is_convertible<T2, T>::value>::type>
        UAPtr(const UAPtr<T2>& other) 
            : m_handleTable( other._GetHandleTable() ), m_handle( other._GetHandle() ) {}

        template<typename T2, typename = std::enable_if<std::is_convertible<T2, T>::value>::type>
        UAPtr<T>& operator = (const UAPtr<T2>& other)
        {
            m_handleTable = other._GetHandleTable();
            m_handle = other._GetHandle();
            return *this;
        }

        // Still allow explicit typecasting for e.g. casting to a superclass
        template<typename T2>
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

            template <typename T>
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
            template <typename T>
            void relocateTemplate(Byte* newAddr);
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
        template <typename T, typename... Args>
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
        template <typename T>
        void deallocate(UAPtr<T>& data);

    private:
        UniversalAllocator      m_universalAllocator;
        _HandleTable            m_handleTable;
        std::vector<UsedChunk>  m_usedChunks;

        template <typename T>
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

    UniversalAllocatorDefragmented::UniversalAllocatorDefragmented( Size amountOfBytes, Size _HandleTableSize, _IParentAllocator* parentAllocator )
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
    UAPtr<Byte> UniversalAllocatorDefragmented::allocateRaw( Size amountOfBytes, Size alignment )
    {
        void* mem = m_universalAllocator.allocateRaw( amountOfBytes, alignment );

        // Add entry in _HandleTable
        Size nextFreeHandle = m_handleTable.nextFreeHandle();
        m_handleTable[nextFreeHandle] = mem;

        _AddUsedChunk( nextFreeHandle, amountOfBytes, static_cast<Byte*>(mem) );

        return UAPtr<Byte>( &m_handleTable, nextFreeHandle );
    }

    //----------------------------------------------------------------------
    template <typename T, typename... Args>
    UAPtr<T> UniversalAllocatorDefragmented::allocate( Size amountOfObjects, Args&&... args )
    {
        T* mem = m_universalAllocator.allocate<T>( amountOfObjects, args... );

        // Add entry in _HandleTable
        Size nextFreeHandle = m_handleTable.nextFreeHandle();
        m_handleTable[nextFreeHandle] = mem;

        _AddUsedChunk(nextFreeHandle, amountOfObjects * sizeof(T), mem);

        return UAPtr<T>( &m_handleTable, nextFreeHandle );
    }

    //----------------------------------------------------------------------
    void UniversalAllocatorDefragmented::deallocate( UAPtr<Byte>& data )
    {
        ASSERT( data.isValid() && "Given data was already deallocated or were never allocated." );

        m_universalAllocator.deallocate( data.getRaw() );

        _RemoveUsedChunk( data._GetHandle() );

        m_handleTable.freeHandle( data._GetHandle() );

        data.invalidate();
    }

    //----------------------------------------------------------------------
    template <typename T>
    void UniversalAllocatorDefragmented::deallocate( UAPtr<T>& data )
    {
        ASSERT( data.isValid() && "Given data was already deallocated or were never allocated." );

        m_universalAllocator.deallocate<T, void>( data.getRaw() );

        _RemoveUsedChunk( data._GetHandle() );

        m_handleTable.freeHandle( data._GetHandle() );

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
    template <typename T>
    void UniversalAllocatorDefragmented::_AddUsedChunk( Size handle, Size sizeInBytes, T* type )
    {
        m_usedChunks.push_back( UsedChunk(handle, sizeInBytes, &m_handleTable, type) );
        std::sort( m_usedChunks.begin(), m_usedChunks.end() );
    }

    //----------------------------------------------------------------------
    void UniversalAllocatorDefragmented::_RemoveUsedChunk( Size handle )
    {
        UsedChunk chunkToRemove( handle, &m_handleTable );
        m_usedChunks.erase( std::remove( m_usedChunks.begin(), m_usedChunks.end(), chunkToRemove ), m_usedChunks.end() );
    }

    //**********************************************************************
    // IMPLEMENTATION - UsedChunk
    //**********************************************************************

    template <typename T>
    void UniversalAllocatorDefragmented::UsedChunk::relocateTemplate(Byte* newAddr)
    {
        static_assert( alignof(T) <= 128, "Max Alignment of 128 was exceeded!" );

        Byte* currentAddress = getAddress();

        // Determine new aligned address
        U8 additionalBytes = AMOUNT_OF_BYTES_FOR_OFFSET + AMOUNT_OF_BYTES_FOR_SIZE;
        Byte* alignedAddress = alignAddress( newAddr + additionalBytes, alignof(T) );

        // Save offset and amountOfBytes
        Byte offset = static_cast<Byte>(alignedAddress - newAddr);
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
            Byte tempByteArr[ sizeof(T) + alignof(T) ];
            T* tempObj = reinterpret_cast<T*>( alignAddress( &tempByteArr, alignof(T) ) );

            // Move object first to temp location, then to final position
            for (Size i = 0; i < amtOfObjects; i++)
            {
                new (tempObj) T( std::move( oldData[i] ) );
                std::addressof( oldData[i]) -> ~T();

                new ( std::addressof( newData[i] ) ) T( std::move( *tempObj ) );
                tempObj -> ~T();
            }
        }

        // Update table with new location
        m_handleTable->get( m_handle ) = alignedAddress;
    }

    //**********************************************************************
    // IMPLEMENTATION - HandleTable
    //**********************************************************************

    //----------------------------------------------------------------------
    _HandleTable::_HandleTable( Size sizeOfTable, _IParentAllocator* allocator )
        : m_allocator( allocator ), m_amtOfHandles( sizeOfTable + 1 )
    {
        m_table = static_cast<Handle*>( m_allocator->allocateRaw( m_amtOfHandles * sizeof(void*), alignof(void*) ) );
        m_head = std::addressof( m_table[m_amtOfHandles - 1] );

        for (Size i = (m_amtOfHandles - 1); i > 0; i--)
        {
            m_table[i].nextHandle = std::addressof( m_table[i - 1] );
        }
        m_table[1].nextHandle = nullptr;
    }

    //----------------------------------------------------------------------
    _HandleTable::~_HandleTable()
    {
        m_allocator->deallocate(m_table);
    }

    //----------------------------------------------------------------------
    Size _HandleTable::nextFreeHandle()
    {
        ASSERT( m_head != nullptr && "Out of Handles!" );

        Size nextFreeHandle = (m_head - m_table);
        m_head = m_head->nextHandle;

        return nextFreeHandle;
    }

    //----------------------------------------------------------------------
    void _HandleTable::freeHandle( Size handle )
    {
        ASSERT( handle > 0 && handle <= m_amtOfHandles );

        Handle* newHandle = std::addressof( m_table[handle] );
        newHandle->nextHandle = m_head;
        m_head = newHandle;
    }

    //----------------------------------------------------------------------
    void _HandleTable::_PrintTable()
    {
        LOG("");
        for (Size i = 0; i < m_amtOfHandles; i++)
        {
            String line = TS(i) + ": " + TS( (Size)(&m_table[i]) ) + " -> " + TS( (Size)m_table[i].nextHandle );
            if ( m_head == (&m_table[i]) )
                line += " <-- m_head";
            LOG(line + "\n");
        }
    }

} } // end namespaces