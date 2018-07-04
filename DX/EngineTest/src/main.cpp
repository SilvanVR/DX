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

public:
    TestScene() : IScene("TestScene") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 10, -25);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        cam->setClearColor(Color(66, 134, 244));
        //go->addComponent<AutoOrbiting>(15.0f);
        //auto camSpot = go->addComponent<Components::SpotLight>(1.0f, Color::WHITE, 25.0f, 20.0f);

        //createGameObject("Grid")->addComponent<GridGeneration>(20);
        auto obj = createGameObject("GO");
        obj->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), ASSETS.getMaterial("/materials/blinn_phong/grass.material"));
        obj->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90.0f);
        obj->getTransform()->scale = { 20,20,20 };

        auto obj2 = createGameObject("GO2");
        obj2->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/blinn_phong/monkey.material"));
        obj2->getTransform()->position = { 5, 1, 0 };

        auto cubeGO = createGameObject("GO3");
        cubeGO->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreateCubeUV(0.3f), ASSETS.getMaterial("/materials/blinn_phong/cube.material"));
        cubeGO->getTransform()->position = { -5.0f, 0.3001f, 0.0f };
        cubeGO->addComponent<ConstantRotation>(0.0f, 10.0f, 0.0f);

        Assets::MeshMaterialInfo matInfo;
        auto treeMesh = ASSETS.getMesh("/models/tree/tree.obj", &matInfo);

        auto treeGO = createGameObject("Tree");
        auto mr = treeGO->addComponent<Components::MeshRenderer>(treeMesh);

        if (matInfo.isValid())
        {
            for (I32 i = 0; i < treeMesh->getSubMeshCount(); i++)
            {
                auto material = RESOURCES.createMaterial(ASSETS.getShader("/shaders/phong_shadow.shader"));
                material->setFloat("uvScale", 1.0f);

                for (auto& texture : matInfo[i].textures)
                {
                    switch (texture.type)
                    {
                    case Assets::MaterialTextureType::Albedo: material->setTexture("_MainTex", ASSETS.getTexture2D(texture.filePath)); break;
                    case Assets::MaterialTextureType::Normal: material->setTexture("normalMap", ASSETS.getTexture2D(texture.filePath)); break;
                    case Assets::MaterialTextureType::Shininess: break;
                    case Assets::MaterialTextureType::Specular: break;
                    }
                }
                material->setReplacementShader( TAG_SHADOW_PASS, ASSETS.getShadowMapShaderAlpha() );
                material->setReplacementShader( TAG_SHADOW_PASS_LINEAR, ASSETS.getShadowMapShaderLinearAlpha() );
                mr->setMaterial(material, i);
            }
        }
        //treeGO->addComponent<VisualizeNormals>(0.1f, Color::BLUE);

        // LIGHTS
        auto sun = createGameObject("Sun");
        auto dl = sun->addComponent<Components::DirectionalLight>(0.3f, Color::WHITE, Graphics::ShadowType::CSMSoft, ArrayList<F32>{10.0f, 30.0f, 80.0f, 200.0f});
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });
        //sun->addComponent<ConstantRotation>(5.0f, 0.0f, 0.0f);

        //auto sun2 = createGameObject("Sun2");
        //sun2->addComponent<Components::DirectionalLight>(0.3f, Color::WHITE);
        //sun2->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, -1 });

        auto plg = createGameObject("PL");
        auto pl = plg->addComponent<Components::PointLight>(1.0f, Color::ORANGE, 5.0f, true);
        plg->getTransform()->position = { 3, 2, 0 };
        plg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/textures/pointLight.png"), 0.5f);
        go->addComponent<Components::Skybox>(pl->getShadowMap());

        auto slg = createGameObject("PL");
        auto sl = slg->addComponent<Components::SpotLight>(1.0f, Color::WHITE, 25.0f, 20.0f);
        slg->getTransform()->position = { -5, 2, -2 };
        slg->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });
        //slg->addComponent<DrawFrustum>();

        go->addComponent<Components::GUI>();
        //auto imgComp = go->addComponent<Components::GUIImage>(dl->getShadowMap(), 0, Math::Vec2{200, 200});
        go->addComponent<Components::GUIFPS>();
        go->addComponent<Components::GUICustom>([=] {
            static F32 ambient = 0.4f;
            ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
            Locator::getRenderer().setGlobalFloat(SID("_Ambient"), ambient);

            if (ImGui::CollapsingHeader("Shadows"))
            {
                static bool changedSettings = false;

                static bool shadowsActive = true;
                if (ImGui::RadioButton("Enabled", shadowsActive))
                {
                    shadowsActive = !shadowsActive;
                    changedSettings = true;
                }

                CString type[] = { "Hard", "Soft" };
                static I32 type_current = 1;
                if (ImGui::Combo("Type", &type_current, type, 2))
                    changedSettings = true;

                CString qualities[] = { "Low", "Medium", "High", "Insane" };
                static I32 quality_current = 2;
                if (ImGui::Combo("Quality", &quality_current, qualities, 4))
                    changedSettings = true;

                if (changedSettings)
                {
                    changedSettings = false;
                    if (not shadowsActive)
                    {
                        CONFIG.setShadowType(Graphics::ShadowType::None);
                    }
                    else
                    {
                        CONFIG.setShadowTypeAndQuality(Graphics::ShadowType(type_current + 1), Graphics::ShadowMapQuality(quality_current));
                        //imgComp->setTexture(dl->getShadowMap()); // Must be set new if shadowmap gets recreated
                    }
                }
            }

            if (ImGui::CollapsingHeader("Lights"))
            {
                if (ImGui::TreeNode("Directional Light"))
                {
                    CString type[] = { "None", "Hard", "Soft", "CSM", "CSMSoft" };
                    static I32 type_current = 1;
                    type_current = (I32)dl->getShadowType(); 
                    if (ImGui::Combo("Shadow Type", &type_current, type, 5))
                        dl->setShadowType((Graphics::ShadowType)(type_current));

                    CString qualities[] = { "Low", "Medium", "High", "Insane" };
                    static I32 quality_current = 2;
                    quality_current = (I32)dl->getShadowMapQuality();
                    if (ImGui::Combo("Quality", &quality_current, qualities, 4))
                        dl->setShadowMapQuality((Graphics::ShadowMapQuality)(quality_current));

                    static Math::Vec3 deg{ 45.0f, 0.0f, 0.0f };
                    ImGui::SliderFloat2("Rotation", &deg.x, 0.0f, 360.0f);
                    sun->getTransform()->rotation = Math::Quat::FromEulerAngles(deg);

                    if (type_current > 0 && type_current <= 2)
                    {
                        static F32 dlRange = 30.0f;
                        ImGui::SliderFloat("Shadow Range", &dlRange, 5.0f, 100.0f);
                        dl->setShadowRange(dlRange);
                        ImGui::Image(dl->getShadowMap(), 0, Math::Vec2{ 200, 200 });
                    }
                    else if (type_current > 2)
                    {
                        ImGui::Text("Shadow-maps");
                        for (I32 i = 0; i < dl->getCascadeCount(); ++i)
                            ImGui::Image(dl->getShadowMap(), i, Math::Vec2{ 200, 200 });
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Point Light"))
                {
                    static Math::Vec3 pos{ 0, 2.5, -1 };
                    ImGui::SliderFloat3("Position", &pos.x, -3.0f, 3.0f);
                    plg->getTransform()->position = pos;

                    static F32 range = 25.0f;
                    ImGui::SliderFloat("Range", &range, 5.0f, 50.0f);

                    static F32 intensity = 1.0f;
                    ImGui::SliderFloat("Intensity", &intensity, 0.5f, 5.0f);

                    pl->setIntensity(intensity);
                    pl->setRange(range);

                    ImGui::TreePop();
                }
            }
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
            index = (index + 1) % (sizeof(mscounts) / sizeof(int));
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
        guiSceneMenu->registerScene<VertexGenScene>("Dynamic Vertex Buffer regeneration");
        guiSceneMenu->registerScene<ScenePostProcessMultiCamera>("Multi Camera Post Processing");
        guiSceneMenu->registerScene<TestScene>("Test Scene");
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
            Locator::getSceneManager().LoadSceneAsync(new SceneGUISelectSceneMenu);

        if (KEYBOARD.wasKeyPressed(Key::One))
            Locator::getSceneManager().LoadSceneAsync(new VertexGenScene);
        if (KEYBOARD.wasKeyPressed(Key::Two))
            Locator::getSceneManager().LoadSceneAsync(new ScenePostProcessMultiCamera);
        if (KEYBOARD.wasKeyPressed(Key::Three))
            Locator::getSceneManager().LoadSceneAsync(new SceneMirror);
        if (KEYBOARD.wasKeyPressed(Key::Four))
            Locator::getSceneManager().LoadSceneAsync(new ManyObjectsScene(10000));
        if (KEYBOARD.wasKeyPressed(Key::Five))
            Locator::getSceneManager().LoadSceneAsync(new BlinnPhongLightingScene());
        if (KEYBOARD.wasKeyPressed(Key::Six))
            Locator::getSceneManager().LoadSceneAsync(new ScenePBRSpheres());
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

        if(KEYBOARD.isKeyDown(Key::Escape))
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
