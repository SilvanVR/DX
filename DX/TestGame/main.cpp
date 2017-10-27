

#include <DX.h>


class Game : public IGame
{

public:
    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        Locator::getLogger().setSaveToDisk( false );
    }

    //----------------------------------------------------------------------
    void tick(F32 delta) override
    {
        static U64 ticks = 0;

        ticks++;
        LOG( "Tick: " + TS(ticks) );


        if ( ticks == 1000 )
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