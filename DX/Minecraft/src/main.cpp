#include <DX.h>
#define DISPLAY_CONSOLE 1

#include "world_generator.h"

#ifdef _DEBUG
    const char* gameName = "[DEBUG] Minecraft";
#else
    const char* gameName = "[RELEASE] Minecraft";
#endif


//**********************************************************************
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

//**********************************************************************
class Minimap : public Components::IComponent
{
    Components::Transform* minimapCamTransform;
    Components::Transform* transform;
    F32 m_heightOffset;
    F32 m_size;

public:
    Minimap(F32 heightOffset, F32 size) : m_heightOffset(heightOffset), m_size(size) {}

    void addedToGameObject(GameObject* go) override
    {
        auto minimapCamGo = go->getScene()->createGameObject();
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
        minimapCamTransform->position.x = transform->position.x;
        minimapCamTransform->position.z = transform->position.z;
    }
};

//**********************************************************************
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

class PlayerController : public Components::IComponent
{
    F32 rayDistance;
    Components::Transform* transform;

public:
    PlayerController(F32 placeDistance = 10.0f) : rayDistance(placeDistance) {}

    void addedToGameObject(GameObject* go) override
    {
        transform = getComponent<Components::Transform>();
    }

    void tick(Time::Seconds delta) override
    {
        auto viewerDir = transform->rotation.getForward();

        Physics::Ray ray(transform->position, viewerDir * rayDistance);
        World::Get().RayCast(ray, [](const ChunkRayCastResult& result){
            if (result.block != Block::Air)
            {
                Math::Vec3 blockSize(BLOCK_SIZE * 0.5f);
                Math::Vec3 min = result.blockCenter - blockSize;
                Math::Vec3 max = result.blockCenter + blockSize;
                DEBUG.drawCube(min, max, Color::GREEN, 0);
            }
        });

        // Remove blocks
        if ( MOUSE.wasKeyPressed(MouseKey::LButton) )
        {
            World::Get().RayCast(ray, [=](const ChunkRayCastResult& result) {
                if (result.block != Block::Air)
                {
                    World::Get().SetVoxelAt((I32)result.blockCenter.x, (I32)result.blockCenter.y, (I32)result.blockCenter.z, Block::Air);
                    //DEBUG.drawSphere(result.hitPoint, 0.1f, Color::RED, 10);
                    //DEBUG.drawRay(transform->position, ray.getDirection(), Color::BLUE, 10);
                }
            });
        }

        // Place block
        if ( KEYBOARD.wasKeyPressed(Key::E) )
        {
            World::Get().RayCast(ray, [=](const ChunkRayCastResult& result) {
                if (result.block != Block::Air)
                {
                    Math::Vec3 hitDir = result.hitPoint - result.blockCenter;

                    // Calculate where to place the new block by checking which component has the largest abs.
                    Math::Vec3 axis(0, 0, hitDir.z);
                    F32 xAbs = std::abs(hitDir.x);
                    F32 yAbs = std::abs(hitDir.y);
                    F32 zAbs = std::abs(hitDir.z);
                    if (xAbs > yAbs && xAbs > zAbs)
                        axis = Math::Vec3(hitDir.x, 0, 0);
                    else if (yAbs > xAbs && yAbs > zAbs)
                        axis = Math::Vec3(0, hitDir.y, 0);

                    // Set world voxel
                    Math::Vec3 block = result.blockCenter + axis.normalized();
                    World::Get().SetVoxelAt((I32)block.x, (I32)block.y, (I32)block.z, Block::Sand);
                }
            });
        }
    }
};

//**********************************************************************
class MyScene : public IScene
{
public:
    MyScene() : IScene("MyScene"){}

    void init() override
    {
        // CAMERA
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,0,-5);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        //go->addComponent<Minimap>(500.0f, 50.0f);
        go->addComponent<Components::AudioListener>();
        cam->setClearColor(Color::BLUE);

        F32 placeDistance = 10.0f;
        go->addComponent<PlayerController>(placeDistance);

        //auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
        //    "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
        //    "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        //go->addComponent<Components::Skybox>(cubemap);

        // GAMEOBJECTS
        I32 chunkViewDistance = 8;
        auto worldGenerator = createGameObject("World Generation")->addComponent<WorldGeneration>(chunkViewDistance);
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
        OS::VirtualFileSystem::mount("textures", "res/textures");
        OS::VirtualFileSystem::mount("shaders", "res/shaders");
        gLogger->setSaveToDisk( false );

        Locator::getEngineClock().setInterval([] {
           //LOG( "Time: " + TS( Locator::getEngineClock().getTime().value ) + " FPS: " + TS( Locator::getProfiler().getFPS() ) );
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
            String newTitle = String(gameName) + " | Time: " + TS(TIME.getTime().value) + " | Delta: " + TS(delta) + "ms (" + TS(fps) + " FPS)";
            Locator::getWindow().setTitle(newTitle.c_str());
        }, 1000);

        getWindow().setCursor( "../dx/res/internal/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "../dx/res/internal/icon.ico" );

        // Create a gameobject in the default scene with a new component and add new scene onto the stack.
        // This way the music manager will stay alife during the whole program.
        SCENE.createGameObject("MusicManager")->addComponent<MusicManager>(ArrayList<OS::Path>{"/audio/minecraft.wav", "/audio/minecraft2.wav"});

        Locator::getSceneManager().PushSceneAsync( new MyScene(), false );
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        if (KEYBOARD.wasKeyPressed(Key::One))
            Locator::getSceneManager().LoadSceneAsync(new MyScene);
        if (KEYBOARD.wasKeyPressed(Key::P))
            PROFILER.logGPU();
        if (KEYBOARD.wasKeyPressed(Key::F1))
            Locator::getRenderer().setGlobalMaterialActive("NONE");
        if (KEYBOARD.wasKeyPressed(Key::F2))
            Locator::getRenderer().setGlobalMaterialActive("Wireframe");
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
        game.start(gameName, 800, 600);

        return 0;
    }

#endif