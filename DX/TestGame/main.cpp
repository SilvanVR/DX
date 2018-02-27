#include <DX.h>


#define DISPLAY_CONSOLE 1

class Game : public IGame
{

public:
    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        gLogger.setSaveToDisk( false );

        // Want to call a function every x-milliseconds or after x-millis
        Locator::getEngineClock().setInterval([] {
           LOG( "Time: " + TS( Locator::getEngineClock().getTime().value ) + " FPS: " + TS( Locator::getProfiler().getFPS() ) );
        }, 1000);

        getWindow().setCursor( "../dx/res/internal/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "../dx/res/internal/icon.ico" );
    }

    //----------------------------------------------------------------------
    void tick(Core::Time::Seconds delta) override
    {
        static U64 ticks = 0;

        ticks++;
        //LOG( "Tick: " + TS(ticks) );

        if ( ticks == GAME_TICK_RATE * 100.1f)
            terminate();
    }

    //----------------------------------------------------------------------
    void shutdown() override 
    {
        LOG( "Shutdown game..." );
    }
};

#if DISPLAY_CONSOLE

    int main()
    {
        Game game;
        game.start( "Awesome Game!", 800, 600 );

        system("pause");
        return 0;
    }

#else

    #undef ERROR
    #include <Windows.h>

    int APIENTRY WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPTSTR    lpCmdLine,
        int       nCmdShow)
    {
        Game game;
        game.start("Awesome Game!", 800, 600);

        return 0;
    }

#endif