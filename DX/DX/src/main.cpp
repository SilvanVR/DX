#include "OS/PlatformTimer/platform_timer.h"
#include "Memory/Allocators/include.hpp"
#include "Core/subsystem_manager.h"

#include "OS/FileSystem/file_system.h"
#include "OS/Threading/thread_pool.h"
#include "OS/FileSystem/file.h"
#include "i_game.hpp"
#include "Time/clock.h"

#include "locator.h"
#include "Common/color.h"
#include "GameplayLayer/Components/fps_camera.h"

#include "Core/Assets/mesh_generator.h"
#include "Math/random.h"

using namespace Core;

class AutoClock
{
    I64 begin;

public:
    AutoClock()
    {
        begin = OS::PlatformTimer::getTicks();
    }

    ~AutoClock()
    {
        I64 elapsedTicks = OS::PlatformTimer::getTicks() - begin;

        F64 elapsedSeconds = OS::PlatformTimer::ticksToSeconds(elapsedTicks);
        LOG("Seconds: " + TS(elapsedSeconds));

        F64 elapsedMillis = OS::PlatformTimer::ticksToMilliSeconds(elapsedTicks);
        LOG("Millis: " + TS(elapsedMillis));

        F64 elapsedMicros = OS::PlatformTimer::ticksToMicroSeconds(elapsedTicks);
        LOG("Micros: " + TS(elapsedMicros));

        F64 elapsedNanos = OS::PlatformTimer::ticksToNanoSeconds(elapsedTicks);
        LOG("Nanos: " + TS(elapsedNanos));
    }
};

ArrayList<Color> cubeColors =
{
    Color(0, 0, 0),
    Color(0, 255, 0),
    Color(255, 255, 0),
    Color(255, 0, 0),
    Color(0, 0, 255),
    Color(0, 255, 255),
    Color(255, 255, 255),
    Color(255, 0, 255)
};

ArrayList<Color> planeColors =
{
    Color(0, 0, 255),
    Color(0, 255, 0),
    Color(255, 0, 0),
    Color(255, 255, 0)
};

//----------------------------------------------------------------------
// COMPONENTS
//----------------------------------------------------------------------

class ConstantRotation : public Components::IComponent
{
    Math::Vec3 m_speeds = Math::Vec3(0.0f);
    Math::Vec3 m_curDegrees = Math::Vec3(0.0f);
public:
    // Attached gameobject rotates "{degree,degree,degree}" per second around the respective axis.
    ConstantRotation(F32 pitchSpeed, F32 yawSpeed, F32 rollSpeed) 
        : m_speeds(Math::Vec3{ pitchSpeed, yawSpeed, rollSpeed }) {}

    void Tick( Time::Seconds delta ) override
    {
        auto t = getGameObject()->getComponent<Components::Transform>();
        t->rotation = Math::Quat::FromEulerAngles( m_curDegrees );
        m_curDegrees += m_speeds * (F32)delta.value;
    }
};

class WorldGeneration : public Components::IComponent
{
    MeshPtr                     mesh;
    Components::MeshRenderer*   mr;

public:
    void AddedToGameObject(GameObject* go) override
    {
        mesh = Assets::MeshGenerator::CreatePlane();
        mesh->setColors(planeColors);
        mesh->setBufferUsage(Graphics::BufferUsage::Frequently);

        mr = go->addComponent<Components::MeshRenderer>();
        mr->setMesh(mesh);
    }

    void Tick(Time::Seconds delta)
    {
        auto newVertices = mesh->getVertices();
        auto indices = mesh->getIndices();

        int i = 0;
        while (i < newVertices.size())
        {
            F32 newZ = newVertices[i].z = (F32)sin(TIME.getTime().value);
            newVertices[i].z = i % 2 == 0 ? newZ : -newZ;
            i++;
        }

        auto newColors = mesh->getColors();
        for(auto& color : newColors)
            color.setRed( (Byte) ( (sin(TIME.getTime().value) + 1) / 2 * 255 ) );

        mesh->setVertices( newVertices );
        mesh->setColors( newColors );
    }
};

class VertexGeneration : public Components::IComponent
{
    MeshPtr                     mesh;
    Components::MeshRenderer*   mr;

