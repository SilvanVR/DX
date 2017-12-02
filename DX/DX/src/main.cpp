#include "Core/OS/PlatformTimer/platform_timer.h"
#include "Core/MemoryManagement/Allocators/include.hpp"
#include "Core/subsystem_manager.h"

#include "Core/OS/FileSystem/file_system.h"
#include "Core/OS/Threading/thread_pool.h"
#include "Core/OS/FileSystem/file.h"
#include "GameInterface/i_game.hpp"
#include "Core/Time/clock.h"
#include "Core/MemoryManagement/Allocators/pool_allocator.hpp"

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


class Game : public IGame
{
    const F64 duration = 1000;
    Time::Clock clock;

public:
    Game() : clock( duration ) {}

    void hello()
    {
        LOG("Hello World", Color::RED);
    }

    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        getWindow().setCursor( "/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "/internal/icon.ico" );
        Locator::getLogger().setSaveToDisk( false );

        Locator::getEngineClock().setInterval([] {
            U32 fps = Locator::getProfiler().getFPS();
            F64 delta = (1.0 / fps) * 1000.0;
            LOG("Time: " + TS( Locator::getEngineClock().getTime().value ) + " FPS: " + TS( fps ) + " Delta: " + TS( delta ) + " ms" );
        }, 1000);

        IGC_REGISTER_COMMAND_WITH_NAME( "Hello", std::bind(&Game::hello, this) );
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        getWindow().setTitle(  Locator::getProfiler().getUpdateDelta().toString().c_str() );
        static U64 ticks = 0;
        ticks++;

        clock.tick( delta );
        //LOG( TS( clock.getTime().value ) );
        //auto time = clock.getTime();

        static bool inState = false;
        if (KEYBOARD.wasKeyPressed(Key::Q))
        {
            inState = !inState;
        }

        if (inState)
        {

        }

        if( KEYBOARD.isKeyDown( Key::Escape ) )
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
    game.start( "Awesome Game!", 800, 600 );

    system("pause");
    return 0;
}



