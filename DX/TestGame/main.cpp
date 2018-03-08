#include <DX.h>

#define DISPLAY_CONSOLE 1

class MyScene : public IScene
{
    GameObject*             go;
    Components::Camera*     cam;

public:
    MyScene() : IScene("MyScene"){}

    void init() override
    {
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,0,-10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
    }

    void shutdown() override
    {
    }
};

class Game : public IGame
{
    GameObject* go;
    Components::Camera* cam;

public:
    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        gLogger->setSaveToDisk( false );

        // Want to call a function every x-milliseconds or after x-millis
        Locator::getEngineClock().setInterval([] {
           LOG( "Time: " + TS( Locator::getEngineClock().getTime().value ) + " FPS: " + TS( Locator::getProfiler().getFPS() ) );
        }, 1000);

        getWindow().setCursor( "../dx/res/internal/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "../dx/res/internal/icon.ico" );
        Locator::getSceneManager().LoadSceneAsync( new MyScene() );
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
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

#ifdef _DEBUG
        const char* gameName = "[DEBUG] Test Game";
#else
        const char* gameName = "[RELEASE] Test Game";
#endif

        game.start( gameName, 800, 600 );

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