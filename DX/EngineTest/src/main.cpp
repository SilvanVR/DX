#include "scenes.hpp"
#include "OS/FileSystem/file.h"
#include "OS/FileSystem/file_system.h"
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
    Components::Camera* cam;
    GameObject* go;
    Components::Transform* t;
    Components::Transform* t2;

    Components::VRCamera* vrCam;

public:
    TestScene() : IScene("TestScene") {}

    void init() override
    {
        // Camera 1
        go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -1);

        //go->addComponent<Components::Camera>();
        //go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);

        vrCam = go->addComponent<Components::VRCamera>(Components::ScreenDisplay::LeftEye, Graphics::MSAASamples::Four);
        go->addComponent<Components::VRFPSCamera>();
        //go->addComponent<Tonemap>();
        //go->addComponent<PostProcess>(ASSETS.getMaterial("/materials/post processing/color_grading.material"));

        //createGameObject("Grid")->addComponent<GridGeneration>(20);

        RENDERER.setGlobalFloat(SID("_Ambient"), 0.1f);

        auto world = createGameObject("World");
        world->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/box_n_inside.obj"), ASSETS.getMaterial("/materials/blinn_phong/cellar.material"));
        world->getTransform()->position.y = 10.0f;
        world->getTransform()->scale = 10.0f;

        auto plg = createGameObject("PL");
        plg->addComponent<Components::PointLight>(2.0f, Color::ORANGE, 15.0f);
        plg->getTransform()->position = { 0, 1.5f, 0 };
        plg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        auto monkey = createGameObject("monkey");
        monkey->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/normals.material"));
        t = monkey->getTransform();
        t->scale = { 0.2f };
        t->position.y = 0.3f;
        monkey->addComponent<ConstantRotation>(0.0f, 15.0f, 0.0f);
        //monkey->addComponent<Components::AudioSource>(ASSETS.getAudioClip("/audio/start_dash.wav"));

        auto ps = createGameObject("Particles!");
        ps->addComponent<Components::ParticleSystem>("/particles/ambient.ps");

        //go->addComponent<Components::GUI>();
        //go->addComponent<Components::GUIFPS>();
        //go->addComponent<Components::GUICustom>([=] {
        //    static Math::Vec3 deg{ 0.0f, 0.0f, 0.0f };
        //    if (ImGui::SliderFloat3("Rotation", &deg.x, 0.0f, 360.0f))
        //        t->rotation = Math::Quat::FromEulerAngles(deg);

        //    static Math::Vec3 pos{ 0.0f, 0.0f, 0.0f };
        //    if (ImGui::SliderFloat3("Position", &pos.x, -3.0f, 3.0f))
        //        t->position = pos;
        //});

        LOG("TestScene initialized!", Color::RED);
    }

    void tick(Time::Seconds delta) override
    {
        if (KEYBOARD.wasKeyPressed(Key::F))
            DEBUG.drawCube({ -1,-1,-1 }, { 1,1,1 }, Color::RED, 10_s);

        if (KEYBOARD.wasKeyPressed(Key::M))
        {
            static int index = 0;
            U32 mscounts[]{ 1,2,4,8 };
            U32 newmscount = mscounts[index];
            index = (index + 1) % (sizeof(mscounts) / sizeof(U32));
            cam->setMultiSamples((Graphics::MSAASamples) newmscount);
            LOG("New Multisample-Count: " + TS(newmscount), Color::GREEN);
        }
    }

    void shutdown() override { LOG("TestScene Shutdown!", Color::RED); }
};

//----------------------------------------------------------------------
class SceneGUISelectSceneMenu : public IScene
{
public:
    SceneGUISelectSceneMenu() : IScene("SceneGUISelectSceneMenu") {}

    void init() override
    {
        auto gui = createGameObject("GUI");
        gui->addComponent<Components::Camera>();
        gui->addComponent<Components::GUI>();

        auto guiSceneMenu = gui->addComponent<GUISceneMenu>();
        guiSceneMenu->registerScene<TestScene>("Test Scene");
        guiSceneMenu->registerScene<SceneParticleSystem>("Particle System");
        guiSceneMenu->registerScene<ShadowScene>("Shadow Scene");
        guiSceneMenu->registerScene<ScenePostProcessMultiCamera>("Multi Camera Post Processing");
        guiSceneMenu->registerScene<SceneGUI>("GUI Example");
        guiSceneMenu->registerScene<SceneMirror>("Offscreen rendering on material");
        guiSceneMenu->registerScene<ManyObjectsScene>("Many Drawcalls!");
        guiSceneMenu->registerScene<BlinnPhongLightingScene>("Blinn-Phong Lighting");
        guiSceneMenu->registerScene<ScenePBRSpheres>("PBR Spheres");
        guiSceneMenu->registerScene<ScenePBRPistol>("PBR Pistol + Dagger");
        guiSceneMenu->registerScene<SponzaScene>("Sponza");
        guiSceneMenu->registerScene<TransparencyScene>("Ordered Transparency");
        guiSceneMenu->registerScene<SceneGraphScene>("SceneGraph");
        guiSceneMenu->registerScene<BRDFLUTScene>("BRDFLut");
        guiSceneMenu->registerScene<SceneFrustumVisualization>("Frustum Visualization");
        guiSceneMenu->registerScene<TexArrayScene>("Texture arrays");
        guiSceneMenu->registerScene<CubemapScene>("Cubemap");
        guiSceneMenu->registerScene<VertexGenScene>("Dynamic Vertex Buffer regeneration");

        LOG("SceneGUISelectSceneMenu initialized!", Color::RED);
    }
    void shutdown() override { LOG("SceneGUISelectSceneMenu Shutdown!", Color::RED); }
};

