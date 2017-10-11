
#include "Core/OS/PlatformTimer/platform_timer.h"
#include "Core/MemoryManagement/include.hpp"

#include "Core/subsystem_manager.h"

#include "Core/Misc/color.h"
#include "Core/Misc/layer_mask.hpp"

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


// FILTER - channels 
//   -> Bitmask which is enabled

int main(void)
{
    //MemoryManagement::UniversalAllocator gUniversalAllocator(1000);
    //StringID te = SID("Hello");
    //StringID te2 = SID("World");
    //LOG(te.toString() + " " + te2.toString());

    

    Core::SubSystemManager gSubSystemManager;
    gSubSystemManager.init();

    LayerMask ma;
    ma.setBit(3);



    {
        Logging::ILogger& logger = Locator::getLogger();
        logger.setDefaultColor(Color::WHITE);

        //logger.setLogLevel(Logging::LOG_LEVEL_NOT_SO_IMPORTANT);
        auto renderMask = Logging::LOG_CHANNEL_RENDERING;
        auto physicsMask = Logging::LOG_CHANNEL_PHYSICS;

        logger.filterChannels(renderMask | physicsMask);
        //logger.filterChannels(Logging::LOG_CHANNEL_ALL);
        //logger.unfilterChannels(renderMask | physicsMask);

        //auto& mask = logger.getFilterMask();
        //mask.unsetBits(physicsMask);


        LOG( "Hello World" );

        LOG("IMPORTANT", Logging::LOG_LEVEL_IMPORTANT, Color::BLUE);
        LOG("NOT SO IMPORTANT", Logging::LOG_LEVEL_NOT_SO_IMPORTANT);
        LOG("NOT IMPORTANT", Logging::LOG_LEVEL_NOT_IMPORTANT);

        WARN_PHYSICS ( "Hello World" );
        ERROR( "Hello World", Logging::LOG_LEVEL_IMPORTANT );

        LOG_PHYSICS( "Hello Physics", Logging::LOG_LEVEL_IMPORTANT);
        WARN_RENDERING( "Rendering Warning!" );

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
