#include "scenes.hpp"
#include "OS/FileSystem/file.h"
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

        //createGameObject("Grid")->addComponent<GridGeneration>(20);

        //auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
        //    "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
        //    "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png", true);

        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/malibu.hdr", 2048, true);

        auto pbrShader = ASSETS.getShader("/shaders/pbr.shader");

        Assets::BRDFLut brdfLut;
        Assets::EnvironmentMap envMap(cubemapHDR, 64, 512);
        auto diffuse = envMap.getDiffuseIrradianceMap();
        auto specular = envMap.getSpecularReflectionMap();
        pbrShader->setTexture("diffuseIrradianceMap", diffuse);
        pbrShader->setTexture("specularReflectionMap", specular);
        pbrShader->setTexture("brdfLUT", brdfLut.getTexture());
        pbrShader->setFloat("maxReflectionLOD", F32(specular->getMipCount()-1));

        createGameObject("Skybox")->addComponent<Components::Skybox>(cubemapHDR);

        auto mesh = ASSETS.getMesh("/models/pistol.fbx");

        auto go2 = createGameObject("Obj");
        auto mr = go2->addComponent<Components::MeshRenderer>(mesh, ASSETS.getMaterial("/materials/pbr/pistol.pbrmaterial"));

        go2->getTransform()->scale = { 0.1f };
        //go2->addComponent<VisualizeNormals>(0.3f, Color::WHITE);
        //go2->getTransform()->position = { 0, -mesh->getBounds().getHeight() * 0.5f, 0 };
        go2->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, -90.0f);
        go2->getTransform()->rotation *= Math::Quat(Math::Vec3::UP, -90.0f);

        // LIGHTS
        auto sun = createGameObject("Sun");
        sun->addComponent<Components::DirectionalLight>(1.0f, Color::WHITE);
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        auto pl = createGameObject("PointLight");
        pl->addComponent<Components::PointLight>(10.0f, Color::WHITE, 20.0f);
        pl->getTransform()->position = {4, 2, 0};
        pl->addComponent<Components::Billboard>(ASSETS.getTexture2D("/textures/pointLight.png"), 0.3f);
        pl->addComponent<AutoOrbiting>(20.0f);

        //auto planeMat = ASSETS.getMaterial("/materials/texture.material");
        //planeMat->setTexture("tex0", ASSETS.getTexture2D("/textures/cubemaps/malibu.hdr"));
        //auto plane = createGameObject("Plane");
        //plane->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(1), planeMat);
        //plane->getTransform()->position = { 0, 2, 0 };

        LOG("TestScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.isKeyDown(Key::Up))
        {
        }
        if (KEYBOARD.isKeyDown(Key::Down))
        {
        }
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

        Locator::getRenderer().setVSync(true);
        Locator::getRenderer().setGlobalFloat(SID("gAmbient"), 0.5f);

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
        if (KEYBOARD.wasKeyPressed(Key::Eight))
            Locator::getSceneManager().LoadSceneAsync(new TransparencyScene());
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