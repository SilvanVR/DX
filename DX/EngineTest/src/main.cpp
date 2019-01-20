#include "scenes.hpp"
#include "thesis_scenes.hpp"
#define DISPLAY_CONSOLE 1

#ifdef _DEBUG
    const char* gameName = "[DEBUG] EngineTest";
#else
    const char* gameName = "[RELEASE] EngineTest";
#endif

//----------------------------------------------------------------------
// TEST SCENE
//----------------------------------------------------------------------
class TestScene : public IScene
{
    Components::Camera* cam;
    GameObject* go;
    GameObject* cubeGO;
    Components::GUI* gui;

public:
    TestScene() : IScene("TestScene") {}

    void init() override
    {
        // Camera
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>( 45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Four );
        cam->setClearColor(Color(175, 181, 191));
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -5);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        go->addComponent<Components::AudioListener>();

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto cubeMesh = Core::MeshGenerator::CreateCubeUV(1.0f);
        cubeGO = createGameObject("Cube");
        cubeGO->addComponent<Components::MeshRenderer>(cubeMesh, ASSETS.getMaterial("/materials/texture.material"));
        cubeGO->addComponent<ConstantRotation>(0.0f, 15.0f, 0.0f);

        gui = go->addComponent<Components::GUI>();
        gui->setActive(false);
        go->addComponent<Components::GUICustom>([=] {
            ImGui::Begin("FOV change", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            static F32 fov = 45.0f;
            if (ImGui::SliderFloat("FOV", &fov, 1.0f, 180.0f))
                cam->setFOV(fov);
            ImGui::End();
        });


        auto sun = createGameObject("Sun");
        auto dl = sun->addComponent<Components::DirectionalLight>(0.5f, Color::WHITE, Graphics::ShadowType::None, ArrayList<F32>{20.0f, 40.0f, 80.0f, 200.0f});
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 }, Math::Vec3{ 0, 0, 1 });
        dl->setShadowMapQuality(Graphics::ShadowMapQuality::Insane);
    }

    void tick(Time::Seconds delta) override
    {
        if (KEYBOARD.wasKeyPressed(Key::M))
        {
            static int index = 0;
            U32 mscounts[]{ 1,2,4,8 };
            U32 newmscount = mscounts[index];
            index = (index + 1) % (sizeof(mscounts) / sizeof(U32));
            cam->setMultiSamples((Graphics::MSAASamples) newmscount);
            LOG("New Multisample-Count: " + TS(newmscount), Color::GREEN);
        }

        if (KEYBOARD.wasKeyPressed(Key::G))
            gui->setActive(!gui->isActive());

        static F32 ss = 1.0f;
        if (KEYBOARD.wasKeyPressed(Key::Up))
        {
            ss += 0.1f;
            cam->setSuperSampling(ss);
            LOG("Screen-Res Mod: %f", ss);
        }
        else if (KEYBOARD.wasKeyPressed(Key::Down))
        {
            ss = std::max(ss - 0.1f, 0.1f);
            cam->setSuperSampling(ss);
            LOG("Screen-Res Mod: %f", ss);
        }

        Time::Milliseconds t = RENDERER.getGPUTimeQuery(SID("Camera#0"));
        LOG("Draw-Time: %fms", t.value);
    }

    void shutdown() override {}
};

//----------------------------------------------------------------------
class SceneGUISelectSceneMenu : public IScene
{
public:
    SceneGUISelectSceneMenu() : IScene("SceneGUISelectSceneMenu") {}

    void init() override
    {
        auto gui = createGameObject("GUI");
        gui->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);
        gui->addComponent<Components::GUI>();

        // Add a draw-call, otherwise v-ez won't clear the screen and rubbish is seen occasionally
        auto cubeMesh = Core::MeshGenerator::CreateCube();
        createGameObject("Cube")->addComponent<Components::MeshRenderer>(cubeMesh, ASSETS.getErrorMaterial());

        auto guiSceneMenu = gui->addComponent<GUISceneMenu>("Scenes");
        guiSceneMenu->registerScene<AnimationTestScene2>("Animation Test Scene 2 (D3D11 only)");
        guiSceneMenu->registerScene<AnimationTestScene>("Animation Test Scene (D3D11 only)");
        guiSceneMenu->registerScene<SceneGUIThesisScenesMenu>("Thesis Test Scenes");
        guiSceneMenu->registerScene<TestScene>("Test Scene");
        guiSceneMenu->registerScene<SceneSplines>("Catmull-Rom Spline");
        guiSceneMenu->registerScene<VRScene>("VR Scene");
        guiSceneMenu->registerScene<SceneParticleSystem>("Particle System");
        guiSceneMenu->registerScene<ShadowScene>("Shadow Scene");
        guiSceneMenu->registerScene<ScenePostProcessMultiCamera>("Multi Camera Post Processing");
        guiSceneMenu->registerScene<SceneGUI>("GUI Example");
        guiSceneMenu->registerScene<SceneRenderToTexture>("RenderToTexture");
        guiSceneMenu->registerScene<ManyObjectsScene>("Many Drawcalls!");
        guiSceneMenu->registerScene<BlinnPhongLightingScene>("Blinn-Phong Lighting");
        guiSceneMenu->registerScene<ScenePBRSpheres>("[PBR] Spheres");
        guiSceneMenu->registerScene<ScenePBRPistol>("[PBR] Pistol + Dagger");
        guiSceneMenu->registerScene<SponzaScene>("[PBR] Sponza");
        guiSceneMenu->registerScene<TransparencyScene>("Ordered Transparency");
        guiSceneMenu->registerScene<SceneGraphScene>("SceneGraph");
        guiSceneMenu->registerScene<BRDFLUTScene>("BRDFLut");
        guiSceneMenu->registerScene<SceneFrustumVisualization>("Frustum Visualization");
        guiSceneMenu->registerScene<TexArrayScene>("Texture arrays");
        guiSceneMenu->registerScene<CubemapScene>("Cubemap");
        guiSceneMenu->registerScene<VertexGenScene>("Dynamic Vertex Buffer regeneration");
    }
};