    const U32 width  = 20;
    const U32 height = 20;

public:
    void AddedToGameObject(GameObject* go) override
    {
        generateMesh();
        auto transform = go->getComponent<Components::Transform>();
        transform->rotation = Math::Quat(Math::Vec3::RIGHT, 90);
        transform->position = Math::Vec3(-(width/2.0f), -2.0f, -(height/2.0f));

        mr = go->addComponent<Components::MeshRenderer>();
        mr->setMesh(mesh);
    }

    void Tick(Time::Seconds delta)
    {
        auto newVertices = mesh->getVertices();
        auto indices = mesh->getIndices();

        int i = 0;
        while (i < newVertices.size())
        {
            F32 newY = (F32)sin(TIME.getTime().value);
            newVertices[i].z = (i % 2 == 0 ? newY : -newY);
            i++;
        }

        auto newColors = mesh->getColors();
        for (auto& color : newColors)
            color.setBlue((Byte)((sin(TIME.getTime().value) + 1) / 2 * 255));

        mesh->setVertices(newVertices);
        mesh->setColors(newColors);
    }

private:
    void generateMesh()
    {
        mesh = Assets::MeshGenerator::CreatePlane(width, height);
        mesh->setBufferUsage(Graphics::BufferUsage::Frequently);

        ArrayList<Color> m_colors;
        for (U32 i = 0; i < mesh->getVertexCount(); i++)
            m_colors.push_back( Math::Random::Color() );
        mesh->setColors( m_colors );
    }
};

class GridGeneration : public Components::IComponent
{
    U32 m_size;

public:
    GridGeneration(U32 size) : m_size(size) {}

    void AddedToGameObject(GameObject* go) override
    {
        auto mesh = Assets::MeshGenerator::CreateGrid(m_size);
        go->addComponent<Components::MeshRenderer>(mesh);
    }

private:
};

class AutoOrbiting : public Components::IComponent
{
    F32 m_speed;
    F32 m_curDegrees;
    Math::Vec3 m_center;

public:
    // Attached gameobject rotates "yawSpeed" (in degrees) per second around the center.
    AutoOrbiting(F32 yawSpeed, Math::Vec3 center = Math::Vec3())
        : m_speed( yawSpeed ), m_center(center) {}

    void Tick(Time::Seconds delta) override
    {
        auto t = getGameObject()->getComponent<Components::Transform>();

        Math::Vec3 vecXZ = t->position;
        vecXZ.y = 0.0f;
        F32 length = vecXZ.magnitude();

        t->position.x = m_center.x + length * cos( Math::deg2Rad(m_curDegrees) );
        t->position.z = m_center.z + length * sin( Math::deg2Rad(m_curDegrees) );

        t->lookAt(m_center);

        m_curDegrees += m_speed * (F32)delta.value;
    }
};

class DrawFrustumDebug : public Components::IComponent
{
public:
    void Tick(Time::Seconds delta) override
    {
        auto cam = getGameObject()->getComponent<Components::Camera>();
        auto transform = getGameObject()->getComponent<Components::Transform>();

        auto up = transform->rotation.getUp();
        auto right = transform->rotation.getRight();
        auto fw = transform->rotation.getForward();

        DEBUG.drawFrustum(transform->position, up, right, fw, cam->getFOV(), cam->getZNear(), cam->getZFar(), cam->getAspectRatio(), Color::RED, 0, true);
    }
};

class DrawFrustum : public Components::IComponent
{
public:
    void AddedToGameObject(GameObject* go) override
    {
        auto cam = go->getComponent<Components::Camera>();
        auto mesh = Assets::MeshGenerator::CreateFrustum( Math::Vec3(0), Math::Vec3::UP, Math::Vec3::RIGHT, Math::Vec3::FORWARD, 
                                                          cam->getFOV(), cam->getZNear(), cam->getZFar(), cam->getAspectRatio() );

        auto mr = go->addComponent<Components::MeshRenderer>();
        mr->setMesh( mesh );
    }
};

//----------------------------------------------------------------------
// SCENES
//----------------------------------------------------------------------

class SceneCameras : public IScene
{
    GameObject* go;
    GameObject* go2;

