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

namespace Components
{

}

//----------------------------------------------------------------------
// SCENES
//----------------------------------------------------------------------

class TestScene : public IScene
{
    Components::Camera* cam;
    GameObject* go;

    GameObject* test;

public:
    TestScene() : IScene("TestScene") {}

    void init() override
    {
        // Camera 1
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>( 45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One );
        cam->setClearColor(Color(175, 181, 191));
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -5);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        go->addComponent<Components::AudioListener>();

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto cube = Core::MeshGenerator::CreateCubeUV(1.0f);
        createGameObject("Cube")->addComponent<Components::MeshRenderer>(cube, ASSETS.getMaterial("/materials/texture.material"));

        LOG("TestScene initialized!", Color::RED);
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
        gui->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);
        gui->addComponent<Components::GUI>();

        auto guiSceneMenu = gui->addComponent<GUISceneMenu>();
        guiSceneMenu->registerScene<TestScene>("Test Scene");
        guiSceneMenu->registerScene<VRScene>("VR Scene");
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
        Locator::getSceneManager().LoadSceneAsync(new VRScene());
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        if (KEYBOARD.wasKeyPressed(Key::F5))
            _OpenMenu();

        if (KEYBOARD.wasKeyPressed(Key::Zero))
            Locator::getSceneManager().LoadScene(new TestScene());

        // VR
        {
            // Toggle between normal & vr camera
            if (KEYBOARD.wasKeyPressed(Key::V) && RENDERER.hasHMD())
            {
                auto mainCamera = SCENE.getMainCamera();
                if (not mainCamera->isBlittingToHMD())
                {
                    // Disable main camera and fps moving script
                    bool isActive = mainCamera->isActive();
                    mainCamera->setActive(not isActive);

                    auto go = mainCamera->getGameObject();
                    if (auto fpsMovScript = go->getComponent<Components::FPSCamera>())
                        fpsMovScript->setActive(not isActive);

                    // Create/Destroy VRCamera 
                    static GameObject* vrCamGO = nullptr;
                    if (not vrCamGO)
                    {
                        vrCamGO = SCENE.createGameObject("VRCamera");
                        vrCamGO->getTransform()->position = go->getTransform()->position;
                        vrCamGO->addComponent<Components::VRCamera>(Components::ScreenDisplay::LeftEye);
                        vrCamGO->addComponent<Components::VRFPSCamera>();
                        vrCamGO->addComponent<Components::VRBasicTouch>(Core::MeshGenerator::CreateCubeUV(0.1f), ASSETS.getMaterial("/materials/blinn_phong/cube.material"));
                        //vrCamGO->addComponent<PostProcess>(ASSETS.getMaterial("/materials/post processing/color_grading.material"));
                    }
                    else
                    {
                        vrCamGO->removeComponent<Components::VRCamera>();
                        vrCamGO->removeComponent<Components::VRFPSCamera>();
                        vrCamGO->removeComponent<Components::VRBasicTouch>();
                        vrCamGO->getScene()->destroyGameObject( vrCamGO );
                        vrCamGO = nullptr;
                    }
                }
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
    void _OpenMenu() const
    {
        Locator::getSceneManager().LoadScene(new SceneGUISelectSceneMenu);
    }
};

#if DISPLAY_CONSOLE

    int main()
    {
        Game game;
        game.start( gameName, 800, 600, Graphics::API::D3D11 );

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
