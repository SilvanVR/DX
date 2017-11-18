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

//@TODO: 
// - In Game console -> Map Name to Functions
// - Profiler
// - Input

class TestObject : public Input::IKeyListener, public Input::IMouseListener
{
    void OnKeyPressed(Key key, KeyMod mod) override
    {
        //LOG( "PRESSED: " + KeyToString(key) );
    }

    void OnMouseMoved(I16 x, I16 y) override
    {
        //LOG("(" + TS(x) + "," + TS(y) + ")", Color::GREEN);
    }

    void OnChar(char c) override
    {
        //static String buffer;
        //if (c == '\b')
        //    buffer = buffer.substr(0, buffer.size() - 1);
        //else
        //    buffer += c;

        //if (buffer.size() > 0)
        //    LOG(buffer);
    }

    void OnMousePressed(MouseKey key, KeyMod mod) override
    {
        if (key == MouseKey::LButton)
            LOG("Left Mouse Down", Color::RED);
        else if (key == MouseKey::MButton)
        {
            LOG("Middle Mouse Down", Color::RED);
            MOUSE.centerCursor();
        }
        else if (key == MouseKey::RButton)
            LOG("Right Mouse Down", Color::RED);
    }

    void OnMouseReleased(MouseKey key, KeyMod mod) override
    {
        if (key == MouseKey::LButton)
            LOG("Left Mouse Up", Color::RED);
        else if (key == MouseKey::MButton)
            LOG("Middle Mouse Up", Color::RED);
        else if (key == MouseKey::RButton)
            LOG("Right Mouse Up", Color::RED);
    }

    void OnMouseWheel(I16 delta) override
    {
        //LOG("WHEEL DELTA: " + TS(delta));
    }
};

class Game : public IGame
{
    const F64 duration = 1000;
    Time::Clock clock;
    TestObject* obj;

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
            LOG("Time: " + TS( Locator::getEngineClock().getTime().value ) + " FPS: " + TS( Locator::getProfiler().getFPS() ) );
        }, 1000);

        getWindow().setCallbackSizeChanged([](U16 w, U16 h) {
            LOG( "New Window-Size: " + TS(w) + "," + TS(h) );
        });
        
        obj = new TestObject();

        auto& mapper = Locator::getInputManager().getActionMapper();
        mapper.attachKeyboardEvent("Fire", Key::E);
        mapper.attachMouseEvent("Fire", MouseKey::LButton);
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        static U64 ticks = 0;
        ticks++;

        clock.tick( delta );
        //LOG( TS( clock.getTime().value ) );
        //auto time = clock.getTime();

        static bool inState = false;
        if ( KEYBOARD.wasKeyPressed( Key::Q ) )
        {
            inState = !inState;
        }
        if (inState)
        {
            F64 axis = Locator::getInputManager().getMouseWheelAxis();
            LOG(TS(axis));
        }

        static bool fpsMode = false;
        if ( KEYBOARD.wasKeyPressed(Key::P) )
        {
            fpsMode = !fpsMode;
            Locator::getInputManager().setFirstPersonMode(fpsMode);
        }
        if (fpsMode)
        {
            I16 x,y;
            MOUSE.getMouseDelta(x,y);
            LOG(TS(x)+ "|" + TS(y));
        }


        if ( ACTION_MAPPER.isKeyDown( "Fire" ) )
        {
            LOG("FIRE!");
        }

        //LOG( "Tick: " + TS(ticks) );
        //if ( ticks == GAME_TICK_RATE * 2)
        //    terminate();
        if( KEYBOARD.isKeyDown( Key::Escape ) )
            terminate();
    }

    //----------------------------------------------------------------------
    void shutdown() override 
    {
        LOG( "Shutdown game..." );  
        delete obj;
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
