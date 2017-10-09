#include <iostream>

#include "MemoryManagement/memory_management.hpp"
#include "OS/PlatformTimer/platform_timer.h"


#define LOG(x) std::cout << x << std::endl

class AutoClock
{
    I64 begin;

public:
    AutoClock()
    {
        begin = OS::PlatformTimer::getTicks();
    }

    ~AutoClock()
    {
        I64 elapsedTicks = OS::PlatformTimer::getTicks() - begin;

        F64 elapsedSeconds = OS::PlatformTimer::ticksToSeconds(elapsedTicks);
        LOG("Seconds: " + TS(elapsedSeconds));

        F64 elapsedMillis = OS::PlatformTimer::ticksToMilliSeconds(elapsedTicks);
        LOG("Millis: " + TS(elapsedMillis));

        F64 elapsedMicros = OS::PlatformTimer::ticksToMicroSeconds(elapsedTicks);
        LOG("Micros: " + TS(elapsedMicros));

        F64 elapsedNanos = OS::PlatformTimer::ticksToNanoSeconds(elapsedTicks);
        LOG("Nanos: " + TS(elapsedNanos));
    }
};

MemoryManagement::UniversalAllocator gUniversalAllocator(1000);

class A
{
public:
    A() {
        //LOG("Constructor");
    }
    ~A() {
        int i = 25;
        //LOG("Destructor");
    }

    A(const A& other)
    {
        LOG("Copy Constructor");
    }

    A& operator=(const A& other)
    {
        LOG("Assignment Operator");
        return *this;
    }

    A(const A&& other)
    {
        LOG("Move Constructor");
        time = other.time;
    }

    A& operator=(const A&& other)
    {
        LOG("Move Assignment");
        time = other.time;
        return *this;
    }


    //static void* operator new(Size sz)
    //{
    //    std::cout << "custom new for size " << sz << '\n';
    //    return gPoolAllocator.allocateRaw( sz, alignof(A) );
    //}

    //static void operator delete(void* mem)
    //{
    //    std::cout << "custom delete \n";
    //    return gPoolAllocator.deallocate( mem );
    //}

    //static void* operator new[](std::size_t sz)
    //{
    //    std::cout << "custom new[] for size " << sz << '\n';
    //    return gPoolAllocator.allocateRaw(sz, alignof(A));
    //}

    //static void operator delete[](void* mem)
    //{
    //    std::cout << "custom delete[] \n";
    //    return gPoolAllocator.deallocate(mem);
    //}


    SystemTime time;
};

class B : public A
{
public:
    U32 lols;
};

class C
{
    U64 lul;
};

//@TODO: 
// - LOGGER
//  - Logging of memorytracker
// - Make MemoryManager part of a subsystem and call init() ? -> that way static allocations don't count

// - FileSystem
//    -> Configuration-File(s) .ini
// - Profiler

// - refactor parentheses



int main(void)
{
    //StringID te = SID("Hello");
    //StringID te2 = SID("World");
    //String ste = IDS(te);
    //String ste2 = IDS(te2);
    //LOG(ste + " " + ste2);
#define SIZE 100000

    //LOG("MEASURE NEW + DELETE...");
    //static A* a[SIZE];
    //{
    //    AutoClock clock;
    //    
    //    for (int i = 0; i < SIZE; i++)
    //    {
    //        a[i] = new A();
    //    }
    //    for (int i = 0; i < SIZE; i++)
    //    {
    //        delete a[i];
    //    }
    //}

   /* {
        LOG("MEASURE POOL ALLOCATOR...");
        static A* a2[SIZE];
        MemoryManagement::PoolAllocator poolAllocator(sizeof(A), SIZE);
        {
            AutoClock clock;

            for (int i = 0; i < SIZE; i++)
            {
                a2[i] = poolAllocator.allocate<A>();
            }
            for (int i = 0; i < SIZE; i++)
            {
                poolAllocator.deallocate(a2[i]);
            }
        }
    }


    {
        LOG("MEASURE STACK ALLOCATOR...");
        static A* a3[SIZE];
        MemoryManagement::StackAllocator stackAllocator(SIZE * sizeof(A) * 2);
        {
            AutoClock clock;
            for (int i = 0; i < SIZE; i++)
            {
                a3[i] = stackAllocator.allocate<A>();
            }
            stackAllocator.clear();
        }
    }

    {
        MemoryManagement::PoolListAllocator poolListAllocator({ 8, 16, 32, 64, 128, 256 }, 32);

        A* a;
        a = poolListAllocator.allocate<A>();
        poolListAllocator.deallocate(a);

        Size* f = poolListAllocator.allocate<Size>();
        poolListAllocator.deallocate(f);

        void* test = poolListAllocator.allocateRaw(255);
        poolListAllocator.deallocate(test);
    }*/

    {
        MemoryManagement::UniversalAllocatorDefragmented universalDefragmentedAllocator(1000, 100);

        auto a = universalDefragmentedAllocator.allocate<A>(1);
        A* aRaw = a.getRaw();
        universalDefragmentedAllocator.deallocate(a);

        MemoryManagement::UAPtr<B> b = universalDefragmentedAllocator.allocate<B>(1);
        MemoryManagement::UAPtr<A> a2 = b;
        universalDefragmentedAllocator.deallocate(a2);

        auto raw = universalDefragmentedAllocator.allocateRaw(1);
        Byte* r = raw.getRaw();
        universalDefragmentedAllocator.deallocate(raw);

        MemoryManagement::UAPtr<U32> u32 = universalDefragmentedAllocator.allocate<U32>();
        //MemoryManagement::UAPtr<Byte> test = u32;
        universalDefragmentedAllocator.deallocate(u32);


        MemoryManagement::UAPtr<U32> arr[10];
        for (int i = 0; i < 10; i++)
        {
            arr[i] = universalDefragmentedAllocator.allocate<U32>(1);
            *arr[i] = i;
        }
        for (int i = 0; i < 10; i++)
        {
            LOG(*arr[i]);
            universalDefragmentedAllocator.deallocate(arr[i]);
        }


        auto a5 = universalDefragmentedAllocator.allocate<A>();
        //auto rawr = universalDefragmentedAllocator.allocateRaw(100);

        auto a6 = universalDefragmentedAllocator.allocate<A>();

        universalDefragmentedAllocator.deallocate(a5);
        //universalDefragmentedAllocator.deallocate(rawr);

        a6->time = OS::PlatformTimer::getCurrentTime();
        LOG(a6->time.toString());
        std::cout << a6.getRaw() << std::endl;

        // A6 should be shifted
        universalDefragmentedAllocator.defragmentOnce();

        LOG(a6->time.toString());
        std::cout << a6.getRaw() << std::endl;

        universalDefragmentedAllocator.deallocate(a6);
    }

    MemoryManagement::MemoryTracker::log();
    system("pause");
    return 0;
}

