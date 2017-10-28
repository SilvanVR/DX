#include <DX.h>


class Game : public IGame
{

public:
    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        Locator::getLogger().setSaveToDisk( false );

        // Want to call a function every x-milliseconds or after x-millis
        Locator::getMasterClock().setInterval([] {
            LOG( "Time: " + TS( Locator::getMasterClock().getTime() ) );
            //LOG( "FPS: " + TS( Locator::getProfiler().getFPS() ) );
        }, 1000);

        Locator::getMasterClock().setTimeout([] {
            LOG("ONCE");
        }, 1000);
    }

    //----------------------------------------------------------------------
    void tick(F32 delta) override
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


int main()
{
    Game game;
    game.start();

    system("pause");
    return 0;
}