//----------------------------------------------------------------------
// GAME
//----------------------------------------------------------------------

class Game : public IGame
{
    GameObject* vrCamGO = nullptr;
    Events::EventListener m_sceneSwitchListener;
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
            Locator::getWindow().setTitle("%s | API: %s | Time: %.0fs | Delta: %fms (%d FPS)", gameName, Locator::getRenderer().getAPIName(), TIME.getTime().value, delta, fps);
        }, 1000);

        ASSETS.setHotReloading(true);
        
        // On scene switch the GO gets deleted automatically, so we just make sure its null
        m_sceneSwitchListener = Events::EventDispatcher::GetEvent(EVENT_SCENE_CHANGED).addListener([this] {
            vrCamGO = nullptr;
            SCENE.getMainCamera()->getGameObject()->addComponent<FadeIn>(1000_ms);
        });

        IGC_REGISTER_COMMAND_WITH_NAME( "menu", BIND_THIS_FUNC_0_ARGS(&Game::_OpenMenu) );

        Locator::getRenderer().setVSync(false);
        Locator::getRenderer().setGlobalFloat(SID("_Ambient"), 0.5f);

        //Locator::getSceneManager().LoadSceneAsync(new SceneGUISelectSceneMenu());
        Locator::getSceneManager().LoadScene(new TestScene());
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        if (KEYBOARD.wasKeyPressed(Key::F5))
            _OpenMenu();

        if (KEYBOARD.wasKeyPressed(Key::Zero))
            Locator::getSceneManager().LoadScene(new TestScene());

        if (KEYBOARD.wasKeyPressed(Key::P))
            Locator::getProfiler().logGPU();

        if (KEYBOARD.wasKeyPressed(Key::O))
            PROFILER.beginProfiling(5_s, [](Profiling::ProfileResult res){
                String str = StringUtils::format( "Avg: %fms (%d FPS)\n" 
                                                  "Min: %fms (%d FPS)\n"
                                                  "Max: %fms (%d FPS)\n",
                                                   res.avgFrameTime.value, (I32)(1000 / res.avgFrameTime.value),
                                                   res.minFrameTime.value, (I32)(1000 / res.minFrameTime.value),
                                                   res.maxFrameTime.value, (I32)(1000 / res.maxFrameTime.value) );
                LOG( "<<<< PROFILING RESULT >>>>\n" + str, Color::GREEN );
            });

        // VR
        {
            if (RENDERER.hasHMD())
            {
                // Perfhud
                static I32 perfHudMode = 0;
                if (KEYBOARD.wasKeyPressed(Key::Left))
                {
                    perfHudMode = perfHudMode - 1; if (perfHudMode < 0) perfHudMode = (I32)Graphics::VR::PerfHudMode::Count - 1;
                    RENDERER.getHMD().setPerformanceHUD((Graphics::VR::PerfHudMode)perfHudMode);
                }
                if (KEYBOARD.wasKeyPressed(Key::Right))
                {
                    perfHudMode = (perfHudMode + 1) % (I32)Graphics::VR::PerfHudMode::Count;
                    RENDERER.getHMD().setPerformanceHUD((Graphics::VR::PerfHudMode)perfHudMode);
                }

                // Toggle between normal & vr camera
                if ((KEYBOARD.wasKeyPressed(Key::V) || CONTROLLER.wasKeyPressed(ControllerKey::Enter)) && RENDERER.hasHMD())
                    _SwitchCameraVR();
            }
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
    void _SwitchCameraVR()
    {
        auto mainCamera = SCENE.getMainCamera();
        if (not mainCamera->isBlittingToHMD())
        {
            // Disable main camera and fps moving script
            bool isActive = mainCamera->isActive();
            mainCamera->setActive(not isActive);

            // Disable movement script if the main camera had one
            auto go = mainCamera->getGameObject();
            if (auto fpsMovScript = go->getComponent<Components::FPSCamera>())
                fpsMovScript->setActive(not isActive);

            // Create/Destroy components for VRCamera 
            if (not vrCamGO)
            {
                vrCamGO = SCENE.createGameObject("VRCamera");
                vrCamGO->getTransform()->position = go->getTransform()->position;
                bool isHDR = mainCamera->isHDR();
                auto vrCam = vrCamGO->addComponent<Components::VRCamera>(Components::ScreenDisplay::LeftEye, Graphics::MSAASamples::Four, isHDR);
                vrCam->setClearColor(mainCamera->getClearColor());
                vrCamGO->addComponent<Components::VRFPSCamera>();
                vrCamGO->addComponent<Components::VRBasicTouch>(Core::MeshGenerator::CreateCubeUV(0.1f), ASSETS.getMaterial("/materials/blinn_phong/cube.material"));
                if (isHDR)
                    vrCamGO->addComponent<Tonemap>();
            }
            else
            {
                vrCamGO->getScene()->destroyGameObject(vrCamGO);
                vrCamGO = nullptr;
            }
        }
    }

    //----------------------------------------------------------------------
    void _OpenMenu() const
    {
        Locator::getSceneManager().LoadScene(new SceneGUISelectSceneMenu);
    }
};

#if DISPLAY_CONSOLE

    int main()
    {
        Game game;
        game.start( gameName, 1280, 720, Graphics::API::D3D11 );
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
