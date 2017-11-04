#include "Core/OS/PlatformTimer/platform_timer.h"
#include "Core/MemoryManagement/Allocators/include.hpp"
#include "Core/subsystem_manager.h"

#include "Core/OS/FileSystem/file_system.h"
#include "Core/OS/Threading/thread_pool.h"
#include "Core/OS/FileSystem/file.h"
#include "GameInterface/i_game.hpp"
#include "Core/Time/clock.h"

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

//@TODO: 
// - Profiler
// - Input
// - Window


class Game : public IGame
{
    const F64 duration = 1000;
    Time::Clock clock;

public:
    Game() : clock( duration ) {}

    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        getWindow().setCursor("/cursors/Areo Cursor Red.cur");
        Locator::getLogger().setSaveToDisk( false );

        Locator::getEngineClock().setInterval([] {
            LOG("Time: " + TS( Locator::getEngineClock().getTime().value ) + " FPS: " + TS( Locator::getProfiler().getFPS() ) );
        }, 1000);

        Locator::getEngineClock().setTimeout([this] {
            getWindow().setBorderlessFullscreen(true);
        }, 1000);

        Locator::getEngineClock().setTimeout([this] {
            getWindow().setBorderlessFullscreen(false);
            getWindow().center();
        }, 3000);

    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        static U64 ticks = 0;
        ticks++;

        clock.tick( delta );
        //LOG( TS( clock.getTime().value ) );
        //auto time = clock.getTime();

        //LOG( "Tick: " + TS(ticks) );
        //if ( ticks == GAME_TICK_RATE * 2)
        //    terminate();
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
    game.start( "Awesome Game!", 800, 600 );

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
