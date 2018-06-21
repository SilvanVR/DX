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
    Components::Camera* cam;

public:
    TestScene() : IScene("TestScene") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, -8);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        createGameObject("Grid")->addComponent<GridGeneration>(20);
        go->addComponent<Components::GUI>();
        go->addComponent<Components::GUIImage>(ASSETS.getTexture2D("/textures/nico.jpg"));
        go->addComponent<Components::GUIDemoWindow>();

        auto go2 = createGameObject("Camera");
        auto renderTex = RESOURCES.createRenderTexture(1024, 720, Graphics::DepthFormat::None, Graphics::TextureFormat::BGRA32, 2, Graphics::MSAASamples::One);
        auto cam2 = go2->addComponent<Components::Camera>(renderTex);
        cam2->setRenderTarget(renderTex);
        go2->addComponent<Components::GUI>();
        go2->addComponent<Components::GUIDemoWindow>();

        auto depthMapGO = createGameObject("DepthMapGO");
        auto depthMapMaterial = RESOURCES.createMaterial(ASSETS.getShader("/shaders/tex.shader"));
        depthMapMaterial->setTexture("tex", cam2->getRenderTarget()->getColorBuffer());
        depthMapMaterial->setColor("tintColor", Color::WHITE);
        depthMapGO->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), depthMapMaterial);
        depthMapGO->getTransform()->position = { 0, 1, 0 };

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
            cam->setMultiSamples( (Graphics::MSAASamples) newmscount );
            LOG("New Multisample-Count: " + TS(newmscount), Color::GREEN);
        }
    }

    void shutdown() override {
        LOG("TestScene Shutdown!", Color::RED); 
    }
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



//class TestScene : public IScene
//{
//    Components::Camera* cam;
//public:
//    TestScene() : IScene("TestScene") {}
//
//    void init() override
//    {
//        // Camera 1
//        auto go = createGameObject("Camera");
//        cam = go->addComponent<Components::Camera>();
//        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, -8);
//        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
//        //createGameObject("Grid")->addComponent<GridGeneration>(20);
//
//        auto planeMesh = Core::MeshGenerator::CreatePlane();
//        auto obj = createGameObject("GO");
//        obj->addComponent<Components::MeshRenderer>(planeMesh, ASSETS.getMaterial("/materials/blinn_phong/grass.material"));
//        obj->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90.0f);
//        obj->getTransform()->scale = { 20,20,20 };
//
//        auto obj2 = createGameObject("GO2");
//        obj2->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/blinn_phong/monkey.material"));
//        obj2->getTransform()->position = { 5, 1, 0 };
//
//        auto cubeGO = createGameObject("GO3");
//        cubeGO->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreateCubeUV(0.3f), ASSETS.getMaterial("/materials/blinn_phong/cube.material"));
//        cubeGO->getTransform()->position = { 0.0f, 0.3001f, -3.0f };
//
//        Assets::MeshMaterialInfo matInfo;
//        auto treeMesh = ASSETS.getMesh("/models/tree/tree.obj", &matInfo);
//
//        auto treeGO = createGameObject("Tree");
//        auto mr = treeGO->addComponent<Components::MeshRenderer>(treeMesh);
//        //treeGO->getTransform()->scale = { 0.01f };
//
//        if (matInfo.isValid())
//        {
//            for (I32 i = 0; i < treeMesh->getSubMeshCount(); i++)
//            {
//                auto material = RESOURCES.createMaterial(ASSETS.getShader("/shaders/phong_shadow.shader"));
//                material->setFloat("uvScale", 1.0f);
//
//                for (auto& texture : matInfo[i].textures)
//                {
//                    switch (texture.type)
//                    {
//                    case Assets::MaterialTextureType::Albedo: material->setTexture("albedo", ASSETS.getTexture2D(texture.filePath)); break;
//                    case Assets::MaterialTextureType::Normal: material->setTexture("normalMap", ASSETS.getTexture2D(texture.filePath)); break;
//                    case Assets::MaterialTextureType::Shininess: break;
//                    case Assets::MaterialTextureType::Specular: break;
//                    }
//                }
//                mr->setMaterial(material, i);
//            }
//        }
//        //treeGO->addComponent<VisualizeNormals>(0.1f, Color::BLUE);
//
//        // LIGHTS
//        auto sun = createGameObject("Sun");
//        sun->addComponent<Components::DirectionalLight>(1.0f, Color::WHITE);
//        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });
//        //sun->addComponent<ConstantRotation>(15.0f, 0.0f, 0.0f);
//
//        auto depthMapGO = createGameObject("DepthMapGO");
//        auto depthMapMaterial = RESOURCES.createMaterial(ASSETS.getShader("/shaders/tex.shader"));
//        depthMapMaterial->setTexture("tex", ASSETS.getTexture2D("/textures/nico.jpg"));
//        depthMapMaterial->setColor("tintColor", Color::WHITE);
//        depthMapGO->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), depthMapMaterial);
//        depthMapGO->getTransform()->position = { -5, 1, 0 };
//
//        // IMGUI
//        ImGui::CreateContext();
//        ImGuiIO& io = ImGui::GetIO(); (void)io;
//
//        ImGui::StyleColorsDark();
//
//        LOG("TestScene initialized!", Color::RED);
//    }
//
//    void tick(Time::Seconds d) override
//    {
//        if (KEYBOARD.wasKeyPressed(Key::M))
//        {
//            static int index = 0;
//            U32 mscounts[]{ 1,2,4,8 };
//            U32 newmscount = mscounts[index];
//            index = (index + 1) % (sizeof(mscounts) / sizeof(int));
//            cam->setMultiSamples((Graphics::MSAASamples) newmscount);
//            LOG("New Multisample-Count: " + TS(newmscount), Color::GREEN);
//        }
//    }
//
//    void shutdown() override { LOG("TestScene Shutdown!", Color::RED); }
//};
