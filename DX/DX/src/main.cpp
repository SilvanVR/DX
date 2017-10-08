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
    }

    A& operator=(const A&& other)
    {
        LOG("Move Assignment");
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

};

//@TODO: 
// - LOGGER
//  - Logging of memorytracker
// - Make MemoryManager part of a subsystem and call init() ? -> that way static allocations don't count

// - FileSystem
//    -> Configuration-File(s) .ini
// - Profiler



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
    }*/


   /* {
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
    }*/

    //{
    //    MemoryManagement::PoolListAllocator poolListAllocator({ 8, 16, 32, 64, 128, 256 }, 32);

    //    A* a;
    //    a = poolListAllocator.allocate<A>();
    //    poolListAllocator.deallocate(a);

    //    Size* f = poolListAllocator.allocate<Size>();
    //    poolListAllocator.deallocate(f);

    //    void* test = poolListAllocator.allocateRaw(257);
    //    poolListAllocator.deallocate(test);
    //}

    //MemoryManagement::


    MemoryManagement::MemoryTracker::log();
    system("pause");
    return 0;
}

