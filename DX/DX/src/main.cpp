#include "Core/OS/PlatformTimer/platform_timer.h"
#include "Core/MemoryManagement/include.hpp"
#include "Core/subsystem_manager.h"

#include "Core/OS/FileSystem/file_system.h"
#include "Core/OS/Threading/thread_pool.h"
#include "Core/OS/FileSystem/file.h"
#include "GameInterface/i_game.hpp"

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
// - Profiler
// - Input

// - CoreEngine
// - Clock
// - Game Class
// - Window



class Game : public IGame
{

public:
    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        Locator::getLogger().setSaveToDisk( false );

        //// Want to call a function every x-milliseconds or after x-millis
        //Locator::getMasterClock().setInterval([] {
        //    //LOG( "Time: " + TS( Locator::getMasterClock().getTime() ) );
        //    LOG("Hello World!");
        //}, 1000);

        Locator::getMasterClock().setTimeout([] {
            LOG("ONCE");
        }, 1000);

   /*     Locator::getMasterClock().setTimeout([this] {
            terminate();
        }, 3000);*/
    }

    //----------------------------------------------------------------------
    void tick(F32 delta) override
    {
        static U64 ticks = 0;
        ticks++;
        //LOG( "Tick: " + TS(ticks) );

        if ( ticks == GAME_TICK_RATE * 1.1f)
            terminate();
    }

    //----------------------------------------------------------------------
    void shutdown() override 
    {
        LOG( "Shutdown game..." );
    }
};


int main()
{
    Game game;
    game.start();

    system("pause");
    return 0;
}


//int main(void)
//{
//    Core::SubSystemManager gSubSystemManager;
//    gSubSystemManager.init();
//    Locator::getLogger().setSaveToDisk( false );
//
//    {
//
//
//
//    }
//
//    {
//        //AutoClock clock;
//
//        //LOG( "Hello World" );
//        //LOG( "IMPORTANT", Logging::ELogLevel::IMPORTANT, Color::BLUE );
//        //WARN( "NOT SO IMPORTANT", Logging::ELogLevel::NOT_SO_IMPORTANT );
//
//        //LOG( SID("Hello") );
//        //Color color( 16, 52, 128, 255);
//        //LOG( color.toString(true), Color::RED );
//    }
//
//    gSubSystemManager.shutdown();
//    system("pause");
//
//    return 0;
//}
