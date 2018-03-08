#include "OS/PlatformTimer/platform_timer.h"
#include "Memory/Allocators/include.hpp"
#include "Core/subsystem_manager.h"

#include "OS/FileSystem/file_system.h"
#include "OS/Threading/thread_pool.h"
#include "OS/FileSystem/file.h"
#include "i_game.hpp"
#include "Time/clock.h"

#include "Graphics/model.h"
#include "Graphics/vertex_layout.hpp"

#include "locator.h"
#include "GameplayLayer/Components/fps_camera.h"

using namespace Core;
using namespace DirectX;

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

class MyScene2 : public IScene
{
    GameObject* go;
    GameObject* go2;

    Components::Camera* cam;

public:
    MyScene2() : IScene("MyScene2") {}

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

        go2 = createGameObject("Box");
        go2->addComponent<Components::ModelRenderer>();

        //auto& viewport = cam->getViewport();
        //viewport.width  = 0.5f;
        //viewport.height = 0.5f;

        {
            //go2 = createGameObject("Camera2");
            //auto cam2 = go2->addComponent<Components::Camera>();
            //go2->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -5);
            //cam2->setClearMode(Components::Camera::EClearMode::NONE);

            //auto& viewport2 = cam2->getViewport();
            //viewport2.topLeftX = 0.5f;
            //viewport2.topLeftY = 0;
            //viewport2.width = 0.5f;
            //viewport2.height = 0.5f;
        }

        GameObject* go2 = findGameObject("Test");
        if (go2 != nullptr)
            LOG("Found GameObject!", Color::GREEN);

        //bool removed = go2->removeComponent( c );
        //bool destroyed = go2->removeComponent<Transform>();

        LOG("MyScene2 initialized!", Color::RED);
    }

    void shutdown() override
    {
        LOG("MyScene2 Shutdown!", Color::RED);
    }
};

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT3 color;
};

Vertex vertices[] =
{
    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
    { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
    { XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
    { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
    { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
    { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
    { XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
};

U32 indices[36] = {
    0, 1, 2, 0, 2, 3,
    4, 6, 5, 4, 7, 6,
    4, 5, 1, 4, 1, 0,
    3, 2, 6, 3, 6, 7,
    1, 5, 6, 1, 6, 2,
    4, 0, 3, 4, 3, 7
};

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

class MyScene : public IScene
{
    GameObject* go;
    Components::Camera* cam;

    Graphics::Model* m;

public:
    MyScene() : IScene("MyScene"){}

    void init() override
    {
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0,0,-10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f);

        GameObject* goModel = createGameObject("Test");
        auto mr = goModel->addComponent<Components::ModelRenderer>();
        goModel->addComponent<ConstantRotation>(0.0f, 20.0f, 20.0f);

        {
            GameObject* goModel2 = createGameObject("Test");
            goModel2->addComponent<Components::ModelRenderer>();
            goModel2->getComponent<Components::Transform>()->position = {5,0,0};
            goModel2->addComponent<ConstantRotation>(20.0f, 20.0f, 0.0f);

            GameObject* goModel3 = createGameObject("Test");
            goModel3->addComponent<Components::ModelRenderer>();
            goModel3->getComponent<Components::Transform>()->position = { -5,0,0 };
            goModel3->addComponent<ConstantRotation>(0.0f, 20.0f, 0.0f);
        }

        // Graphics::VertexLayout layout;
        m = new Graphics::Model(vertices, indices);

        // Create 3D-Model or load it... How to manage resources?
        // ModelPtr m = Assets::loadMesh("/models/test.obj");
        // ModelPtr m = ModelGenerator.createCube(...);

        // mr->setModel(m);

        LOG("MyScene initialized!", Color::RED);
    }

    void tick( Time::Seconds d ) override
    {
        static F32 speed = 50.0f;
        F32 delta = (F32)d.value;
        if (KEYBOARD.isKeyDown(Key::Add))
        {
            //cam->setFOV(cam->getFOV() + speed * (F32)delta.value);
            go->getComponent<Components::Transform>()->scale.y += speed * delta;
        }
        if (KEYBOARD.isKeyDown(Key::Subtract))
        {
            //cam->setFOV(cam->getFOV() - speed * delta);
            go->getComponent<Components::Transform>()->scale.y -= speed * delta;
        }
    }

    void shutdown() override
    {
        delete m;
        LOG("MyScene Shutdown!", Color::RED);
    }

};


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

        Locator::getSceneManager().LoadSceneAsync(new MyScene);
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        getWindow().setTitle( PROFILER.getUpdateDelta().toString().c_str() );
        static U64 ticks = 0;
        ticks++;

        clock.tick( delta );
        //LOG( TS( clock.getTime().value ) );
        auto time = clock.getTime();

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
            enabled = !enabled;
            g->setActive(enabled);
            LOG(TS(enabled), Color::BLUE);
        }

        if (KEYBOARD.wasKeyPressed(Key::R))
            auto go = SCENE.createGameObject("LOL");

        if (KEYBOARD.wasKeyPressed(Key::One))
            Locator::getSceneManager().LoadSceneAsync(new MyScene);
        if (KEYBOARD.wasKeyPressed(Key::Two))
            Locator::getSceneManager().LoadSceneAsync(new MyScene2);

        if (KEYBOARD.wasKeyPressed(Key::M))
            _ChangeMultiSampling();

        if( KEYBOARD.isKeyDown( Key::Escape ) )
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