//----------------------------------------------------------------------
// GAME
//----------------------------------------------------------------------

class Game : public IGame
{
public:
    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        getWindow().setCursor( "/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "/engine/icon.ico" );
        gLogger->setSaveToDisk( false );

        Locator::getEngineClock().setInterval([=] {
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
            String api = Locator::getRenderer().getAPIName();
            String title = StringUtils::format("%s | API: %s | Time: %.0fs | Delta: %fms (%d FPS)", gameName, api.c_str(), TIME.getTime().value, delta, fps);
            Locator::getWindow().setTitle(title.c_str());
        }, 1000);

        ASSETS.setHotReloading(true);

        IGC_REGISTER_COMMAND_WITH_NAME( "menu", BIND_THIS_FUNC_0_ARGS(&Game::_OpenMenu) );

        Locator::getRenderer().setVSync(true);
        Locator::getRenderer().setGlobalFloat(SID("_Ambient"), 0.5f);

        //Locator::getSceneManager().LoadSceneAsync(new SceneGUISelectSceneMenu());
        Locator::getSceneManager().LoadSceneAsync(new TestScene());
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        if (KEYBOARD.wasKeyPressed(Key::F5))
            _OpenMenu();

        if (KEYBOARD.wasKeyPressed(Key::Zero))
            Locator::getSceneManager().LoadSceneAsync(new TestScene());

        if (KEYBOARD.wasKeyPressed(Key::P))
            PROFILER.logGPU();

        if (KEYBOARD.wasKeyPressed(Key::F1))
            Locator::getRenderer().setGlobalMaterialActive("NONE");
        if (KEYBOARD.wasKeyPressed(Key::F2))
            Locator::getRenderer().setGlobalMaterialActive("Wireframe");

        // VR
        {
            // Toggle between normal & vr camera
            if (KEYBOARD.wasKeyPressed(Key::V))
            {
                auto mainCamera = SCENE.getMainCamera();
                if (not mainCamera->isRenderingToHMD())
                {
                    bool isActive = mainCamera->isActive();
                    mainCamera->setActive(not isActive);

                    auto go = mainCamera->getGameObject();
                    if (auto fpsMovScript = go->getComponent<Components::FPSCamera>())
                        fpsMovScript->setActive(not isActive);

                    auto vrCamera = go->getComponent<Components::VRCamera>();
                    if (not vrCamera)
                    {
                        go->getTransform()->rotation.x = go->getTransform()->rotation.z = 0;
                        go->addComponent<Components::VRCamera>(Components::ScreenDisplay::LeftEye);
                        go->addComponent<Components::VRFPSCamera>();
                        //go->addComponent<PostProcess>(ASSETS.getMaterial("/materials/post processing/color_grading.material"));
                    }
                    else
                    {
                        go->removeComponent<Components::VRCamera>();
                        go->removeComponent<Components::VRFPSCamera>();
                    }
                }
            }

            static I32 perfHudMode = 0;
            if (KEYBOARD.wasKeyPressed(Key::Left))
            {
                perfHudMode = perfHudMode - 1; if (perfHudMode < 0) perfHudMode = (I32)Graphics::VR::PerfHudMode::Count - 1;
                RENDERER.getVRDevice().setPerformanceHUD((Graphics::VR::PerfHudMode)perfHudMode);
            }
            if (KEYBOARD.wasKeyPressed(Key::Right))
            {
                perfHudMode = (perfHudMode + 1) % (I32)Graphics::VR::PerfHudMode::Count;
                RENDERER.getVRDevice().setPerformanceHUD((Graphics::VR::PerfHudMode)perfHudMode);
            }

            // Change world scale
            if (KEYBOARD.isKeyDown(Key::Up))
                RENDERER.getVRDevice().setWorldScale(RENDERER.getVRDevice().getWorldScale() + 1.0f * (F32)delta);
            if (KEYBOARD.isKeyDown(Key::Down))
                RENDERER.getVRDevice().setWorldScale(RENDERER.getVRDevice().getWorldScale() - 1.0f * (F32)delta);
        }

        if(KEYBOARD.isKeyDown(Key::Escape))
            terminate();
    }

    //----------------------------------------------------------------------
    void shutdown() override 
    {
        LOG( "Shutdown game..." );
    }

private:
    //----------------------------------------------------------------------
    void _OpenMenu() const
    {
        Locator::getSceneManager().LoadSceneAsync(new SceneGUISelectSceneMenu);
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
