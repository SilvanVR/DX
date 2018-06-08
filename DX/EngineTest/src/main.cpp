#include "scenes.hpp"
#include "OS/FileSystem/file.h"
#define DISPLAY_CONSOLE 1

#ifdef _DEBUG
    const char* gameName = "[DEBUG] EngineTest";
#else
    const char* gameName = "[RELEASE] EngineTest";
#endif

#include "Time/clock.h"

class ColorGrading : public Components::IComponent
{
    MaterialPtr material;
    Graphics::CommandBuffer cmd;

public:
    ColorGrading() {}

    void addedToGameObject(GameObject* go)
    {
        auto cam = go->getComponent<Components::Camera>();
        auto shader = ASSETS.getShader("/shaders/post processing/color_grading.shader");

        material = RESOURCES.createMaterial(shader);
        material->setColor("color", Color::RED);

        //material->setTexture("depthBuffer", ??); // How to set depth-buffer?
        material->setTexture("sceneBuffer", cam->getRenderTarget());

        // Tell the engine to render a fullscreenquard into a new render-texture with the shader above
        auto rt = RESOURCES.createRenderTexture();
        rt->create(WINDOW.getWidth(), WINDOW.getHeight(), 0, Graphics::TextureFormat::RGBA32);
        rt->setDynamicScreenScale(true);
        rt->setClampMode(Graphics::TextureAddressMode::Clamp);

        // Apply post processing
        cmd.blit(PREVIOUS_BUFFER, rt, material);
        cmd.blit(rt, SCREEN_BUFFER, material);

        // Attach command buffer to camera
        cam->addCommandBuffer(&cmd);
    }

    void tick(Time::Seconds delta) override
    {
    }
};

class GreyScale : public Components::IComponent
{
    MaterialPtr material;
    Graphics::CommandBuffer cmd;

public:
    GreyScale() {}

    void addedToGameObject(GameObject* go)
    {
        auto cam = go->getComponent<Components::Camera>();
        auto shader = ASSETS.getShader("/shaders/post processing/greyscale.shader");

        material = RESOURCES.createMaterial(shader);

        auto rt = RESOURCES.createRenderTexture();
        rt->create(WINDOW.getWidth(), WINDOW.getHeight(), 0, Graphics::TextureFormat::RGBA32);
        rt->setDynamicScreenScale(true);

        // Apply post processing
        cmd.blit(PREVIOUS_BUFFER, rt, material);

        // Attach command buffer to camera
        cam->addCommandBuffer(&cmd);
    }
};

class GaussianBlur : public Components::IComponent
{
    Graphics::CommandBuffer cmd;
    MaterialPtr horizontalBlur;
    MaterialPtr verticalBlur;

public:
    GaussianBlur() {}

    void addedToGameObject(GameObject* go)
    {
        // Create rendertarget
        auto rt = RESOURCES.createRenderTexture();
        rt->create(WINDOW.getWidth(), WINDOW.getHeight(), 0, Graphics::TextureFormat::RGBA32);
        rt->setDynamicScreenScale(true);

        auto rt2 = RESOURCES.createRenderTexture();
        rt2->create(WINDOW.getWidth(), WINDOW.getHeight(), 0, Graphics::TextureFormat::RGBA32);
        rt2->setDynamicScreenScale(true);

        horizontalBlur = ASSETS.getMaterial("/materials/post processing/gaussian_blur_horizontal.material");
        verticalBlur = ASSETS.getMaterial("/materials/post processing/gaussian_blur_vertical.material");

        // Apply post processing
        cmd.blit(PREVIOUS_BUFFER, rt, horizontalBlur);
        cmd.blit(rt, rt2, verticalBlur);

        // Attach command buffer to camera
        auto cam = go->getComponent<Components::Camera>();
        cam->addCommandBuffer(&cmd);
    }
};

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
        cam->setHDRRendering(true);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        go->addComponent<ColorGrading>();
        //go->addComponent<GreyScale>();

        //cam->getViewport().width = 0.5f;
        //auto go2 = createGameObject("Camera2");
        //auto cam2 = go2->addComponent<Components::Camera>();
        //cam2->getViewport().width = 0.5f;
        //cam2->getViewport().topLeftX = 0.5f;
        //go2->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -4);
        //go2->addComponent<GaussianBlur>();

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto mesh = ASSETS.getMesh("/models/sphere.obj");

        auto obj = createGameObject("Obj");
        obj->addComponent<Components::MeshRenderer>(mesh, ASSETS.getMaterial("/materials/texture.material"));

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
            Locator::getSceneManager().LoadSceneAsync(new MultiCamera());
        if (KEYBOARD.wasKeyPressed(Key::Seven))
            Locator::getSceneManager().LoadSceneAsync(new TransparencyScene());
        if (KEYBOARD.wasKeyPressed(Key::Eight))
            Locator::getSceneManager().LoadSceneAsync(new SponzaScene());
        if (KEYBOARD.wasKeyPressed(Key::Nine))
            Locator::getSceneManager().LoadSceneAsync(new ScenePBRPistol());
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