#include "Core/OS/PlatformTimer/platform_timer.h"
#include "Core/MemoryManagement/Allocators/include.hpp"
#include "Core/subsystem_manager.h"

#include "Core/OS/FileSystem/file_system.h"
#include "Core/OS/Threading/thread_pool.h"
#include "Core/OS/FileSystem/file.h"
#include "GameInterface/i_game.hpp"
#include "Core/Time/clock.h"
#include "Core/MemoryManagement/Allocators/pool_allocator.hpp"

#include <chrono>
#include <thread>

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

class MyScene2 : public IScene
{
public:
    MyScene2() : IScene("MyScene2") {}

    void init() override
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        LOG("MyScene2 initialized!", Color::RED);
    }

    void shutdown() override
    {
        LOG("MyScene2 Shutdown!", Color::RED);
    }

};

class MyScene : public IScene
{
    GameObject* go;

public:
    MyScene() : IScene("MyScene"){}

    void init() override
    {
        LOG("MyScene initialized!", Color::RED);

        go = createGameObject("Test");

        GameObject* go2 = findGameObject("Test");
        CTransform* c = go2->getComponent<CTransform>();

        bool removed = go2->removeComponent( c );

        //bool destroyed = go2->removeComponent<CTransform>();

        int i = 523;
    }

    void shutdown() override
    {
        LOG("MyScene Shutdown!", Color::RED);
    }

};



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
        getWindow().setCursor( "/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "/internal/icon.ico" );
        Locator::getLogger().setSaveToDisk( false );

        Locator::getEngineClock().setInterval([] {
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
            LOG("Time: " + TS( TIME.getTime().value ) + " FPS: " + TS( fps ) + " Delta: " + TS( delta ) + " ms" );
            //LOG("Num Scenes: " + TS(Locator::getSceneManager().numScenes()));
        }, 1000);

        Locator::getSceneManager().LoadSceneAsync(new MyScene);
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        getWindow().setTitle( PROFILER.getUpdateDelta().toString().c_str() );
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

        if (KEYBOARD.wasKeyPressed(Key::One))
            Locator::getSceneManager().LoadSceneAsync(new MyScene);
        if (KEYBOARD.wasKeyPressed(Key::Two))
            Locator::getSceneManager().LoadSceneAsync(new MyScene2);

        if (KEYBOARD.wasKeyPressed(Key::One))
            Locator::getRenderer().setMultiSampleCount(1);
        if (KEYBOARD.wasKeyPressed(Key::Four))
            Locator::getRenderer().setMultiSampleCount(4);
        if (KEYBOARD.wasKeyPressed(Key::Eight))
            Locator::getRenderer().setMultiSampleCount(8);

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



