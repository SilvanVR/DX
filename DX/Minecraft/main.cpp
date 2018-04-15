#include <DX.h>
#define DISPLAY_CONSOLE 1

#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"

using namespace PolyVox;

void createSphereInVolume(SimpleVolume<uint8_t>& volData, float fRadius)
{
    //This vector hold the position of the center of the volume
    Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

    //This three-level for loop iterates over every voxel in the volume
    for (int z = 0; z < volData.getDepth(); z++)
    {
        for (int y = 0; y < volData.getHeight(); y++)
        {
            for (int x = 0; x < volData.getWidth(); x++)
            {
                //Store our current position as a vector...
                Vector3DFloat v3dCurrentPos(x, y, z);
                //And compute how far the current position is from the center of the volume
                float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

                uint8_t uVoxelValue = 0;

                //If the current voxel is less than 'radius' units from the center then we make it solid.
                if (fDistToCenter <= fRadius)
                {
                    //Our new voxel value
                    uVoxelValue = 255;
                }

                //Wrte the voxel value into the volume	
                volData.setVoxelAt(x, y, z, uVoxelValue);
            }
        }
    }
}

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

////**********************************************************************
//class WorldGeneration : public Components::IComponent
//{
//    stbvox_mesh_maker* mm;
//
//public:
//    void init() override
//    {
//       // stbvox_init_mesh_maker(mm);
//
//        //   In mode 0 & mode 20, there is only one slot. The mesh data for that
//        //   slot is two interleaved vertex attributes: attr_vertex, a single
//        //   32-bit uint, and attr_face, a single 32-bit uint.
//       // stbvox_set_buffer();
//    }
//
//    void tick(Time::Seconds delta) override
//    {
//
//    }
//};

//**********************************************************************
class WorldGeneration : public Components::IComponent
{
public:
    void addedToGameObject(GameObject* go) override
    {
        //Create an empty volume and then place a sphere in it
        SimpleVolume<uint8_t> volData(PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(63, 63, 63)));
        createSphereInVolume(volData, 30);

        //A mesh object to hold the result of surface extraction
        SurfaceMesh<PositionMaterialNormal> mesh;

        //Create a surface extractor. Comment out one of the following two lines to decide which type gets created.
        CubicSurfaceExtractorWithNormals< SimpleVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);
        //MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);

        //Execute the surface extractor.
        surfaceExtractor.execute();

        // BUILD MESH
        auto chunk = RESOURCES.createMesh();
        ArrayList<Math::Vec3> vertices;
        ArrayList<Math::Vec3> normals;
        ArrayList<Color> colors;
        for (auto vertex : mesh.getVertices())
        {
            vertices.emplace_back( vertex.getPosition().getX(), vertex.getPosition().getY(), vertex.getPosition().getZ() );
            normals.emplace_back( vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ());

            U8 material = static_cast<U8>( vertex.getMaterial() + 0.5 );

            colors.emplace_back(Math::Random::Color());
        }

        chunk->setVertices(vertices);        
        chunk->setIndices(mesh.getIndices());
        chunk->setColors(colors);
        chunk->setNormals(normals);

        auto shader = RESOURCES.createShader("TerrainShader", "/shaders/terrainVS.hlsl", "/shaders/terrainPS.hlsl");
        auto material = RESOURCES.createMaterial(shader);

        getGameObject()->addComponent<Components::MeshRenderer>(chunk, material);
    }

    void tick(Time::Seconds delta) override
    {
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
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,0,-10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        go->addComponent<Minimap>(500.0f, 3.0f);
        go->addComponent<Components::AudioListener>();

        // SHADER
        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");

        // TEXTURES
        auto dirt = ASSETS.getTexture2D("/textures/dirt.jpg");
        auto terrain = ASSETS.getTexture2D("/textures/terrain.png");
        terrain->setFilter(Graphics::TextureFilter::Point);
        terrain->setAnisoLevel(1);

        // MATERIAL
        auto material = RESOURCES.createMaterial(texShader);
        material->setTexture(SID("tex0"), terrain);
        material->setTexture(SID("tex1"), dirt);
        material->setFloat(SID("mix"), 0.0f);
        material->setColor(SID("tintColor"), Color::WHITE);

        // GAMEOBJECTS
        createGameObject("World Generation")->addComponent<WorldGeneration>();
        createGameObject("Cube")->addComponent<Components::MeshRenderer>(Core::Assets::MeshGenerator::CreateCubeUV(), material);
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
           //LOG( "Time: " + TS( Locator::getEngineClock().getTime().value ) + " FPS: " + TS( Locator::getProfiler().getFPS() ) );
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
           LOG("Time: " + TS(TIME.getTime().value) + " FPS: " + TS(fps) + " Delta: " + TS(delta) + " ms");
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