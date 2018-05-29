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
    Components::SpotLight* spot;

    MaterialPtr skyboxMat;

public:
    TestScene() : IScene("TestScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        spot = go->addComponent<Components::SpotLight>(2.0f, Color::RED, 25.0f);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto brdfLut = RESOURCES.createRenderTexture();
        brdfLut->create(512, 512, 0, Graphics::TextureFormat::RGHalf);
        brdfLut->setAnisoLevel(1);
        brdfLut->setFilter(Graphics::TextureFilter::Bilinear);
        brdfLut->setClampMode(Graphics::TextureAddressMode::Clamp);

        Graphics::CommandBuffer cmd;
        cmd.setRenderTarget(brdfLut);
        cmd.drawFullscreenQuad(ASSETS.getMaterial("/materials/pbr_brdfLut.material"));
        Locator::getRenderer().dispatch(cmd);

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png", true);

        auto pbrShader = ASSETS.getShader("/shaders/pbr.shader");

        Assets::EnvironmentMap envMap(cubemap, 256, 1024);
        auto diffuse = envMap.getDiffuseIrradianceMap();
        auto specular = envMap.getSpecularReflectionMap();
        pbrShader->setTexture("diffuseIrradianceMap", envMap.getDiffuseIrradianceMap());
        pbrShader->setTexture("specularReflectionMap", envMap.getSpecularReflectionMap());
        pbrShader->setTexture("brdfLUT", brdfLut);
        pbrShader->setFloat("maxReflectionLOD", (F32)specular->getMipCount() - 1);
        pbrShader->setFloat("BLABLA", (F32)specular->getMipCount() - 1);

        auto mesh = Core::Assets::MeshGenerator::CreateUVSphere(20,20);
        mesh->recalculateNormals();

        auto mat = ASSETS.getMaterial("/materials/pbr.material");

        auto go2 = createGameObject("Obj");
        go2->addComponent<Components::MeshRenderer>(mesh, mat);
        //go2->addComponent<VisualizeNormals>(0.1f, Color::WHITE);
        go2->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90);
        go2->getTransform()->scale = { 1.0f };
        go2->getTransform()->position = { 0, 0, -3 };
        //go2->addComponent<Components::Skybox>(specular);

        skyboxMat = ASSETS.getMaterial("/materials/skyboxLOD.material");
        skyboxMat->setTexture("Cubemap", specular);
        auto skybox = createGameObject("Skybox");
        skybox->getTransform()->scale = { 1000.0f };
        skybox->addComponent<Components::MeshRenderer>(mesh, skyboxMat);

        I32 num = 6;
        F32 distance = 3.0f;
        for (I32 x = 0; x < num; x++)
        {
            F32 roughness = x / (F32)(num - 1);

            for (I32 y = 0; y < 2; y++)
            {
                auto gameobject = createGameObject("Obj");

                auto material = RESOURCES.createMaterial(pbrShader);
                material->setColor("color", Color::WHITE);
                material->setFloat("roughness", roughness);
                F32 metallic = (F32)y;
                material->setFloat("metallic", metallic);

                gameobject->addComponent<Components::MeshRenderer>(mesh, material);
                gameobject->getTransform()->position = Math::Vec3(x * distance - (num/2 * distance), y * distance + 0.01f, 0.0f);
            }
        }

        auto sun = createGameObject("Sun");
        sun->addComponent<Components::DirectionalLight>(1.0f, Color::WHITE);
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        auto pl = createGameObject("PointLight");
        pl->addComponent<Components::PointLight>(10.0f, Color::GREEN);
        pl->getTransform()->position = {4, 2, 0};
        pl->addComponent<Components::Billboard>(ASSETS.getTexture2D("/textures/pointLight.png"), 0.3f);
        pl->addComponent<AutoOrbiting>(20.0f);
        
        F32 intensity = 2.0f;
        auto pl2 = createGameObject("PointLight");
        pl2->addComponent<Components::PointLight>(intensity, Color::WHITE);
        pl2->getTransform()->position = { -5, 3, -3 };
        pl2->addComponent<Components::Billboard>(ASSETS.getTexture2D("/textures/pointLight.png"), 0.3f);

        auto pl3 = createGameObject("PointLight");
        pl3->addComponent<Components::PointLight>(intensity, Color::WHITE);
        pl3->getTransform()->position = { 5, 3, -3 };
        pl3->addComponent<Components::Billboard>(ASSETS.getTexture2D("/textures/pointLight.png"), 0.3f);

        auto pl4 = createGameObject("PointLight");
        pl4->addComponent<Components::PointLight>(intensity, Color::WHITE);
        pl4->getTransform()->position = { -5, -3, -3 };
        pl4->addComponent<Components::Billboard>(ASSETS.getTexture2D("/textures/pointLight.png"), 0.3f);

        auto pl5 = createGameObject("PointLight");
        pl5->addComponent<Components::PointLight>(intensity, Color::WHITE);
        pl5->getTransform()->position = { 5, -3, -3 };
        pl5->addComponent<Components::Billboard>(ASSETS.getTexture2D("/textures/pointLight.png"), 0.3f);

        LOG("TestScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.wasKeyPressed(Key::F))
            spot->setActive(!spot->isActive());

        if (KEYBOARD.isKeyDown(Key::Up))
        {
            skyboxMat->setFloat("lod", skyboxMat->getFloat("lod") + 2.0f * (F32)d );
            LOG(TS(skyboxMat->getFloat("lod")));
        }

        if (KEYBOARD.isKeyDown(Key::Down))
        {
            skyboxMat->setFloat("lod", skyboxMat->getFloat("lod") - 2.0f * (F32)d );
            LOG(TS(skyboxMat->getFloat("lod")));
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
        Locator::getRenderer().setGlobalFloat(SID("gAmbient"), 1.0f);

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