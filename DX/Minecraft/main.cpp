#include <DX.h>

#define STB_VOXEL_RENDER_IMPLEMENTATION
#define STBVOX_CONFIG_MODE 20
#include "stb_voxel_renderer.hpp"

#include "PolyVoxCore/Array.h"

#define DISPLAY_CONSOLE 1

class DrawFrustum : public Components::IComponent
{
public:
    void addedToGameObject(GameObject* go) override
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
    void init() override
    {
       // stbvox_init_mesh_maker(mm);

        //   In mode 0 & mode 20, there is only one slot. The mesh data for that
        //   slot is two interleaved vertex attributes: attr_vertex, a single
        //   32-bit uint, and attr_face, a single 32-bit uint.
       // stbvox_set_buffer();
    }

    void tick(Time::Seconds delta) override
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

    void addedToGameObject(GameObject* go) override
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

    void tick(Time::Seconds delta) override
    {
        minimapCamTransform->position = transform->position;
        minimapCamTransform->position.y += m_heightOffset;
        //minimapCamTransform->lookAt(transform->position);

        //DEBUG.drawSphere(minimapCamTransform->position, 1.0f, Color::RED, 0);
    }
};

class MusicManager : public Components::IComponent
{
    ArrayList<AudioClipPtr> m_tracks;

    I32           m_curIndex        = 0;
    I32           m_nextIndex       = 1;
    Time::Seconds m_timer           = 0.0f;
    Time::Seconds m_transitionTime  = 5.0f;

public:
    MusicManager(const ArrayList<OS::Path>& musicFilePaths)
    {
        for (auto& path : musicFilePaths)
            m_tracks.push_back(ASSETS.getAudioClip(path));

        if ( not m_tracks.empty() )
            m_tracks[m_curIndex]->play();

        auto length = m_tracks[m_curIndex]->getLength();
        Time::Minutes mins = length;

        LOG(length.toString());
        LOG(mins.toString());
    }

    void tick(Time::Seconds delta) override
    {
        static bool startNextTrack = true;
        if ( m_tracks.size() <= 1 )
            return;

        m_timer += delta;

        if ( m_timer > (m_tracks[m_curIndex]->getLength() - m_transitionTime) )
        {
            if (startNextTrack)
            {
                m_tracks[m_nextIndex]->play();
                startNextTrack = false;
            }

            // Smoothly transition between next two tracks
            F32 volume = (F32)((m_tracks[m_curIndex]->getLength() - m_timer) / m_transitionTime).value;
            volume = Math::clamp( volume, 0.0f, 1.0f );

            m_tracks[m_curIndex]->setVolume( volume );
            m_tracks[m_nextIndex]->setVolume( 1.0f - volume );

            // Check when fully transitioned to next track and stop then
            if ( volume == 0.0f )
            {
                m_tracks[m_curIndex]->stop();
                m_curIndex      = m_nextIndex;
                m_nextIndex     = (m_nextIndex + 1) % m_tracks.size();
                m_timer         = 0.0f;
                startNextTrack  = true;
            }
        }
    }
};


class MyScene : public IScene
{
public:
    MyScene() : IScene("MyScene"){}

    void init() override
    {
        // CAMERA
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,0,-10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        go->addComponent<Minimap>(20.0f, 3.0f);
        go->addComponent<Components::AudioListener>();
        go->addComponent<MusicManager>(ArrayList<OS::Path>{"/audio/minecraft.wav", "/audio/minecraft2.wav"});

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

        // GAMEOBJECTS
        createGameObject("World Generation")->addComponent<WorldGeneration>();
        createGameObject("Cube")->addComponent<Components::MeshRenderer>(mesh, material);
    }

    void shutdown() override
    {
    }
};

class Game : public IGame
{
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