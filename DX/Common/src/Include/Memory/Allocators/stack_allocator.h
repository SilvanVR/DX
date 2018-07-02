#pragma once

/**********************************************************************
    class: StackAllocator (stack_allocator.hpp)

    author: S. Hau
    date: October 4, 2017

    A custom allocator preallocates a fixed amount of memory beforehand
    to AVOID dynamic memory allocation from the OS, which is in
    general very costly. See below for a class description.
**********************************************************************/
#include "iallocator.h"

namespace Memory {

    //----------------------------------------------------------------------
    // Defines
    //----------------------------------------------------------------------

    #define INITIAL_DESTRUCTOR_LIST_CAPACITY 32

    //**********************************************************************
    // Marks a point in the stack. Used to deallocate
    // everything in a StackAllocator up to this point.
    //**********************************************************************
    class StackAllocatorMarker
    {
    public:
        Byte*   m_address;              // Address to jump back
        Size    m_amountOfDestructors;  // Number of destructors existed at that time

        StackAllocatorMarker(Byte* markerAddress = nullptr, Size curAmountOfDestructors = 0)
            : m_address(markerAddress), m_amountOfDestructors(curAmountOfDestructors)
        {}
    };

    //**********************************************************************
    // Features:
    //  [+] Allocations can be made in any size and any order
    //  [-] Deallocation only possible to a (saved) marker or all at once.
    //      A Marker can be retrieved via a method.
    // Be careful about pointers pointing to memory in this allocator : -)
    //**********************************************************************
    class StackAllocator : public _IAllocator
    {
        //**********************************************************************
        // Necessary to call the destructor of an object.
        //**********************************************************************
        class StackAllocatorDestructor
        {
        public:
            template <typename T>
            explicit StackAllocatorDestructor(const T& data)
                : m_data( std::addressof(data) )
            {
                destructor = [](const void* data) {
                    auto originalType = static_cast<const T*>( data );
                    originalType -> ~T();
                };
            }

            void operator()() { destructor(m_data); }

        private:
            const void* m_data;
            void(*destructor)(const void*);
        };

    public:
        explicit StackAllocator(U32 amountOfBytes, _IParentAllocator* parentAllocator = nullptr);
        ~StackAllocator();

        //----------------------------------------------------------------------
        // Allocate "amountOfObjects" objects of type T.
        // @Params:
        // "amountOfObjects": Amount of objects to allocate (array-allocation)
        // "args": Constructor arguments from the class T
        //----------------------------------------------------------------------
        template <typename T, typename... Args>
        T* allocate(Size amountOfObjects = 1, Args&&... args);

        //----------------------------------------------------------------------
        // Allocate fixed amount of bytes with optionally an alignment.
        // @Params:
        // "amountOfBytes": Amount of bytes to allocate
        // "alignment": Alignment to use. MUST be power of two
        //----------------------------------------------------------------------
        void* allocateRaw(Size amountOfBytes, Size alignment = 1);

        //----------------------------------------------------------------------
        // Clears the whole stack at once.
        //----------------------------------------------------------------------
        void clear();

        //----------------------------------------------------------------------
        // Clears the stack to the given marker.
        // A marker can be retrieved via getMarker()
        //----------------------------------------------------------------------
        void clearToMarker(StackAllocatorMarker marker);

        //----------------------------------------------------------------------
        // Return a marker, which saves the current position of the stack.
        // Can be used to clear the stack up to this point.
        //----------------------------------------------------------------------
        StackAllocatorMarker getMarker() const { return StackAllocatorMarker( m_head, m_destructors.size() ); }

    private:
        Byte* m_head; // Points to next free memory

        // Stores destructors for allocates objects
        std::vector<StackAllocatorDestructor> m_destructors;

        template <typename T>
        inline typename std::enable_if<std::is_trivially_destructible<T>::value>::type
            _AddDestructorToList(T* object)
        { /* Object is trivially destructible, therefore it's not necessary to destruct it later */ }

        template <typename T>
        inline typename std::enable_if<!std::is_trivially_destructible<T>::value>::type
            _AddDestructorToList(T* object)
        {
            m_destructors.push_back( StackAllocatorDestructor( *object ) );
        }

        StackAllocator (const StackAllocator& other)                = delete;
        StackAllocator& operator = (const StackAllocator& other)    = delete;
        StackAllocator(StackAllocator&& other)                      = delete;
        StackAllocator& operator = (StackAllocator&& other)         = delete;
    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    //----------------------------------------------------------------------
    template <typename T, typename... Args>
    T* StackAllocator::allocate( Size amountOfObjects, Args&&... args )
    {
        Size amountOfBytes = amountOfObjects * sizeof(T);
        T* alignedAddress = reinterpret_cast<T*>( allocateRaw( amountOfBytes, alignof(T) ) );

        if (alignedAddress != nullptr)
        {
            for (Size i = 0; i < amountOfObjects; i++)
            {
                T* object = new ( std::addressof( alignedAddress[i] ) ) T( std::forward<Args>(args)... );
                _AddDestructorToList<T>( object );
            }

            return alignedAddress;
        }

        _OutOfMemory();
        return nullptr;
    }


} // end namespaces