#include <DX.h>
#define DISPLAY_CONSOLE 1

#include "OS/FileSystem/virtual_file_system.h"

#ifdef _DEBUG
    const char* gameName = "[DEBUG] Minecraft";
#else
    const char* gameName = "[RELEASE] Minecraft";
#endif


#define NUM_MATERIALS 3

#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/LargeVolume.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Material.h"

void createCubeInVolume(PolyVox::LargeVolume<PolyVox::MaterialDensityPair44>& volData, 
                        PolyVox::Vector3DInt32 lowerCorner, PolyVox::Vector3DInt32 upperCorner, uint8_t uValue)
{
    uint8_t maxDen = PolyVox::MaterialDensityPair44::getMaxDensity();
    uint8_t minDen = PolyVox::MaterialDensityPair44::getMinDensity();
    //This three-level for loop iterates over every voxel between the specified corners
    for (int z = lowerCorner.getZ(); z <= upperCorner.getZ(); z++)
    {
        for (int y = lowerCorner.getY(); y <= upperCorner.getY(); y++)
        {
            for (int x = lowerCorner.getX(); x <= upperCorner.getX(); x++)
            {
                volData.setVoxelAt(x, y, z, PolyVox::MaterialDensityPair44(uValue, uValue > 0 ? maxDen : minDen));
            }
        }
    }
}

void createSphereInVolume(PolyVox::LargeVolume<PolyVox::Material8>& volData, float fRadius)
{
    //This vector hold the position of the center of the volume
    PolyVox::Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);
    //Vector3DFloat v3dVolCenter(0,0,0);

    //This three-level for loop iterates over every voxel in the volume
    for (int z = 0; z < volData.getDepth(); z++)
    {
        for (int y = 0; y < volData.getHeight(); y++)
        {
            for (int x = 0; x < volData.getWidth(); x++)
            {
                //Store our current position as a vector...
                PolyVox::Vector3DFloat v3dCurrentPos(x, y, z);
                //And compute how far the current position is from the center of the volume
                float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

                uint8_t uVoxelValue = 0;

                //If the current voxel is less than 'radius' units from the center then we make it solid.
                if (fDistToCenter <= fRadius)
                {
                    //Our new voxel value
                    uVoxelValue = Math::Random::Int(1, NUM_MATERIALS);
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
    MaterialPtr material;

    Texture2DArrayPtr texArr;

public:
    void addedToGameObject(GameObject* go) override
    {
        using namespace PolyVox;

        //Create an empty volume and then place a sphere in it
        PolyVox::LargeVolume<Material8> volData(PolyVox::Region(PolyVox::Vector3DInt32(0, 0, 0), PolyVox::Vector3DInt32(63, 63, 63)));
        createSphereInVolume(volData, 30);

        //I32 volumeSideLength = 10;
        //LargeVolume<MaterialDensityPair44> volData(Region(Vector3DInt32(0, 0, 0), 
        //                                           Vector3DInt32(volumeSideLength - 1, volumeSideLength - 1, volumeSideLength - 1)));

        ////Make our volume contain a sphere in the center.
        //I32 minPos = 0;
        //I32 midPos = volumeSideLength / 2;
        //I32 maxPos = volumeSideLength - 1;

        //createCubeInVolume(volData, Vector3DInt32(minPos, minPos, minPos), Vector3DInt32(midPos - 1, midPos - 1, midPos - 1), 0);

        //A mesh object to hold the result of surface extraction
        PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> mesh;

        //Create a surface extractor. 
        PolyVox::CubicSurfaceExtractorWithNormals< PolyVox::LargeVolume<Material8> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);
        //PolyVox::MarchingCubesSurfaceExtractor< PolyVox::SimpleVolume<U8> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);

        //Execute the surface extractor.
        surfaceExtractor.execute();

        // BUILD MESH
        auto chunk = RESOURCES.createMesh();
        ArrayList<Math::Vec3> vertices;
        ArrayList<Math::Vec3> normals;
        ArrayList<Math::Vec2> materials;
        ArrayList<Color> colors;
        for (auto vertex : mesh.getVertices())
        {
            vertices.emplace_back( vertex.getPosition().getX(), vertex.getPosition().getY(), vertex.getPosition().getZ() );
            normals.emplace_back( vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ());

            U8 material = static_cast<U8>( vertex.getMaterial() + 0.5 ) - 1;
            materials.emplace_back(material);

            colors.emplace_back(Math::Random::Color());
        }

        chunk->setVertices(vertices);
        chunk->setIndices(mesh.getIndices());
        chunk->setColors(colors);
        chunk->setNormals(normals);
        chunk->setUVs(materials);

        // SHADER + MATERIAL
        auto shader = RESOURCES.createShader("TerrainShader", "/shaders/terrainVS.hlsl", "/shaders/terrainPS.hlsl");

        auto tex = ASSETS.getTexture2D("/textures/blocks/dirt.png");
        auto tex2 = ASSETS.getTexture2D("/textures/blocks/brick.png");
        auto tex3 = ASSETS.getTexture2D("/textures/blocks/stone.png");

        texArr = RESOURCES.createTexture2DArray( tex->getWidth(), tex->getHeight(), NUM_MATERIALS, Graphics::TextureFormat::RGBA32 );
        texArr->setPixels( 0, tex->getPixels().data() );
        texArr->setPixels( 1, tex2->getPixels().data() );
        texArr->setPixels( 2, tex3->getPixels().data() );
        texArr->apply();
        texArr->setAnisoLevel(1);
        texArr->setFilter(Graphics::TextureFilter::Point);

        material = RESOURCES.createMaterial(shader);
        material->setColor("color", Color::WHITE);
        material->setVec4("dir", Math::Vec4(0, -1, 1, 0));
        material->setFloat("intensity", 1.0f);
        material->setTexture("texArray", texArr);

        getGameObject()->addComponent<Components::MeshRenderer>(chunk, material);
        getGameObject()->getComponent<Components::Transform>()->position = (-volData.getWidth() / 2.0f, -volData.getHeight() / 2.0f, -volData.getDepth() / 2.0f);
    }

    void tick(Time::Seconds delta) override
    {
        static F64 yaw = 0;
        yaw += 45.0 * delta.value;
        auto fw = Math::Quat::FromEulerAngles({ 0, (F32)yaw, 0}).getForward();
        material->setVec4("dir", Math::Vec4(fw.x, -fw.y, fw.z, 0));

        //Math::Vec3 start{0,15,0};
        //DEBUG.drawLine(start, start + fw * 10, Color::RED, 0);
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
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,0,-20);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        //go->addComponent<Minimap>(500.0f, 3.0f);
        go->addComponent<Components::AudioListener>();

        //auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
        //    "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
        //    "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        //go->addComponent<Components::Skybox>(cubemap);

        // SHADER
        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");

        // TEXTURES
        auto terrain = ASSETS.getTexture2D("/textures/terrain.png");
        terrain->setFilter(Graphics::TextureFilter::Point);
        terrain->setAnisoLevel(1);

        // MESH
        auto mesh = Core::Assets::MeshGenerator::CreateCubeUV();
        mesh->setColors({Color::WHITE});

        // MATERIAL
        auto material = RESOURCES.createMaterial(texShader);
        material->setTexture("tex0", terrain);
        material->setTexture("tex1", terrain);
        material->setFloat("mix", 0.0f);
        material->setColor("tintColor", Color::WHITE);

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
        OS::VirtualFileSystem::mount("textures", "res/textures");
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