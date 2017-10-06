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

class A
{
public:
    A() {
        LOG("Constructor");
    }
    ~A() {
        int i = 25;
        LOG("Destructor");
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

    SystemTime time;
    std::string fr = "Franz";
};

class B : public A
{

};

//@TODO: 
// - USE Global NEW with those allocators
// - Measure Memory Information from all allocators in one place
// - Parent-Allocator
// - Vector optimization in stack-allocator, general-purpose (vector pulls from global new?)

// - MemoryPoolAllocator - Several Pools of different sizes => allocates from pool closest do needed size


int main(void)
{
    //StringID te = SID("Hello");
    //StringID te2 = SID("World");
    //String ste = IDS(te);
    //String ste2 = IDS(te2);
    //LOG(ste + " " + ste2);
#define SIZE 100

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

    //LOG("MEASURE POOL ALLOCATOR...");
    //static A* a2[SIZE];
    //MemoryManagement::PoolAllocator<A> poolAllocator(SIZE);
    //{
    //    AutoClock clock;

    //    for (int i = 0; i < SIZE; i++)
    //    {
    //        a2[i] = poolAllocator.allocate();
    //    }
    //    for (int i = 0; i < SIZE; i++)
    //    {
    //        poolAllocator.deallocate(a2[i]);
    //    }
    //}

 /*   LOG("MEASURE STACK ALLOCATOR...");
    static A* a3[SIZE];
    MemoryManagement::StackAllocator stackAllocator(SIZE * sizeof(A) * 2);
    {
        AutoClock clock;
        for (int i = 0; i < SIZE; i++)
        {
            a3[i] = stackAllocator.allocate<A>();
        }
        stackAllocator.clearAll();
    }*/

    MemoryManagement::GeneralPurposeAllocator gGeneralAllocator(1024);

    A* a = gGeneralAllocator.allocate<A>(1);


    gGeneralAllocator.deallocate(a);

    //gGeneralAllocator.deallocate(a);


    SystemTime curTime = OS::PlatformTimer::getCurrentTime();
    LOG(curTime.toString());

    system("pause");
    return 0;
}

