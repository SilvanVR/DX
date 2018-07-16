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
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -15);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto psGO = createGameObject("ParticleSystem");
        //psGO->addComponent<ConstantRotation>(0.0f, 15.0f, 0.0f);
        auto ps = psGO->addComponent<Components::ParticleSystem>("res/particles/test.ps");
        //auto ps = psGO->addComponent<Components::ParticleSystem>(ASSETS.getMaterial("/materials/particles.material"));
        //ps->setMesh(Core::MeshGenerator::CreateCubeUV());
        //ps->setSpawnPositionFunc( Components::ShapeBox{ Math::Vec3{ -1,-1,-1 } * 10, Math::Vec3{ 1,1,1 } * 10 } );
        //ps->setSpawnPositionFunc( Components::ShapeSphere{ Math::Vec3{0}, 1.0f } );
        ps->setMaxParticleCount(1);

        go->addComponent<Components::GUI>();
        go->addComponent<Components::GUIFPS>();
        go->addComponent<Components::GUICustom>([ps] () mutable {
            ImGui::Begin("Particle System Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            {
                if (ImGui::CollapsingHeader("Particle System 0", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    static F32 pos[3];
                    ImGui::SliderFloat3("Pos", pos, -10.0f, 10.0f);
                    ps->getGameObject()->getTransform()->position = { pos[0],pos[1],pos[2] };

                    static F32 scale = 1.0f;
                    ImGui::SliderFloat("Scale", &scale, 0.0f, 10.0f);
                    ps->getGameObject()->getTransform()->scale = scale;

                    {
                        if (ImGui::Button("Restart")) ps->play();
                        ImGui::SameLine();
                        if (ImGui::Button("Pause")) ps->pause();
                        ImGui::SameLine();
                        if (ImGui::Button("Resume")) ps->resume();

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
                        CString type[] = { "None", "View" };
                        static I32 type_current = 1;
                        type_current = (I32)ps->getParticleAlignment();
                        if (ImGui::Combo("Particle Alignment", &type_current, type, sizeof(type) / sizeof(CString)))
                            ps->setParticleAlignment((Components::ParticleSystem::ParticleAlignment)type_current);
                    }

                    {
                        CString type[] = { "None", "By Distance" };
                        static I32 type_current = 0;
                        type_current = (I32)ps->getSortMode();
                        if (ImGui::Combo("Sort Mode", &type_current, type, sizeof(type) / sizeof(CString)))
                            ps->setSortMode((Components::ParticleSystem::SortMode)type_current);
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

                    if (ImGui::TreeNode("Volume"))
                    {
                        { // Spawn values
                            CString type[] = { "Box", "Sphere" };
                            static I32 type_current = 0;
                            ImGui::Combo("Volume Type", &type_current, type, sizeof(type) / sizeof(CString));

                            switch ((Components::PSShape)type_current)
                            {
                            case Components::PSShape::Box:
                            {
                                static Math::Vec3 min = -10.0f;
                                static Math::Vec3 max = 10.0f;
                                ImGui::SliderFloat3("Min", (float*)&min, -30.0f, 30.0f, "%.2f");
                                ImGui::SliderFloat3("Max", (float*)&max, -30.0f, 30.0f, "%.2f");
                                ps->setSpawnPositionFunc(Components::ShapeBox(min, max));
                                break;
                            }
                            case Components::PSShape::Sphere:
                            {
                                static Math::Vec3 center = 0.0f;
                                static F32 radius = 10.0f;
                                ImGui::SliderFloat3("Center", (float*)&center, -30.0f, 30.0f, "%.2f");
                                ImGui::SliderFloat("Radius", &radius, 0.0f, 30.0f, "%.2f");
                                ps->setSpawnPositionFunc(Components::ShapeSphere(center, radius));
                                break;
                            }
                            }
                        }
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
                                if (lifeTimeEnd < lifeTimeStart) lifeTimeStart = lifeTimeEnd;
                            ps->setSpawnLifetimeFnc(Components::RandomBetweenTwoConstants(lifeTimeStart, lifeTimeEnd));
                            break;
                        }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Size"))
                    {
                        { // Spawn values
                            CString type[] = { "Constant", "Random Between Two Constants" };
                            static I32 type_current = 0;
                            ImGui::Combo("Spawn Mode", &type_current, type, sizeof(type) / sizeof(CString));

                            switch ((Components::PSValueMode)type_current)
                            {
                            case Components::PSValueMode::Constant:
                            {
                                static F32 size = 1.0f;
                                ImGui::SliderFloat("Size", &size, 0.1f, 10.0f, "%.2f");
                                ps->setSpawnScaleFnc(Components::Constant(size));
                                break;
                            }
                            case Components::PSValueMode::RandomBetweenTwoConstants:
                            {
                                static F32 sizeMin = 1.0f;
                                static F32 sizeMax = 1.0f;
                                if (ImGui::SliderFloat("Min", &sizeMin, 0.1f, 10.0f, "%.2f"))
                                    if (sizeMin > sizeMax) sizeMax = sizeMin;
                                if (ImGui::SliderFloat("Max", &sizeMax, 0.1f, 10.0f, "%.2f"))
                                    if (sizeMax < sizeMin) sizeMin = sizeMax;
                                ps->setSpawnScaleFnc(Components::RandomBetweenTwoConstants(sizeMin, sizeMax));
                                break;
                            }
                            }
                        }
                        { // Over lifetime
                            static bool active = false;
                            if (ImGui::Checkbox("Over lifetime", &active))
                                if (!active) ps->setLifetimeScaleFnc(nullptr);

                            if (active)
                            {
                                static F32 start = 1.0f;
                                static F32 mid = 1.0f;
                                static F32 end = 1.0f;
                                ImGui::SliderFloat("Start", &start, 0.1f, 10.0f, "%.2f");
                                ImGui::SliderFloat("Mid", &mid, 0.1f, 10.0f, "%.2f");
                                ImGui::SliderFloat("End", &end, 0.1f, 10.0f, "%.2f");
                                ps->setLifetimeScaleFnc([](F32 lerp) -> F32 {
                                    return lerp < 0.5f ? Math::Lerp(start, mid, lerp * 2) : Math::Lerp(mid, end, (lerp-0.5f) * 2);
                                });
                            }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Rotation"))
                    {
                        { // Spawn values
                            CString type[] = { "Constant", "Random Between Two Constants" };
                            static I32 type_current = 0;
                            ImGui::Combo("Spawn Mode", &type_current, type, sizeof(type) / sizeof(CString));

                            switch ((Components::PSValueMode)type_current)
                            {
                            case Components::PSValueMode::Constant:
                            {
                                static Math::Vec3 rotation = 0.0f;
                                ImGui::SliderFloat3("Rotation", (float*)&rotation, 0.0f, 360.0f, "%.2f");
                                ps->setSpawnRotationFunc(Components::Constant(Math::Quat::FromEulerAngles(rotation)));
                                break;
                            }
                            case Components::PSValueMode::RandomBetweenTwoConstants:
                            {
                                static Math::Vec3 rotationMin = 0.0f;
                                static Math::Vec3 rotationMax = 360.0f;
                                ImGui::SliderFloat3("Min", (float*)&rotationMin, 0.0f, 360.0f, "%.2f");
                                ImGui::SliderFloat3("Max", (float*)&rotationMax, 0.0f, 360.0f, "%.2f");
                                ps->setSpawnRotationFunc([]() { return Math::Quat::FromEulerAngles(Math::Random::Vec3(rotationMin, rotationMax)); });
                                break;
                            }
                            }
                        }
                        { // Over lifetime
                            static bool active = false;
                            if (ImGui::Checkbox("Over lifetime", &active))
                                if (!active) ps->setLifetimeRotationFnc(nullptr);

                            if (active)
                            {
                                static Math::Vec3 rotationStart = 0.0f;
                                static Math::Vec3 rotationMid = 0.0f;
                                static Math::Vec3 rotationEnd = 0.0f;
                                ImGui::SliderFloat3("Start", (float*)&rotationStart, 0.0f, 360.0f, "%.2f");
                                ImGui::SliderFloat3("Mid", (float*)&rotationMid, 0.0f, 360.0f, "%.2f");
                                ImGui::SliderFloat3("End", (float*)&rotationEnd, 0.0f, 360.0f, "%.2f");
                                ps->setLifetimeRotationFnc([](F32 lerp) -> Math::Quat {
                                    auto v = lerp <= 0.5f ? Math::Lerp(rotationStart, rotationMid, lerp*2) : Math::Lerp(rotationMid, rotationEnd, (lerp-0.5f) * 2);
                                    return Math::Quat::FromEulerAngles(v);
                                });
                            }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Color"))
                    {
                        { // Spawn values
                            CString type[] = { "Constant", "Random Between Two Colors" };
                            static I32 type_current = 0;
                            ImGui::Combo("Spawn Mode", &type_current, type, sizeof(type) / sizeof(CString));

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
                        }
                        { // Over lifetime
                            static bool active = false;
                            if (ImGui::Checkbox("Over lifetime", &active))
                                if (!active) ps->setLifetimeColorFnc(nullptr);

                            if (active)
                            {
                                static F32 colorStart[4] = { 1,1,1,1 };
                                static F32 colorMid[4] = { 1,1,1,1 };
                                static F32 colorEnd[4] = { 1,1,1,1 };
                                ImGui::ColorEdit4("Start", colorStart);
                                ImGui::ColorEdit4("Mid", colorMid);
                                ImGui::ColorEdit4("End", colorEnd);
                                ps->setLifetimeColorFnc([](F32 lerp) -> Color {
                                    return lerp <= 0.5f ? Color::Lerp(Color(colorStart), Color(colorMid), lerp * 2) : Color::Lerp(Color(colorMid), Color(colorEnd), (lerp-0.5f)*2);
                                });
                            }
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Velocity"))
                    {
                        { // Spawn values
                            CString type[] = { "Constant", "Random Between Two Constants" };
                            static I32 type_current = 0;
                            ImGui::Combo("Spawn Mode", &type_current, type, sizeof(type) / sizeof(CString));

                            switch ((Components::PSValueMode)type_current)
                            {
                            case Components::PSValueMode::Constant:
                            {
                                static Math::Vec3 velocity;
                                ImGui::SliderFloat3("Velocity", (float*)&velocity, -5.0f, 5.0f, "%.2f");
                                ps->setSpawnVelocityFunc(Components::Constant(velocity));
                                break;
                            }
                            case Components::PSValueMode::RandomBetweenTwoConstants:
                            {
                                static Math::Vec3 velocityMin = -1.0f;
                                static Math::Vec3 velocityMax = 1.0f;
                                ImGui::SliderFloat3("Min", (float*)&velocityMin, -5.0f, 5.0f, "%.2f");
                                ImGui::SliderFloat3("Max", (float*)&velocityMax, -5.0f, 5.0f, "%.2f");
                                ps->setSpawnVelocityFunc(Components::ShapeBox(velocityMin, velocityMax));
                                break;
                            }
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
