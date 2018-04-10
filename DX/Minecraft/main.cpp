#include <DX.h>

#define STB_VOXEL_RENDER_IMPLEMENTATION
#define STBVOX_CONFIG_MODE 20
#include "stb_voxel_renderer.hpp"

#include "PolyVoxCore/Array.h"

#define DISPLAY_CONSOLE 1


class DrawFrustum : public Components::IComponent
{
public:
    void AddedToGameObject(GameObject* go) override
    {
        auto cam = go->getComponent<Components::Camera>();
        auto mesh = Core::Assets::MeshGenerator::CreateFrustum(Math::Vec3(0), Math::Vec3::UP, Math::Vec3::RIGHT, Math::Vec3::FORWARD,
            cam->getFOV(), cam->getZNear(), cam->getZFar(), cam->getAspectRatio());

        auto mr = go->addComponent<Components::MeshRenderer>();
        mr->setMesh(mesh);
    }
};

class WorldGeneration : public Components::IComponent
{
    stbvox_mesh_maker* mm;

public:
    void Init() override
    {
       // stbvox_init_mesh_maker(mm);

        //   In mode 0 & mode 20, there is only one slot. The mesh data for that
        //   slot is two interleaved vertex attributes: attr_vertex, a single
        //   32-bit uint, and attr_face, a single 32-bit uint.
       // stbvox_set_buffer();
    }

    void Tick(Time::Seconds delta) override
    {

    }
};

class Minimap : public Components::IComponent
{
    GameObject* minimapCamGo;
    Components::Transform* minimapCamTransform;
    Components::Transform* transform;
    F32 m_heightOffset;
    F32 m_size;

public:
    Minimap(F32 heightOffset, F32 size) : m_heightOffset(heightOffset), m_size(size) {}

    void AddedToGameObject(GameObject* go) override
    {
        minimapCamGo = go->getScene()->createGameObject();
        auto cam = minimapCamGo->addComponent<Components::Camera>();
        cam->setClearMode(Components::Camera::EClearMode::NONE);
        cam->setCameraMode(Components::Camera::EMode::ORTHOGRAPHIC);
        cam->setOrthoParams(-m_size, m_size,-m_size, m_size,0,1000);
        cam->getViewport().height = 0.3f;
        cam->getViewport().width = 0.3f;
        //minimapCamGo->addComponent<DrawFrustum>();

        transform = go->getComponent<Components::Transform>();
        minimapCamTransform = minimapCamGo->getComponent<Components::Transform>();
        minimapCamTransform->position.y = m_heightOffset;
        minimapCamTransform->rotation = Math::Quat::LookRotation(Math::Vec3::DOWN, Math::Vec3::FORWARD);
    }

    void Tick(Time::Seconds delta) override
    {
        minimapCamTransform->position = transform->position;
        minimapCamTransform->position.y += m_heightOffset;
        //minimapCamTransform->lookAt(transform->position);

        //DEBUG.drawSphere(minimapCamTransform->position, 1.0f, Color::RED, 0);
    }
};

class MyScene : public IScene
{
    GameObject*             go;
    Components::Camera*     cam;

public:
    MyScene() : IScene("MyScene"){}

    void init() override
    {
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,0,-10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        go->addComponent<Minimap>(20.0f, 3.0f);

        createGameObject("World Generation")->addComponent<WorldGeneration>();

        // SHADER
        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");

        // TEXTURES
        auto dirt = ASSETS.getTexture2D("/textures/dirt.jpg");
        auto terrain = ASSETS.getTexture2D("/textures/terrain.png");
        terrain->setFilter(Graphics::TextureFilter::Point);
        terrain->setAnisoLevel(1);

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture(SID("tex0"), terrain);
        material->setTexture(SID("tex1"), dirt);
        material->setFloat(SID("mix"), 0.0f);
        material->setColor(SID("tintColor"), Color::WHITE);

        // MESH
        auto mesh = Core::Assets::MeshGenerator::CreateCubeUV();

        // GAMEOBJECT
        createGameObject("Cube")->addComponent<Components::MeshRenderer>(mesh, material);
    }

    void shutdown() override
    {
    }
};

class Game : public IGame
{
    GameObject* go;
    Components::Camera* cam;

public:
    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        gLogger->setSaveToDisk( false );

        Locator::getEngineClock().setInterval([] {
           LOG( "Time: " + TS( Locator::getEngineClock().getTime().value ) + " FPS: " + TS( Locator::getProfiler().getFPS() ) );
        }, 1000);

        getWindow().setCursor( "../dx/res/internal/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "../dx/res/internal/icon.ico" );

        Locator::getSceneManager().LoadSceneAsync( new MyScene() );
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        static U64 ticks = 0;

        ticks++;
        //LOG( "Tick: " + TS(ticks) );

        if ( ticks == GAME_TICK_RATE * 100.1f)
            terminate();
    }

    //----------------------------------------------------------------------
    void shutdown() override 
    {
        LOG( "Shutdown game..." );
    }
};

#ifdef _DEBUG
        const char* gameName = "[DEBUG] Minecraft";
#else
        const char* gameName = "[RELEASE] Minecraft";
#endif

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
        game.start(gameName, 800, 600);

        return 0;
    }

#endif