    Components::Camera* cam;

public:
    SceneCameras() : IScene("SceneCameras") {}

    void init() override
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        cam->setCameraMode(Components::Camera::ORTHOGRAPHIC);
        F32 size = 5.0f;
        cam->setOrthoParams(-size, size, -size, size, 0.1f, 100.0f);

        auto sphere = Assets::MeshGenerator::CreateUVSphere(30,30);
        ArrayList<Color> sphereColors;
        for (U32 i = 0; i < sphere->getVertexCount(); i++)
            sphereColors.push_back(Math::Random::Color());
        sphere->setColors(sphereColors);

        go2 = createGameObject("Mesh");
        go2->addComponent<Components::MeshRenderer>(sphere);
        go2->addComponent<ConstantRotation>(0.0f, 20.0f, 0.0f);

        auto& viewport = cam->getViewport();
        viewport.width  = 0.5f;
        viewport.height = 0.5f;

        {
            // CAMERA 2
            auto go3 = createGameObject("Camera2");
            auto cam2 = go3->addComponent<Components::Camera>();
            go3->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -5);
            cam2->setClearMode(Components::Camera::EClearMode::NONE);

            auto& viewport2 = cam2->getViewport();
            viewport2.topLeftX = 0.5f;
            viewport2.width = 0.5f;
            viewport2.height = 0.5f;

            // CAMERA 3
            auto go4 = createGameObject("Camera3");
            auto cam3 = go4->addComponent<Components::Camera>();
            go4->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, 5);
            go4->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
            cam3->setClearMode(Components::Camera::EClearMode::NONE);

            auto& viewport3 = cam3->getViewport();
            viewport3.topLeftY = 0.5f;
            viewport3.width = 0.5f;
            viewport3.height = 0.5f;

            // CAMERA 4
            auto go5 = createGameObject("Camera4");
            auto cam4 = go5->addComponent<Components::Camera>();
            go5->getComponent<Components::Transform>()->position = Math::Vec3(0, -5, -5);
            go5->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
            cam4->setClearMode(Components::Camera::EClearMode::NONE);

            auto& viewport4 = cam4->getViewport();
            viewport4.topLeftY = 0.5f;
            viewport4.topLeftX = 0.5f;
            viewport4.width = 0.5f;
            viewport4.height = 0.5f;
        }

        //bool removed = go2->removeComponent( c );
        //bool destroyed = go2->removeComponent<Components::Transform>();

        LOG("SceneCameras initialized!", Color::RED);
    }

    void tick(Time::Seconds delta)
    {
        if(KEYBOARD.wasKeyPressed(Key::T))
            go2->removeComponent<Components::MeshRenderer>();
    }

    void shutdown() override
    {
        LOG("SceneCameras Shutdown!", Color::RED);
    }
};

class VertexGenScene : public IScene
{
    GameObject* goModel;

public:
    VertexGenScene() : IScene("VertexGenScene"){}

    void init() override
    {
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,10,-25);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        go->addComponent<AutoOrbiting>(15.0f);

