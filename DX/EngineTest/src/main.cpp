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

public:
    TestScene() : IScene("TestScene") {}

    void init() override
    {
        // Camera 1
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -5);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto psGO = createGameObject("ParticleSystem");
        //psGO->addComponent<Components::ParticleSystem>("res/particles/test.ps");
        auto ps = psGO->addComponent<Components::ParticleSystem>(ASSETS.getMaterial("/materials/particles.material"));

        go->addComponent<Components::GUI>();
        go->addComponent<Components::GUIFPS>();
        go->addComponent<Components::GUICustom>([ps] () mutable {
            ImGui::Begin("Particle System Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            {
                if (ImGui::CollapsingHeader("Particle System 0", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    {
                        if (ImGui::Button("Restart")) ps->play();
                        ImGui::SameLine();
                        if (ImGui::Button("Pause")) ps->pause();

                        ImGui::Text("Particles: %d", ps->getCurrentParticleCount());
                        static I32 maxParticles;
                        maxParticles = ps->getMaxParticleCount();
                        if (ImGui::SliderInt("Max Particles", &maxParticles, 0, 100000))
                            ps->setMaxParticleCount(maxParticles);

                        static I32 emissionRate;
                        emissionRate = ps->getEmissionRate();
                        if (ImGui::SliderInt("Emission Rate", &emissionRate, 0, 10000))
                            ps->setEmissionRate(emissionRate);
                    }

                    static F32 gravity;
                    gravity = ps->getGravity();
                    if (ImGui::SliderFloat("Gravity", &gravity, -5.0f, 5.0f, "%.2f"))
                        ps->setGravity(gravity);

                    {
                        CString type[] = { "None", "By Distance" };
                        static I32 type_current = 0;
                        if (ImGui::Combo("Sort Mode", &type_current, type, sizeof(type) / sizeof(CString)))
                            ps->setSortMode((Components::PSSortMode)type_current);
                    }

                    {
                        CString type[] = { "None", "View" };
                        static I32 type_current = 0;
                        if (ImGui::Combo("Particle Alignment", &type_current, type, sizeof(type) / sizeof(CString)))
                            ps->setParticleAlignment((Components::PSParticleAlignment)type_current);
                    }

                    if (ImGui::TreeNode("Clock"))
                    {
                        ImGui::Text("Clock time: %.2fs", (F32)ps->getClock().getTime());
                        ImGui::SameLine();
                        static bool looping;
                        looping = ps->getClock().isLooping();
                        if (ImGui::Checkbox("Looping", &looping))
                            ps->getClock().setIsLooping(looping);

                        static F32 duration;
                        duration = (F32)ps->getClock().getDuration();
                        if (ImGui::SliderFloat("Duration", &duration, 0.1f, 50.0f, "%.2f"))
                            ps->getClock().setDuration(Time::Seconds(duration));

                        static F32 tickMod = 1.0f;
                        tickMod = ps->getClock().getTickModifier();
                        if (ImGui::SliderFloat("Tick Mod", &tickMod, 0.0f, 5.0f, "%.2f"))
                            ps->getClock().setTickModifier(tickMod);

                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Lifetime"))
                    {
                        CString type[] = { "Constant", "Random Between Two Constants" };
                        static I32 type_current = 0;
                        ImGui::Combo("Mode", &type_current, type, sizeof(type)/sizeof(CString));

                        switch ((Components::PSValueMode)type_current)
                        {
                        case Components::PSValueMode::Constant:
                        {
                            static F32 lifeTime = 1.0f;
                            ImGui::SliderFloat("Lifetime", &lifeTime, 0.5f, 10.0f, "%.2f");
                            ps->setSpawnLifetimeFnc(Components::Constant(lifeTime));
                            break;
                        }
                        case Components::PSValueMode::RandomBetweenTwoConstants:
                        {
                            static F32 lifeTimeStart = 0.5f;
                            static F32 lifeTimeEnd = 10.0f;
                            if (ImGui::SliderFloat("Min", &lifeTimeStart, 0.5f, 10.0f, "%.2f"))
                                if (lifeTimeStart > lifeTimeEnd) lifeTimeEnd = lifeTimeStart;
                            if (ImGui::SliderFloat("Max", &lifeTimeEnd, 0.5f, 10.0f, "%.2f"))
                                if (lifeTimeEnd > lifeTimeStart) lifeTimeStart = lifeTimeEnd;
                            ps->setSpawnLifetimeFnc(Components::RandomBetweenTwoConstants(lifeTimeStart, lifeTimeEnd));
                            break;
                        }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Color"))
                    {
                        CString type[] = { "Constant", "Random Between Two Constants" };
                        static I32 type_current = 0;
                        ImGui::Combo("Mode", &type_current, type, sizeof(type) / sizeof(CString));

                        switch ((Components::PSValueMode)type_current)
                        {
                        case Components::PSValueMode::Constant:
                        {
                            static F32 color[4] = { 1,1,1,1 };
                            ImGui::ColorEdit4("Color", color);
                            ps->setSpawnColorFnc(Components::Constant(Color(color)));
                            break;
                        }
                        case Components::PSValueMode::RandomBetweenTwoConstants:
                        {
                            static F32 colorStart[4] = { 1,1,1,1 };
                            static F32 colorEnd[4] = { 1,1,1,1 };
                            ImGui::ColorEdit4("Min", colorStart);
                            ImGui::ColorEdit4("Max", colorEnd);
                            ps->setSpawnColorFnc(Components::RandomBetweenTwoConstants(Color(colorStart), Color(colorEnd)));
                            break;
                        }
                        }
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::End();
        });

        LOG("TestScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
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
        gui->addComponent<Components::Camera>();
        gui->addComponent<Components::GUI>();

        auto guiSceneMenu = gui->addComponent<GUISceneMenu>();
        guiSceneMenu->registerScene<TestScene>("Test Scene");
        guiSceneMenu->registerScene<ShadowScene>("Shadow Scene");
        guiSceneMenu->registerScene<VertexGenScene>("Dynamic Vertex Buffer regeneration");
        guiSceneMenu->registerScene<ScenePostProcessMultiCamera>("Multi Camera Post Processing");
        guiSceneMenu->registerScene<SceneGUI>("GUI Example");
        guiSceneMenu->registerScene<SceneMirror>("Offscreen rendering on material");
        guiSceneMenu->registerScene<ManyObjectsScene>("Many Objects!");
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

        LOG("SceneGUISelectSceneMenu initialized!", Color::RED);
    }
    void shutdown() override { LOG("SceneGUISelectSceneMenu Shutdown!", Color::RED); }
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
        getWindow().setIcon( "/engine/icon.ico" );
        gLogger->setSaveToDisk( false );

        Locator::getEngineClock().setInterval([=] {
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
            String newTitle = String(gameName) + " | Time: " + TS(TIME.getTime().value) + " | Delta: " + TS(delta) + "ms (" + TS(fps) + " FPS)";
            Locator::getWindow().setTitle(newTitle.c_str());
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
        //clock.tick( delta );
        //LOG( TS( clock.getTime().value ) );
        //auto time = clock.getTime();

        if (KEYBOARD.wasKeyPressed(Key::Backspace))
            _OpenMenu();

        if (KEYBOARD.wasKeyPressed(Key::Zero))
            Locator::getSceneManager().LoadSceneAsync(new TestScene());

        if (KEYBOARD.wasKeyPressed(Key::P))
            PROFILER.logGPU();

        if (KEYBOARD.wasKeyPressed(Key::F1))
            Locator::getRenderer().setGlobalMaterialActive("NONE");
        if (KEYBOARD.wasKeyPressed(Key::F2))
            Locator::getRenderer().setGlobalMaterialActive("Wireframe");

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
