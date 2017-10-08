#pragma once

/**********************************************************************
    class: StackAllocator (stack_allocator.hpp)

    author: S. Hau
    date: October 4, 2017

    StackAllocator interface. A stack-allocator preallocates a
    fixed amount of memory beforehand to AVOID dynamic
    memory allocation from the OS, which is in general very costly.
    Features:
     - All allocations are done on the top of the stack
     - Therefore all allocations have to be freed in reversed order OR
     - Using an (saved) Marker allows to free to a specific point 
     - You can free the whole memory at once
    Be careful about pointers pointing to memory in this allocator :-)
**********************************************************************/

#include "iallocator.hpp"

namespace MemoryManagement
{

    #define INITIAL_DESTRUCTOR_LIST_CAPACITY 32

    //**********************************************************************
    // Marks a point in the stack, to which can be jumped back.
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

    //----------------------------------------------------------------------
    class StackAllocator : public IAllocator
    {
        //**********************************************************************
        // Necessary to call the destructor of an object.
        //**********************************************************************
        class StackAllocatorDestructor
        {
        public:
            template <class T>
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
        explicit StackAllocator(U32 amountOfBytes, IParentAllocator* parentAllocator = nullptr);
        ~StackAllocator();

        //----------------------------------------------------------------------
        // Allocate "amountOfObjects" objects of type T.
        // @Params:
        // "amountOfObjects": Amount of objects to allocate (array-allocation)
        // "args": Constructor arguments from the class T
        //----------------------------------------------------------------------
        template <class T, class... Args>
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

        template <class T>
        inline typename std::enable_if<std::is_trivially_destructible<T>::value>::type
            _AddDestructorToList(T* object)
        { /* Object is trivially destructible, therefore it's not necessary to destruct it later */ }

        template <class T>
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
    StackAllocator::StackAllocator(U32 amountOfBytes, IParentAllocator* parentAllocator)
        : IAllocator( amountOfBytes, parentAllocator )
    {
        ASSERT( m_amountOfBytes > 0 );

        m_data = reinterpret_cast<Byte*>( m_parentAllocator->allocateRaw( m_amountOfBytes ) );
        ASSERT( m_data != nullptr );

        m_head = m_data;

        m_destructors.reserve( INITIAL_DESTRUCTOR_LIST_CAPACITY );
    }

    //----------------------------------------------------------------------
    StackAllocator::~StackAllocator()
    {
        m_head = nullptr;
    }

    //----------------------------------------------------------------------
    void* StackAllocator::allocateRaw(Size amountOfBytes, Size alignment)
    {
        Byte* alignedAddress = alignAddress( m_head, alignment );
        Byte* newHeadPointer = alignedAddress + amountOfBytes;

        bool hasEnoughSpace = ( newHeadPointer <= (m_data + m_amountOfBytes) );
        if (hasEnoughSpace)
        {
            m_head = newHeadPointer;

            _LogAllocatedBytes( amountOfBytes );
            return alignedAddress;
        }

        _OutOfMemory();
        return nullptr;
    }

    //----------------------------------------------------------------------
    template <class T, class... Args>
    T* StackAllocator::allocate(Size amountOfObjects, Args&&... args)
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

    //----------------------------------------------------------------------
    void StackAllocator::clear()
    {
        m_head = m_data;
        for (auto& destructor : m_destructors)
            destructor();
        m_destructors.clear();

        _LogDeallocatedBytes( getMemoryInfo().currentBytesAllocated );
    }

    //----------------------------------------------------------------------
    void StackAllocator::clearToMarker(StackAllocatorMarker marker)
    {
        // Marker address could be null OR the stack was already cleared behind the marker
        ASSERT( marker.m_address != nullptr && marker.m_address < m_head && "Marker was invalid" );
        Size amountOfBytes = (m_head - marker.m_address);

        m_head = marker.m_address;
        while (m_destructors.size() > marker.m_amountOfDestructors)
        {
            m_destructors.back()();
            m_destructors.pop_back();
        }

        _LogDeallocatedBytes( amountOfBytes );
    }



}