        auto worldGO = createGameObject("World");
        worldGO->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, 0);
        worldGO->addComponent<WorldGeneration>();

        auto wavesGO = createGameObject("Waves");
        wavesGO->addComponent<VertexGeneration>();

        auto cube = Assets::MeshGenerator::CreateCube(1.0f);
        cube->setColors(cubeColors);

        auto plane = Assets::MeshGenerator::CreatePlane();
        plane->setColors(planeColors);

        auto sphere = Assets::MeshGenerator::CreateUVSphere(10,10);
        ArrayList<Color> sphereColors;
        for(U32 i = 0; i < sphere->getVertexCount(); i++)
            sphereColors.push_back(Math::Random::Color());
        sphere->setColors(sphereColors);

        {
            goModel = createGameObject("Test");
            goModel->addComponent<ConstantRotation>(0.0f, 20.0f, 20.0f);
            auto mr = goModel->addComponent<Components::MeshRenderer>(cube);

            GameObject* goModel2 = createGameObject("Test");
            goModel2->getComponent<Components::Transform>()->position = {5,0,0};
            goModel2->addComponent<ConstantRotation>(20.0f, 20.0f, 0.0f);
            mr = goModel2->addComponent<Components::MeshRenderer>(sphere);

            GameObject* goModel3 = createGameObject("Test");
            goModel3->getComponent<Components::Transform>()->position = { -5,0,0 };
            goModel3->addComponent<ConstantRotation>(0.0f, 0.0f, 20.0f);
            mr = goModel3->addComponent<Components::MeshRenderer>(plane);
        }

        {
            //auto cam2GO = createGameObject("Camera2");
            //cam2GO->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, 10);
            //cam2GO->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
            //auto cam2 = cam2GO->addComponent<Components::Camera>();
            //cam2->setClearMode(Components::Camera::EClearMode::NONE);
            //cam2->getViewport().topLeftX = 0.5f;
            //cam2->getViewport().width = 0.5f;
        }

        LOG("VertexGenScene initialized!", Color::RED);
    }

    void tick( Time::Seconds d ) override
    {
        static F32 speed = 50.0f;
        F32 delta = (F32)d.value;
        if (KEYBOARD.isKeyDown(Key::Add))
            goModel->getComponent<Components::Transform>()->scale.y += speed * delta;
        if (KEYBOARD.isKeyDown(Key::Subtract))
            goModel->getComponent<Components::Transform>()->scale.y -= speed * delta;
    }

    void shutdown() override { LOG("VertexGenScene Shutdown!", Color::RED); }

};

class ManyObjectsScene : public IScene
{
    U32 m_numObjects;

public:
    ManyObjectsScene(U32 numObjects) : IScene("MaterialTestScene"), m_numObjects(numObjects) {}

    void init() override
    {
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);

        // MESH
        auto cube = Assets::MeshGenerator::CreateCube(1.0f);
        cube->setColors(cubeColors);

        U32 sq = (U32)sqrt(m_numObjects);

        for (U32 i = 0; i < m_numObjects; i++)
        {
            auto go = createGameObject("Test");
            go->addComponent<Components::MeshRenderer>(cube);
            go->getComponent<Components::Transform>()->position = Math::Random::Vec3(-1,1).normalized() * sqrtf((F32)m_numObjects);
        }

        // GAMEOBJECTs
        //for (U32 i = 0; i < sq; i++)
        //{
        //    for (U32 j = 0; j < sq; j++)
        //    {
        //        auto go = createGameObject("Test");
        //        go->addComponent<Components::MeshRenderer>(cube);
        //        go->getComponent<Components::Transform>()->position = Math::Vec3(i * 3.0f, 0, j * 3.0f);
        //    }
        //}
        LOG("ManyObjectsScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("ManyObjectsScene Shutdown!", Color::RED); }
};

class MaterialTestScene : public IScene
{
    MaterialPtr material;

    Texture2DPtr tex;
    Texture2DPtr tex2;

public:
    MaterialTestScene() : IScene("MaterialTestScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        //go->addComponent<AutoOrbiting>(10.0f);

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        go->addComponent<Components::Skybox>(cubemap);

        // Camera 2
        auto renderTex = RESOURCES.createRenderTexture();
        renderTex->create(400, 400, 24, Graphics::TextureFormat::BGRA32);
        auto cam2GO = createGameObject("Camera2");
        cam2GO->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, -10);
        cam2GO->addComponent<AutoOrbiting>(10.0f);

        auto cam2 = cam2GO->addComponent<Components::Camera>();
        cam2->setRenderTarget(renderTex);
        cam2->setClearColor(Color::GREEN);

        auto grid = createGameObject("Grid");
        grid->addComponent<GridGeneration>(20);

        // MESH
        auto cube = Assets::MeshGenerator::CreateCubeUV();
        cube->setColors(cubeColors);

        auto plane = Assets::MeshGenerator::CreatePlane();
        plane->setColors(planeColors);

        // SHADER
        auto texShader = RESOURCES.createShader( "TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");

        // TEXTURES
        tex = RESOURCES.createTexture2D(4, 4, Graphics::TextureFormat::RGBA32);
        for (U32 x = 0; x < tex->getWidth(); x++)
            for (U32 y = 0; y < tex->getHeight(); y++)
                tex->setPixel( x, y, Math::Random::Color() );
        tex->apply();
        tex->setFilter(Graphics::TextureFilter::Point);

