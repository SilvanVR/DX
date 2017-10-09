#pragma once

/**********************************************************************
    class: UniversalAllocatorDefragmented (universal_allocator_defragmented.hpp)

    author: S. Hau
    date: October 5, 2017

    Features:
    - (+) Allocations/Deallocations of any size in any order.
    - (+) Defragmentation is possible via a method.
    - (-) Pointers are encapsulated in a class which uses a HandleTable
          in the background to ensure updated pointers.
    - (-) Less performance than the basic UniversalAllocator due to
          the encapsulation.
    Be careful about pointers pointing to memory in this allocator :-)
**********************************************************************/

#include "universal_allocator.hpp"
#include <stack>

namespace MemoryManagement
{

    namespace INTERNAL
    {
        class HandleTable
        {
            struct Handle { void* m_handle; };

        public:
            HandleTable(Size sizeOfTable, _IParentAllocator* allocator)
                : m_allocator(allocator), m_amtOfHandles(sizeOfTable)
            {
                m_table = (Handle*) m_allocator->allocateRaw( sizeOfTable * sizeof(void*), alignof(void*) );

                for (Size i = (sizeOfTable - 1); i > 0; i--)
                    m_unusedHandleStack.push(i);
            }
            ~HandleTable()
            {
                m_allocator->deallocate( m_table );
            }

            void*& operator[](Size index) { return m_table[index].m_handle; }
            void*& get(Size index){ return m_table[index].m_handle; }

            // @TODO
            Size nextFreeHandle()
            {
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
    }

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

        T*          getRaw(){ return _Get(); }

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

        // Still allow explicit typecasting
        template<class T2>
        explicit UAPtr(const UAPtr<T2>& other)
            : m_handleTable( other._GetHandleTable() ), m_handle( other._GetHandle() ) {}

        // Both methods actually should not accessible from the outside world, 
        // but has to be for conversion to a UAPtr of a different type.
        INTERNAL::HandleTable* _GetHandleTable() const { return m_handleTable; }
        Size _GetHandle() const { return m_handle; }

    private:
        explicit UAPtr(INTERNAL::HandleTable* handleTable, Size handle)
            : m_handleTable(handleTable), m_handle(handle)
        {}

        INTERNAL::HandleTable*  m_handleTable;
        Size                    m_handle;

        inline T* _Get(){ return m_handleTable == nullptr ? nullptr : static_cast<T*>( m_handleTable->get(m_handle) ); }
    };


    class UniversalAllocatorDefragmented
    {
    public:
        explicit UniversalAllocatorDefragmented(Size amountOfBytes, Size handleTableSize, _IParentAllocator* parentAllocator = nullptr);

        //----------------------------------------------------------------------
        // Defragment the universal allocator once.
        // @Return:
        //  Whether defragmentation was necessary or not.
        //----------------------------------------------------------------------
        bool defragment();

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
        void deallocate(UAPtr<Byte> data);

        //----------------------------------------------------------------------
        // Deallocates and deconstructs the given object(s).
        // @Params:
        // "data": The object(s) previously allocated from this allocator.
        //----------------------------------------------------------------------
        template <class T>
        void deallocate(UAPtr<T> data);

    private:
        UniversalAllocator      m_universalAllocator;
        INTERNAL::HandleTable   m_handleTable;

        bool _NeedsDefragmentation();
    };


    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    UniversalAllocatorDefragmented::UniversalAllocatorDefragmented(Size amountOfBytes, Size handleTableSize, _IParentAllocator* parentAllocator)
        : m_universalAllocator(amountOfBytes, parentAllocator), m_handleTable(handleTableSize, m_universalAllocator.getParentAllocator())
    {

    }

    //----------------------------------------------------------------------
    bool UniversalAllocatorDefragmented::defragment()
    {
        if (not _NeedsDefragmentation())
        {
            return false;
        }

        // Defragment once

        return true;
    }

    //----------------------------------------------------------------------
    UAPtr<Byte> UniversalAllocatorDefragmented::allocateRaw(Size amountOfBytes, Size alignment)
    {
        void* mem = m_universalAllocator.allocateRaw( amountOfBytes, alignment );

        Size nextFreeHandle = m_handleTable.nextFreeHandle();
        m_handleTable[nextFreeHandle] = mem;

        return UAPtr<Byte>( &m_handleTable, nextFreeHandle );
    }

    //----------------------------------------------------------------------
    template <class T, class... Args>
    UAPtr<T> UniversalAllocatorDefragmented::allocate(Size amountOfObjects, Args&&... args)
    {
        T* mem = m_universalAllocator.allocate<T>( amountOfObjects, args... );

        Size nextFreeHandle = m_handleTable.nextFreeHandle();
        m_handleTable[nextFreeHandle] = mem;

        return UAPtr<T>( &m_handleTable, nextFreeHandle );
    }

    //----------------------------------------------------------------------
    void UniversalAllocatorDefragmented::deallocate(UAPtr<Byte> data)
    {
        m_handleTable.freeHandle( data.m_handle );

        m_universalAllocator.deallocate( data.getRaw() );
    }

    //----------------------------------------------------------------------
    template <class T>
    void UniversalAllocatorDefragmented::deallocate(UAPtr<T> data)
    {
        m_handleTable.freeHandle( data.m_handle );

        m_universalAllocator.deallocate<T, void>( data.getRaw() );
    }


    //----------------------------------------------------------------------
    bool UniversalAllocatorDefragmented::_NeedsDefragmentation()
    {
        return true;
    }


}