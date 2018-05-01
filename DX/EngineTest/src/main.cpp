#include "scenes.hpp"
#define DISPLAY_CONSOLE 1

#ifdef _DEBUG
    const char* gameName = "[DEBUG] EngineTest";
#else
    const char* gameName = "[RELEASE] EngineTest";
#endif

#include "Time/clock.h"

//----------------------------------------------------------------------
// SCENES
//----------------------------------------------------------------------

class TestScene : public IScene
{
public:
    TestScene() : IScene("TestScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto mesh = Core::Assets::MeshGenerator::CreateCubeUV(1);
        mesh->setColors(cubeColors);

        auto mat = ASSETS.getMaterial("/materials/basic.material");

        //auto shader = ASSETS.getShader("/shaders/color.shader");
        //auto material = RESOURCES.createMaterial(shader);
        //material->setTexture("tex", ASSETS.getTexture2D("/textures/checker.jpg"));

        auto go2 = createGameObject("Obj");
        go2->addComponent<Components::MeshRenderer>(mesh, mat);


        LOG("TestScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
    }

    void shutdown() override { LOG("TestScene Shutdown!", Color::RED); }
};

//----------------------------------------------------------------------
// GAME
//----------------------------------------------------------------------

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
        gLogger->setSaveToDisk( false );

        Locator::getEngineClock().setInterval([=] {
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
            String newTitle = String(gameName) + " | Time: " + TS(TIME.getTime().value) + " | Delta: " + TS(delta) + "ms (" + TS(fps) + " FPS)";
            Locator::getWindow().setTitle(newTitle.c_str());
        }, 1000);

        ASSETS.setHotReloading(true);

        Locator::getSceneManager().LoadSceneAsync(new TestScene());
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        //clock.tick( delta );
        //LOG( TS( clock.getTime().value ) );
        //auto time = clock.getTime();

        if (KEYBOARD.wasKeyPressed(Key::One))
            Locator::getSceneManager().LoadSceneAsync(new VertexGenScene);
        if (KEYBOARD.wasKeyPressed(Key::Two))
            Locator::getSceneManager().LoadSceneAsync(new SceneCameras);
        if (KEYBOARD.wasKeyPressed(Key::Three))
            Locator::getSceneManager().LoadSceneAsync(new MaterialTestScene);
        if (KEYBOARD.wasKeyPressed(Key::Four))
            Locator::getSceneManager().LoadSceneAsync(new ManyObjectsScene(10000));
        if (KEYBOARD.wasKeyPressed(Key::Five))
            Locator::getSceneManager().LoadSceneAsync(new CubemapScene());
        if (KEYBOARD.wasKeyPressed(Key::Six))
            Locator::getSceneManager().LoadSceneAsync(new TexArrayScene());
        if (KEYBOARD.wasKeyPressed(Key::Seven))
            Locator::getSceneManager().LoadSceneAsync(new MultiCamera());
        if (KEYBOARD.wasKeyPressed(Key::Zero))
            Locator::getSceneManager().LoadSceneAsync(new TestScene());

        if (KEYBOARD.wasKeyPressed(Key::P))
            PROFILER.logGPU();

        if (KEYBOARD.wasKeyPressed(Key::F1))
            Locator::getRenderer().setGlobalMaterialActive("NONE");
        if (KEYBOARD.wasKeyPressed(Key::F2))
            Locator::getRenderer().setGlobalMaterialActive("Wireframe");

        if (KEYBOARD.wasKeyPressed(Key::M))
            _ChangeMultiSampling();

        if(KEYBOARD.isKeyDown(Key::Escape))
            terminate();
    }

    //----------------------------------------------------------------------
    void _ChangeMultiSampling()
    {
        static int index = 0;
        int mscounts[]{ 1,4,8 };
        int newmscount = mscounts[index];
        index = (index + 1) % (sizeof(mscounts) / sizeof(int));
        Locator::getRenderer().setMultiSampleCount(newmscount);
        LOG("New Multisample-Count: " + TS(newmscount), Color::GREEN);
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
        game.start( gameName, 800, 600 );

        return 0;
    }

#endif