        tex2 = ASSETS.getTexture2D("/textures/nico.jpg");
        auto dirt = ASSETS.getTexture2D("/textures/dirt.jpg");

        // MATERIAL
        material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture( SID("tex0"), tex2);
        material->setTexture( SID("tex1"), dirt );
        material->setFloat( SID("mix"), 0.0f);
        material->setColor( SID("tintColor"), Color::WHITE );

        auto dirtMaterial = RESOURCES.createMaterial();
        dirtMaterial->setShader(texShader);
        dirtMaterial->setTexture(SID("tex0"), dirt);
        dirtMaterial->setColor(SID("tintColor"), Color::WHITE);

        auto customTexMaterial = RESOURCES.createMaterial();
        customTexMaterial->setShader(texShader);
        customTexMaterial->setTexture(SID("tex0"), renderTex);
        customTexMaterial->setFloat(SID("mix"), 0.0f);
        customTexMaterial->setColor(SID("tintColor"), Color::WHITE);

        // GAMEOBJECT
        auto goModel = createGameObject("Test");
        //goModel->addComponent<ConstantRotation>(0.0f, 20.0f, 20.0f);
        auto mr = goModel->addComponent<Components::MeshRenderer>(plane, material);

        auto go2 = createGameObject("Test2");
        go2->addComponent<Components::MeshRenderer>(cube, dirtMaterial);
        go2->getComponent<Components::Transform>()->position = Math::Vec3(3, 0, 0);

        auto go3 = createGameObject("Test3");
        go3->addComponent<Components::MeshRenderer>(plane, customTexMaterial);
        go3->getComponent<Components::Transform>()->position = Math::Vec3(0, 2, 0);

        go->addComponent<Components::MeshRenderer>(cube, dirtMaterial);

        LOG("MaterialTestScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
        //if(KEYBOARD.wasKeyPressed(Key::NumPad1))
        //    tex2->setClampMode(Graphics::TextureAddressMode::Clamp);
        //if (KEYBOARD.wasKeyPressed(Key::NumPad2))
        //    tex2->setClampMode(Graphics::TextureAddressMode::Mirror);
        //if (KEYBOARD.wasKeyPressed(Key::NumPad3))
        //    tex2->setClampMode(Graphics::TextureAddressMode::MirrorOnce);
        //if (KEYBOARD.wasKeyPressed(Key::NumPad4))
        //    tex2->setClampMode(Graphics::TextureAddressMode::Repeat);

        if (KEYBOARD.wasKeyPressed(Key::NumPad1))
            RESOURCES.setGlobalAnisotropicFiltering(1);
        if (KEYBOARD.wasKeyPressed(Key::NumPad2))
            RESOURCES.setGlobalAnisotropicFiltering(4);
        if (KEYBOARD.wasKeyPressed(Key::NumPad3))
            RESOURCES.setGlobalAnisotropicFiltering(8);
    }

    void shutdown() override
    {
        LOG("MaterialTestScene Shutdown!", Color::RED);
    }
};

class CubemapScene : public IScene
{
public:
    CubemapScene() : IScene("CubemapScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        //go->addComponent<AutoOrbiting>(10.0f);
        //go->addComponent<Components::Skybox>(cubemap);

        auto grid = createGameObject("Grid");
        grid->addComponent<GridGeneration>(20);

        // MESH
        auto sphere = Assets::MeshGenerator::CreateCubeUV();

        // SHADER
        auto texShader = RESOURCES.createShader("Skybox", "/shaders/skyboxVS.hlsl", "/shaders/skyboxPS.hlsl");
        texShader->setRasterizationState({ Graphics::FillMode::Solid, Graphics::CullMode::None });

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture(SID("Cubemap"), RESOURCES.getDefaultCubemap());
        material->setColor(SID("tintColor"), Color::WHITE);

        // GAMEOBJECT
        auto go2 = createGameObject("Test2");
        go2->addComponent<Components::MeshRenderer>(sphere, material);
        go2->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, 0);

