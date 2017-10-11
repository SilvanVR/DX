
#include "Core/OS/PlatformTimer/platform_timer.h"
#include "Core/MemoryManagement/include.hpp"

#include "Core/subsystem_manager.h"

#include "Core/Misc/color.h"

using namespace Core;

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


    OS::SystemTime time;
};

class B : public A
{
public:
    U32 lols;
};

//@TODO: 
// - Make MemoryManager part of a subsystem and call init() ? -> that way static allocations don't count

// - Profiler

// - FileSystem
//    -> Configuration-File(s) .ini

// - LOGGER
//  - Logging of memorytracker


int main(void)
{
    //MemoryManagement::UniversalAllocator gUniversalAllocator(1000);
    //StringID te = SID("Hello");
    //StringID te2 = SID("World");
    //LOG(te.toString() + " " + te2.toString());

    Core::SubSystemManager gSubSystemManager;
    gSubSystemManager.init();

    {
       Logging::ILogger& logger = Locator::getLogger();

        StringID hay = StringID( "STRING LUL" );
        LOG( hay );

        LOG( "Hello World" );
        WARN( "Hello World" );
        LOG( "Hello World", Logging::LOG_LEVEL_IMPORTANT );
        LOG( "Hello World", Color::BLUE );

#define LOG_PHYSICS(...) LOG( __VA_ARGS__ )
        LOG_PHYSICS( "Hello Physics" ); // ??

        //logger.log( "Hello World", Logging::LOG_LEVEL_IMPORTANT, Logging::LOG_SOURCE_DEFAULT );
        //logger.log( "Hello World", Logging::LOG_LEVEL_IMPORTANT, Logging::LOG_SOURCE_DEFAULT, Color::RED );
        //logger.log( "Hello World", Color::RED );

        LOG( 2512 );

        //AutoClock clock;
        LOG( SID("Hello") );

        Color color( 16, 52, 128, 255);
        //LOG( color.toString(true), Color::RED );
    }


    MemoryManagement::MemoryTracker::log();
    gSubSystemManager.shutdown();
    system("pause");

    return 0;
}