        LOG("CubemapScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("CubemapScene Shutdown!", Color::RED); }
};

class TexArrayScene : public IScene
{
    Texture2DPtr        tex;
    Texture2DArrayPtr   arr;

public:
    TexArrayScene() : IScene("TexArrayScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);
        //go->addComponent<Components::Skybox>(cubemap);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // MESH
        auto plane = Assets::MeshGenerator::CreatePlane();

        // SHADER
        auto texShader = RESOURCES.createShader("TextureArray", "/shaders/arrayTexVS.hlsl", "/shaders/arrayTexPS.hlsl");

        // TEXTURES
        tex = ASSETS.getTexture2D("/textures/dirt.jpg");
        //auto tex2 = Assets::Importer::LoadTexture("/textures/nico.jpg");

        Color cols[] = { Color::RED, Color::BLUE, Color::GREEN };

        const I32 slices = 3;
        const I32 size = 512;
        ArrayList<ArrayList<Color>> colors;
        colors.resize(slices);

        arr = RESOURCES.createTexture2DArray(size, size, slices, Graphics::TextureFormat::RGBA32, false);
        for (I32 slice = 0; slice < slices; slice++)
        {
            colors[slice].resize(size * size, cols[slice]);
            arr->setPixels(slice, colors[slice].data());
        }
        arr->apply();
        arr->setFilter(Graphics::TextureFilter::Point);

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture(SID("texArray"), arr);
        material->setInt(SID("texIndex"), 0);

        auto material2 = RESOURCES.createMaterial();
        material2->setShader(texShader);
        material2->setTexture(SID("texArray"), arr);
        material2->setInt(SID("texIndex"), 1);

        auto material3 = RESOURCES.createMaterial();
        material3->setShader(texShader);
        material3->setTexture(SID("texArray"), arr);
        material3->setInt(SID("texIndex"), 2);

        // GAMEOBJECT
        auto go2 = createGameObject("Test2");
        go2->addComponent<Components::MeshRenderer>(plane, material);

        auto go3 = createGameObject("Test3");
        go3->addComponent<Components::MeshRenderer>(plane, material2);
        go3->getComponent<Components::Transform>()->position.x = -2;

        auto go4 = createGameObject("Test4");
        go4->addComponent<Components::MeshRenderer>(plane, material3);
        go4->getComponent<Components::Transform>()->position.x = 2;

        LOG("TexArrayScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("TexArrayScene Shutdown!", Color::RED); }
};


class TestScene : public IScene
{
    GameObject* go2;
    MaterialPtr material;

public:
    TestScene() : IScene("TestScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);

        auto go3 = createGameObject("Camera2");
        auto cam2 = go3->addComponent<Components::Camera>();
        go3->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, -10);
        go3->addComponent<AutoOrbiting>(10.0f);

        cam2->setClearMode(Components::Camera::EClearMode::NONE);
        cam2->setZFar(20.0f);
        cam2->getViewport().width  = 0.25f;
        cam2->getViewport().height = 0.25f;

        go3->addComponent<DrawFrustum>();

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // SHADER
        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");

        // MATERIAL
        material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture(SID("tex0"), ASSETS.getTexture2D("/textures/dirt.jpg"));
        material->setTexture(SID("tex1"), ASSETS.getTexture2D("/textures/nico.jpg"));
        material->setFloat(SID("mix"), 0.0f);
        material->setColor(SID("tintColor"), Color::WHITE);

        //Assets::Importer::LoadTextureAsync("/textures/4k.jpg", [=](Texture2DPtr tex){  material->setTexture(SID("tex0"), tex); });

        // MESH
        auto mesh = Assets::MeshGenerator::CreateCubeUV();
        mesh->setColors(cubeColors);

        // GAMEOBJECT
        go2 = createGameObject("Test2");
        go2->addComponent<Components::MeshRenderer>(mesh, material);

        LOG("TestScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.wasKeyPressed(Key::L))
        {
            ASYNC_JOB([=] {
                auto tex = ASSETS.getTexture2D("/textures/4k.jpg");
                material->setTexture(SID("tex0"), tex);
            });
        }
    }

    void shutdown() override { LOG("TestScene Shutdown!", Color::RED); }

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
        getWindow().setIcon( "/internal/icon.ico" );
        gLogger->setSaveToDisk( false );

        Locator::getEngineClock().setInterval([=] {
            //terminate();
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
            LOG("Time: " + TS( TIME.getTime().value ) + " FPS: " + TS( fps ) + " Delta: " + TS( delta ) + " ms" );
            //LOG("Num Scenes: " + TS(Locator::getSceneManager().numScenes()));
        }, 1000);

        IGC_SET_VAR("test", 1.0f);

        Locator::getSceneManager().LoadSceneAsync(new TestScene());
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        getWindow().setTitle( PROFILER.getUpdateDelta().toString().c_str() );

        //clock.tick( delta );
        //LOG( TS( clock.getTime().value ) );
        //auto time = clock.getTime();

        if (KEYBOARD.wasKeyPressed(Key::T))
            LOG("Num Textures: " + TS(RESOURCES.getTextureCount()));

        static bool inState = false;
        if (KEYBOARD.wasKeyPressed(Key::Q)) {
            inState = !inState;
        }
        if (inState){
        }

        if (KEYBOARD.wasKeyPressed(Key::E))
        {
            auto g = SCENE.findGameObject("Test");
            static bool enabled = true;
            /*enabled = !enabled;
            g->setActive(enabled);
            LOG(TS(enabled), Color::BLUE);*/
        }

        if (KEYBOARD.wasKeyPressed(Key::G))
            DEBUG.drawLine({ 0,0,0 }, { 10,10,10 }, Color::RED, 2);
        if (KEYBOARD.wasKeyPressed(Key::H))
            DEBUG.drawSphere({ 0,5,0 }, 10, Color::BLUE, 5);
        if (KEYBOARD.wasKeyPressed(Key::F))
            DEBUG.drawRay({ 5,5,5 }, { 0,100,0 }, Color::GREEN, 2);
        if (KEYBOARD.wasKeyPressed(Key::F))
            DEBUG.drawCube({ 5,5,5 }, { 10,10,10 }, Color::VIOLET, 5, false);

        if (KEYBOARD.wasKeyPressed(Key::One))
            Locator::getSceneManager().LoadSceneAsync(new VertexGenScene);
        if (KEYBOARD.wasKeyPressed(Key::Two))
            Locator::getSceneManager().LoadSceneAsync(new SceneCameras);
        if (KEYBOARD.wasKeyPressed(Key::Three))
            Locator::getSceneManager().LoadSceneAsync(new MaterialTestScene);
        if (KEYBOARD.wasKeyPressed(Key::Four))
            Locator::getSceneManager().LoadSceneAsync(new ManyObjectsScene(10000));
        if (KEYBOARD.wasKeyPressed(Key::Five))
            Locator::getSceneManager().LoadSceneAsync(new CubemapScene());
        if (KEYBOARD.wasKeyPressed(Key::Six))
            Locator::getSceneManager().LoadSceneAsync(new TexArrayScene());
        if (KEYBOARD.wasKeyPressed(Key::Zero))
            Locator::getSceneManager().LoadSceneAsync(new TestScene());


        if (KEYBOARD.wasKeyPressed(Key::F1))
            Locator::getRenderer().setGlobalMaterialActive("NONE");
        if (KEYBOARD.wasKeyPressed(Key::F2))
            Locator::getRenderer().setGlobalMaterialActive("Wireframe");

        if (KEYBOARD.wasKeyPressed(Key::M))
            _ChangeMultiSampling();

        if(KEYBOARD.isKeyDown(Key::Escape))
            terminate();
    }

    //----------------------------------------------------------------------
    void _ChangeMultiSampling()
    {
        static int index = 0;
        int mscounts[]{ 1,4,8 };
        int newmscount = mscounts[index];
        index = (index + 1) % (sizeof(mscounts) / sizeof(int));
        Locator::getRenderer().setMultiSampleCount(newmscount);
        LOG("New Multisample-Count: " + TS(newmscount), Color::GREEN);
    }

    //----------------------------------------------------------------------
    void shutdown() override 
    {
        LOG( "Shutdown game..." );
    }
};


int main()
{
    Game game;
    game.start( "Awesome Game!", 800, 600 );

    system("pause");
    return 0;
}



