#pragma once
#include <DX.h>
#include "components.hpp"

class SceneCameras : public IScene
{
    GameObject* go;
    GameObject* go2;

    Components::Camera* cam;

public:
    SceneCameras() : IScene("SceneCameras") {}

    void init() override
    {
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        cam->setCameraMode(Graphics::CameraMode::Perspective);
        F32 size = 5.0f;
        cam->setOrthoParams(-size, size, -size, size, 0.1f, 100.0f);

        auto sphere = Core::MeshGenerator::CreateUVSphere(30,30);
        ArrayList<Color> sphereColors;
        for (U32 i = 0; i < sphere->getVertexCount(); i++)
            sphereColors.push_back(Math::Random::Color());
        sphere->setColors(sphereColors);

        go2 = createGameObject("Mesh");
        go2->addComponent<Components::MeshRenderer>(sphere, ASSETS.getColorMaterial());
        go2->addComponent<ConstantRotation>(0.0f, 20.0f, 0.0f);

        auto& viewport = cam->getViewport();
        viewport.width  = 0.5f;
        viewport.height = 0.5f;

        {
            // CAMERA 2
            auto go3 = createGameObject("Camera2");
            auto cam2 = go3->addComponent<Components::Camera>();
            go3->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -5);
            //cam2->setClearMode(Graphics::CameraClearMode::None);

            auto& viewport2 = cam2->getViewport();
            viewport2.topLeftX = 0.5f;
            viewport2.width = 0.5f;
            viewport2.height = 0.5f;

            // CAMERA 3
            auto go4 = createGameObject("Camera3");
            auto cam3 = go4->addComponent<Components::Camera>();
            go4->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, 5);
            go4->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
            //cam3->setClearMode(Graphics::CameraClearMode::None);

            auto& viewport3 = cam3->getViewport();
            viewport3.topLeftY = 0.5f;
            viewport3.width = 0.5f;
            viewport3.height = 0.5f;

            // CAMERA 4
            auto go5 = createGameObject("Camera4");
            auto cam4 = go5->addComponent<Components::Camera>();
            go5->getComponent<Components::Transform>()->position = Math::Vec3(0, -5, -5);
            go5->getComponent<Components::Transform>()->lookAt(Math::Vec3(0));
            //cam4->setClearMode(Graphics::CameraClearMode::None);

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
        //go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        go->addComponent<AutoOrbiting>(15.0f);

        auto wavesGO = createGameObject("Waves");
        wavesGO->addComponent<VertexGeneration>();

        auto cube = Core::MeshGenerator::CreateCube(1.0f);
        cube->setColors(cubeColors);

        auto plane = Core::MeshGenerator::CreatePlane();
        plane->setColors(planeColors);

        auto sphere = Core::MeshGenerator::CreateUVSphere(10,10);
        ArrayList<Color> sphereColors;
        for(U32 i = 0; i < sphere->getVertexCount(); i++)
            sphereColors.push_back(Math::Random::Color());
        sphere->setColors(sphereColors);

        {
            goModel = createGameObject("Test");
            goModel->addComponent<ConstantRotation>(0.0f, 20.0f, 20.0f);
            auto mr = goModel->addComponent<Components::MeshRenderer>(cube, ASSETS.getColorMaterial());

            GameObject* goModel2 = createGameObject("Test");
            goModel2->getComponent<Components::Transform>()->position = {5,0,0};
            goModel2->addComponent<ConstantRotation>(20.0f, 20.0f, 0.0f);
            mr = goModel2->addComponent<Components::MeshRenderer>(sphere, ASSETS.getColorMaterial());

            GameObject* goModel3 = createGameObject("Test");
            goModel3->getComponent<Components::Transform>()->position = { -5,0,0 };
            goModel3->addComponent<ConstantRotation>(0.0f, 0.0f, 20.0f);
            mr = goModel3->addComponent<Components::MeshRenderer>(plane, ASSETS.getColorMaterial());
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
    ManyObjectsScene(U32 numObjects = 10000) : IScene("MaterialTestScene"), m_numObjects(numObjects) {}

    void init() override
    {
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -500);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        // MESH
        auto cube = Core::MeshGenerator::CreateCube(1.0f);
        cube->setColors(cubeColors);

        U32 sq = (U32)sqrt(m_numObjects);

        for (U32 i = 0; i < m_numObjects; i++)
        {
            auto go = createGameObject("Test");
            go->addComponent<Components::MeshRenderer>(cube, ASSETS.getColorMaterial());
            go->getComponent<Components::Transform>()->position = Math::Random::Vec3(-1,1).normalized() * sqrtf((F32)m_numObjects);
        }

        LOG("ManyObjectsScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("ManyObjectsScene Shutdown!", Color::RED); }
};

class SceneMirror : public IScene
{
public:
    SceneMirror() : IScene("SceneMirror") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        go->addComponent<Components::Skybox>(cubemap);

        // Camera 2
        auto renderTex = RESOURCES.createRenderTexture(1024, 720, Graphics::DepthFormat::D32, Graphics::TextureFormat::BGRA32, 2, Graphics::MSAASamples::Four);
        auto cam2GO = createGameObject("Camera2");
        cam2GO->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, -10);
        cam2GO->getTransform()->lookAt({});
        //cam2GO->addComponent<AutoOrbiting>(10.0f);

        auto cam2 = cam2GO->addComponent<Components::Camera>();
        cam2->setRenderTarget(renderTex);
        cam2->setClearColor(Color::GREEN);
        cam2GO->addComponent<PostProcess>(ASSETS.getMaterial("/materials/post processing/color_grading.material"));

        auto grid = createGameObject("Grid");
        grid->addComponent<GridGeneration>(20);

        // MESH
        auto cube = Core::MeshGenerator::CreateCubeUV();
        cube->setColors(cubeColors);

        // MATERIAL
        auto customTexMaterial = RESOURCES.createMaterial(ASSETS.getShader("/shaders/tex.shader"));
        customTexMaterial->setTexture("tex", renderTex);
        customTexMaterial->setColor("tintColor", Color::WHITE);

        // GAMEOBJECT
        auto go3 = createGameObject("Test3");
        go3->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), customTexMaterial);
        go3->getTransform()->position = Math::Vec3(0, 1.5f, 0);
        go3->getTransform()->scale = { 3 };

        auto player = createGameObject("Player");
        player->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/normals.material"));
        player->getTransform()->setParent(go->getTransform(), false);
        player->getTransform()->position = { 0, 0, -0.5f };
        player->getTransform()->rotation *= Math::Quat(Math::Vec3::UP, 180.0f);

        LOG("SceneMirror initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.wasKeyPressed(Key::NumPad1))
            RESOURCES.setGlobalAnisotropicFiltering(1);
        if (KEYBOARD.wasKeyPressed(Key::NumPad2))
            RESOURCES.setGlobalAnisotropicFiltering(4);
        if (KEYBOARD.wasKeyPressed(Key::NumPad3))
            RESOURCES.setGlobalAnisotropicFiltering(8);
    }

    void shutdown() override
    {
        LOG("SceneMirror Shutdown!", Color::RED);
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
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        //go->addComponent<AutoOrbiting>(10.0f);
        //go->addComponent<Components::Skybox>(cubemap);

        auto grid = createGameObject("Grid");
        grid->addComponent<GridGeneration>(20);

        // MESH
        auto sphere = Core::MeshGenerator::CreateCubeUV(1000.0f);

        // SHADER
        auto texShader = ASSETS.getShader( "/engine/shaders/skybox.shader" );

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture(SID("Cubemap"), ASSETS.getDefaultCubemap());

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

public:
    TexArrayScene() : IScene("TexArrayScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        //go->addComponent<Components::Skybox>(cubemap);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // MESH
        auto plane = Core::MeshGenerator::CreatePlane();

        // SHADER
        auto texShader = ASSETS.getShader( "/shaders/textureArray.shader" );

        // TEXTURES
        Color cols[] = { Color::RED, Color::BLUE, Color::GREEN };

        const I32 slices = 3;
        const I32 size = 512;

        auto texArr = RESOURCES.createTexture2DArray(size, size, slices, Graphics::TextureFormat::RGBA32, false);
        for (I32 slice = 0; slice < slices; slice++)
            texArr->setPixels(slice, ArrayList<Color>(size * size, cols[slice]).data());
        texArr->apply();
        texArr->setFilter(Graphics::TextureFilter::Point);

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture("texArray", texArr);
        material->setInt("texIndex", 0);

        auto material2 = RESOURCES.createMaterial();
        material2->setShader(texShader);
        material2->setTexture("texArray", texArr);
        material2->setInt("texIndex", 1);

        auto material3 = RESOURCES.createMaterial();
        material3->setShader(texShader);
        material3->setTexture("texArray", texArr);
        material3->setInt("texIndex", 2);

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

class SceneGraphScene : public IScene
{
    GameObject* parent;
    GameObject* child;
    GameObject* child2;

public:
    SceneGraphScene() : IScene("SceneGraphScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // SHADER
        auto texShader = ASSETS.getShader("/shaders/tex.shader");

        // MATERIAL
        auto material = RESOURCES.createMaterial();
        material->setShader(texShader);
        material->setTexture("tex", ASSETS.getTexture2D("/textures/dirt.jpg"));
        material->setColor("tintColor", Color::WHITE);

        auto material2 = RESOURCES.createMaterial();
        material2->setShader(texShader);
        material2->setTexture("tex", ASSETS.getTexture2D("/textures/nico.jpg"));
        material2->setColor("tintColor", Color::WHITE);

        // MESH
        auto mesh = Core::MeshGenerator::CreateCubeUV();
        mesh->setColors(cubeColors);

        // GAMEOBJECT
        parent = createGameObject("Test2");
        parent->addComponent<Components::MeshRenderer>(mesh, material);
        parent->addComponent<ConstantRotation>(5.0f, 25.0f, 0.0f);

        child = createGameObject("Test3");
        child->addComponent<Components::MeshRenderer>(mesh, material2);
        child->getTransform()->position.x = 5;
        child->getTransform()->position.y = 1;

        child2 = createGameObject("Test4");
        child2->addComponent<Components::MeshRenderer>(mesh, material2);
        child2->getTransform()->position.y = 3;
        child2->getTransform()->position.z = 2;

        parent->getTransform()->addChild(child->getTransform());
        parent->getTransform()->addChild(child2->getTransform());

        LOG("SceneGraphScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.wasKeyPressed(Key::F))
            child2->getTransform()->setParent(nullptr);

        if (KEYBOARD.wasKeyPressed(Key::G))
            child2->getTransform()->setParent(parent->getTransform());
    }

    void shutdown() override { LOG("SceneGraphScene Shutdown!", Color::RED); }
};


class SceneFrustumVisualization : public IScene
{
public:
    SceneFrustumVisualization() : IScene("SceneFrustumVisualization") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 10.0f, 0.3f, 1.0f);
        go->addComponent<DrawFrustum>();

        auto go3 = createGameObject("Camera2");
        auto cam2 = go3->addComponent<Components::Camera>(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
        go3->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, -10);
        go3->addComponent<AutoOrbiting>(10.0f);
        cam2->getViewport().width = 0.25f;
        cam2->getViewport().height = 0.25f;
        go3->addComponent<DrawFrustum>();

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto mesh = Core::MeshGenerator::CreateCube(1);
        mesh->setColors(cubeColors);

        auto go2 = createGameObject("Obj");
        go2->addComponent<Components::MeshRenderer>(mesh, ASSETS.getColorMaterial());

        LOG("SceneFrustumVisualization initialized!", Color::RED);
    }

    void shutdown() override { LOG("SceneFrustumVisualization Shutdown!", Color::RED); }
};


class TransparencyScene : public IScene
{
public:
    TransparencyScene() : IScene("TransparencyScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto plane = Core::MeshGenerator::CreatePlane();
        auto mat = ASSETS.getMaterial("/materials/transparent.material");

        auto go2 = createGameObject("Obj");
        go2->addComponent<Components::MeshRenderer>(plane, mat);

        auto go3 = createGameObject("Obj");
        go3->addComponent<Components::MeshRenderer>(plane, mat);
        go3->getTransform()->position.z = 3;

        auto go4 = createGameObject("Obj");
        go4->addComponent<Components::MeshRenderer>(plane, mat);
        go4->getTransform()->position.z = -3;

        LOG("TransparencyScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("TransparencyScene Shutdown!", Color::RED); }
};

class BlinnPhongLightingScene : public IScene
{
    Components::SpotLight* spot;

public:
    BlinnPhongLightingScene() : IScene("BlinnPhongLightingScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        spot = go->addComponent<Components::SpotLight>(2.0f, Color::RED, 25.0f, 20.0f, false);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto mesh = Core::MeshGenerator::CreatePlane(1.0f);

        auto mat = ASSETS.getMaterial("/materials/phong.material");
        auto go2 = createGameObject("Obj");
        go2->addComponent<Components::MeshRenderer>(mesh, mat);
        go2->addComponent<VisualizeNormals>(0.1f, Color::WHITE);
        go2->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90);
        go2->getTransform()->scale = { 10,10,10 };

        I32 loop = 2;
        F32 distance = 3.0f;
        for (I32 x = -loop; x <= loop; x++)
        {
            for (I32 y = -loop; y <= loop; y++)
            {
                for (I32 z = -loop; z <= loop; z++)
                {
                    auto gameobject = createGameObject("Obj");
                    gameobject->addComponent<Components::MeshRenderer>(mesh, mat);
                    gameobject->getTransform()->position = Math::Vec3(x * distance, y * distance + 0.01f, z * distance);
                    gameobject->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90);
                }
            }
        }

        I32 loop2 = 1;
        F32 distance2 = 3.0f;
        for (I32 x = -loop2; x <= loop2; x++)
        {
            for (I32 z = -loop2; z <= loop2; z++)
            {
                auto gameobject = createGameObject("Obj");
                gameobject->addComponent<Components::PointLight>(2.0f, Math::Random::Color());
                gameobject->getTransform()->position = Math::Vec3(x * distance2, 1.0f, z * distance2);
                gameobject->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
            }
        }

        //auto sun = createGameObject("Sun");
        //sun->addComponent<Components::DirectionalLight>(1.0f, Color::WHITE);
        //sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        auto pl = createGameObject("PointLight");
        pl->addComponent<Components::PointLight>(2.0f, Color::GREEN);
        pl->getTransform()->position = { 3, 1, 0 };
        pl->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        pl->addComponent<AutoOrbiting>(20.0f);

        LOG("BlinnPhongLightingScene initialized!", Color::RED);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.isKeyDown(Key::Up))
        {
            spot->setAngle(spot->getAngle() + 10.0f * (F32)d);
            LOG(TS(spot->getAngle()));
        }
        if (KEYBOARD.isKeyDown(Key::Down))
        {
            spot->setAngle(spot->getAngle() - 10.0f * (F32)d);
            LOG(TS(spot->getAngle()));
        }

        if (KEYBOARD.isKeyDown(Key::Left))
        {
            spot->setRange(spot->getRange() - 10.0f * (F32)d);
            LOG(TS(spot->getRange()));
        }
        if (KEYBOARD.isKeyDown(Key::Right))
        {
            spot->setRange(spot->getRange() + 10.0f * (F32)d);
            LOG(TS(spot->getRange()));
        }

        if (KEYBOARD.wasKeyPressed(Key::F))
            spot->setActive(!spot->isActive());
    }

    void shutdown() override { LOG("BlinnPhongLightingScene Shutdown!", Color::RED); }
};


class BRDFLUTScene : public IScene
{
public:
    BRDFLUTScene() : IScene("BRDFLUTScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        cam->setClearColor(Color::BLUE);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);

        Assets::BRDFLut brdfLut;
        auto planeMat = ASSETS.getMaterial("/materials/texture.material");
        planeMat->setTexture("tex", brdfLut.getTexture());
        auto plane = createGameObject("Plane");
        plane->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(1), planeMat);

        LOG("BRDFLUTScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("BRDFLUTScene Shutdown!", Color::RED); }
};


class ScenePBRSpheres : public IScene
{
public:
    ScenePBRSpheres() : IScene("PBRSpheres") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Four, true);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        go->addComponent<Tonemap>();

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/pine.hdr", 2048, true);

        auto pbrShader = ASSETS.getShader("/engine/shaders/pbr/pbr.shader");

        Assets::EnvironmentMap envMap(cubemapHDR, 128, 512);
        auto diffuse = envMap.getDiffuseIrradianceMap();
        auto specular = envMap.getSpecularReflectionMap();

        auto brdfLut = Assets::BRDFLut().getTexture();
        pbrShader->setReloadCallback([=](Graphics::IShader* shader) {
            shader->setTexture("diffuseIrradianceMap", diffuse);
            shader->setTexture("specularReflectionMap", specular);
            shader->setTexture("brdfLUT", brdfLut);
            shader->setFloat("maxReflectionLOD", F32(specular->getMipCount() - 1));
        });
        pbrShader->invokeReloadCallback();

        auto mesh = ASSETS.getMesh("/models/sphere.obj");
        auto mat = ASSETS.getMaterial("/materials/pbr/test.pbrmaterial");

        auto go2 = createGameObject("Obj");
        go2->addComponent<Components::MeshRenderer>(mesh, mat);
        go2->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90);
        go2->getTransform()->scale = { 1.0f };
        go2->getTransform()->position = { 0, 0, -3 };
        go2->addComponent<Components::Skybox>(cubemapHDR);

        I32 num = 7;
        F32 distance = 3.0f;
        for (I32 x = 0; x < num; x++)
        {
            F32 roughness = x / (F32)(num - 1);

            for (I32 y = 0; y < 2; y++)
            {
                auto gameobject = createGameObject("Obj");

                auto material = RESOURCES.createMaterial(pbrShader);
                material->setTexture("albedoMap", ASSETS.getWhiteTexture());
                material->setTexture("normalMap", ASSETS.getNormalTexture());
                material->setColor("color", Color::WHITE);
                material->setFloat("roughness", roughness);
                F32 metallic = (F32)y;
                material->setFloat("metallic", metallic);
                material->setFloat("useRoughnessMap", 0.0f);
                material->setFloat("useMetallicMap", 0.0f);

                gameobject->addComponent<Components::MeshRenderer>(mesh, material);
                gameobject->getTransform()->position = Math::Vec3(x * distance - (num / 2 * distance), y * distance + 0.01f, 0.0f);
            }
        }

        //auto sun = createGameObject("Sun");
        //sun->addComponent<Components::DirectionalLight>(1.0f, Color::WHITE);
        //sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        auto pl = createGameObject("PointLight");
        pl->addComponent<Components::PointLight>(3.0f, Color::WHITE);
        pl->getTransform()->position = { 5, 2, 0 };
        pl->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        pl->addComponent<AutoOrbiting>(20.0f);

        F32 intensity = 5.0f;
        F32 range = 30.0f;
        //auto pl2 = createGameObject("PointLight");
        //pl2->addComponent<Components::PointLight>(intensity, Math::Random::Color(), range);
        //pl2->getTransform()->position = { -5, 3, -3 };
        //pl2->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        //auto pl3 = createGameObject("PointLight");
        //pl3->addComponent<Components::PointLight>(intensity, Math::Random::Color(), range);
        //pl3->getTransform()->position = { 5, 3, -3 };
        //pl3->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        //auto pl4 = createGameObject("PointLight");
        //pl4->addComponent<Components::PointLight>(intensity, Math::Random::Color(), range);
        //pl4->getTransform()->position = { -5, -3, -3 };
        //pl4->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        //auto pl5 = createGameObject("PointLight");
        //pl5->addComponent<Components::PointLight>(intensity, Math::Random::Color(), range);
        //pl5->getTransform()->position = { 5, -3, -3 };
        //pl5->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.3f);

        LOG("PBRSpheres initialized!", Color::RED);
    }

    void shutdown() override { LOG("PBRSpheres Shutdown!", Color::RED); }
};

class ScenePBRPistol : public IScene
{
public:
    ScenePBRPistol() : IScene("ScenePBRPistol") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Four, true);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        go->addComponent<Tonemap>();

        // Environment map
        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/canyon.hdr", 2048, true);
        auto pbrShader = ASSETS.getShader("/engine/shaders/pbr/pbr.shader");

        Assets::EnvironmentMap envMap(cubemapHDR, 128, 512);
        auto diffuse = envMap.getDiffuseIrradianceMap();
        auto specular = envMap.getSpecularReflectionMap();

        auto brdfLut = Assets::BRDFLut().getTexture();
        pbrShader->setReloadCallback([=](Graphics::IShader* shader) {
            shader->setTexture("diffuseIrradianceMap", diffuse);
            shader->setTexture("specularReflectionMap", specular);
            shader->setTexture("brdfLUT", brdfLut);
            shader->setFloat("maxReflectionLOD", F32(specular->getMipCount() - 1));
        });
        pbrShader->invokeReloadCallback();

        // Skybox
        createGameObject("Skybox")->addComponent<Components::Skybox>(cubemapHDR);

        // Gameobjects
        auto pistolMesh = ASSETS.getMesh("/models/pistol.fbx");
        auto pistol = createGameObject("Pistol");
        pistol->addComponent<Components::MeshRenderer>(pistolMesh, ASSETS.getMaterial("/materials/pbr/pistol.pbrmaterial"));
        pistol->getTransform()->scale = { 0.1f };
        pistol->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, -90.0f);
        pistol->getTransform()->rotation *= Math::Quat(Math::Vec3::UP, -90.0f);
        pistol->getTransform()->position = { 5, 0, 0 };

        auto daggerMesh = ASSETS.getMesh("/models/dagger.obj");
        auto dagger = createGameObject("Dagger");
        dagger->addComponent<Components::MeshRenderer>(daggerMesh, ASSETS.getMaterial("/materials/pbr/dagger.pbrmaterial"));
        dagger->getTransform()->scale = { 0.1f };
        dagger->getTransform()->rotation *= Math::Quat(Math::Vec3::FORWARD, -90.0f);
        dagger->getTransform()->position = { 0, 4, 0 };

        // LIGHTS
        auto sun = createGameObject("Sun");
        sun->addComponent<Components::DirectionalLight>(5.0f, Color::WHITE);
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        LOG("ScenePBRPistol initialized!", Color::RED);
    }

    void shutdown() override { LOG("ScenePBRPistol Shutdown!", Color::RED); }
};

ArrayList<MaterialPtr> GeneratePBRMaterials(const ShaderPtr& pbrShader, const MeshPtr& mesh, const Assets::MeshMaterialInfo& materials)
{
    ASSERT(materials.isValid());
    static const F32 DEFAULT_ROUGHNESS = 0.4f;

    ArrayList<MaterialPtr> pbrMaterials;
    for ( I32 i = 0; i < mesh->getSubMeshCount(); i++ )
    {
        auto pbrMat = RESOURCES.createMaterial( pbrShader );
        pbrMat->setColor( "color", Color::WHITE );
        pbrMat->setTexture( "normalMap", ASSETS.getNormalTexture() );
        pbrMat->setFloat( "useRoughnessMap", 0.0f );
        pbrMat->setFloat( "useMetallicMap", 0.0f );
        pbrMat->setTexture( "roughnessMap", ASSETS.getBlackTexture() );
        pbrMat->setTexture( "metallicMap", ASSETS.getBlackTexture() );
        pbrMat->setFloat( "metallic", 0.0f );
        pbrMat->setFloat( "roughness", DEFAULT_ROUGHNESS );

        auto material = materials[i];
        pbrMat->setColor("color", material.diffuseColor);

        for (auto& texture : material.textures)
        {
            switch (texture.type)
            {
            case Assets::MaterialTextureType::Albedo: pbrMat->setTexture("albedoMap", ASSETS.getTexture2D(texture.filePath)); break;
            case Assets::MaterialTextureType::Normal: pbrMat->setTexture("normalMap", ASSETS.getTexture2D(texture.filePath)); break;
            case Assets::MaterialTextureType::Shininess:
            {
                pbrMat->setFloat("useRoughnessMap", 1.0f);
                pbrMat->setTexture("roughnessMap", ASSETS.getTexture2D(texture.filePath)); 
                break;
            }
            case Assets::MaterialTextureType::Specular:
            {
                pbrMat->setFloat("useMetallicMap", 1.0f);
                pbrMat->setTexture("metallicMap", ASSETS.getTexture2D(texture.filePath)); 
                break;
            }
            }
        }
        pbrMaterials.push_back( pbrMat );
    }
    return pbrMaterials;
}

class SponzaScene : public IScene
{
public:
    SponzaScene() : IScene("SponzaScene") {}

    void init() override
    {
        // Camera
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Four, true);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        go->addComponent<Tonemap>();

        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/canyon.hdr", 2048, true);
        auto pbrShader = ASSETS.getShader("/engine/shaders/pbr/pbr.shader");

        Assets::EnvironmentMap envMap(cubemapHDR, 128, 512);
        auto diffuse = envMap.getDiffuseIrradianceMap();
        auto specular = envMap.getSpecularReflectionMap();

        auto brdfLut = Assets::BRDFLut().getTexture();
        pbrShader->setReloadCallback([=](Graphics::IShader* shader) {
            shader->setTexture("diffuseIrradianceMap", diffuse);
            shader->setTexture("specularReflectionMap", specular);
            shader->setTexture("brdfLUT", brdfLut);
            shader->setFloat("maxReflectionLOD", F32(specular->getMipCount() - 1));
        });
        pbrShader->invokeReloadCallback();

        createGameObject("Skybox")->addComponent<Components::Skybox>(cubemapHDR);

        Assets::MeshMaterialInfo materialImportInfo;
        auto mesh = ASSETS.getMesh( "/models/sponza/sponza.obj", &materialImportInfo );

        auto obj = createGameObject("Obj");
        auto mr = obj->addComponent<Components::MeshRenderer>(mesh, nullptr);
        //obj->addComponent<VisualizeNormals>(1.0f, Color::BLUE);
        //obj->addComponent<VisualizeTangents>(1.0f, Color::RED);
        obj->getTransform()->scale = { 0.05f };

        if ( materialImportInfo.isValid() )
        {
            auto pbrMaterials = GeneratePBRMaterials( pbrShader, mesh, materialImportInfo );
            for ( I32 i = 0; i < pbrMaterials.size(); i++ )
                mr->setMaterial( pbrMaterials[i], i );
        }

        // LIGHTS
        //auto sun = createGameObject("Sun");
        //sun->addComponent<Components::DirectionalLight>(5.0f, Color::WHITE);
        //sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        auto pl = createGameObject("PointLight");
        pl->addComponent<Components::PointLight>(15.0f, Color::WHITE, 30.0f);
        pl->getTransform()->position = { 5, 2, 0 };
        pl->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        pl->addComponent<AutoOrbiting>(20.0f);

        LOG("SponzaScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("SponzaScene Shutdown!", Color::RED); }
};


class ScenePostProcessMultiCamera : public IScene
{
    Components::Camera* cam;
public:
    ScenePostProcessMultiCamera() : IScene("ScenePostProcessMultiCamera") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::Four, true);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA);
        cam->getViewport().width = 0.5f;
        cam->getViewport().height = 0.5f;
        go->addComponent<Tonemap>();

        // Camera 2
        auto go2 = createGameObject("Camera2");
        auto cam2 = go2->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);
        cam2->getViewport().width = 0.5f;
        cam2->getViewport().height = 0.5f;
        cam2->getViewport().topLeftX = 0.5f;
        go2->addComponent<Tonemap>();
        go2->addComponent<Fog>();
        go2->getTransform()->setParent(go->getTransform(), false);

        // Camera 3
        auto go3 = createGameObject("Camera3");
        auto cam3 = go3->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);
        cam3->getViewport().width = 0.5f;
        cam3->getViewport().height = 0.5f;
        cam3->getViewport().topLeftY = 0.5f;
        go3->addComponent<Tonemap>();
        go3->addComponent<PostProcess>(ASSETS.getMaterial("/materials/post processing/color_grading.material"));
        go3->getTransform()->setParent(go->getTransform(), false);

        // Camera 4
        auto go4 = createGameObject("Camera4");
        auto cam4 = go4->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);
        cam4->getViewport().width = 0.5f;
        cam4->getViewport().height = 0.5f;
        cam4->getViewport().topLeftY = 0.5f;
        cam4->getViewport().topLeftX = 0.5f;
        go4->addComponent<Tonemap>();
        go4->addComponent<GaussianBlur>();
        go4->getTransform()->setParent(go->getTransform(), false);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        // PBR
        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/pine.hdr", 2048, true);
        Assets::EnvironmentMap envMap(cubemapHDR, 128, 512);
        auto diffuse = envMap.getDiffuseIrradianceMap();
        auto specular = envMap.getSpecularReflectionMap();

        auto brdfLut = Assets::BRDFLut().getTexture();
        auto pbrShader = ASSETS.getShader("/engine/shaders/pbr/pbr.shader");
        pbrShader->setReloadCallback([=](Graphics::IShader* shader) {
            shader->setTexture("diffuseIrradianceMap", diffuse);
            shader->setTexture("specularReflectionMap", specular);
            shader->setTexture("brdfLUT", brdfLut);
            shader->setFloat("maxReflectionLOD", F32(specular->getMipCount() - 1));
        });
        pbrShader->invokeReloadCallback();

        auto obj = createGameObject("Obj");
        obj->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/pbr/gold.pbrmaterial"));
        obj->addComponent<Components::Skybox>(cubemapHDR);

        LOG("ScenePostProcessMultiCamera initialized!", Color::RED);
    }

    void shutdown() override { LOG("ScenePostProcessMultiCamera Shutdown!", Color::RED); }
};



class SceneGUI : public IScene
{
public:
    SceneGUI() : IScene("SceneGUI") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 3, -8);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);

        createGameObject("Grid")->addComponent<GridGeneration>(20);

        go->addComponent<Components::GUI>();
        //go->addComponent<Components::GUIImage>(ASSETS.getTexture2D("/textures/nico.jpg"));
        go->addComponent<Components::GUIDemoWindow>();

        static F32 color[4] = {};
        go->addComponent<Components::GUICustom>([=] {
            ImGui::Begin("Color is cool!");
            if (ImGui::ColorPicker4("Clear color", color))
                cam->setClearColor(Color(color, true));
            ImGui::End();
        });

        //auto go2 = createGameObject("Camera2");
        //auto cam2 = go2->addComponent<Components::Camera>();
        //auto renderTex = RESOURCES.createRenderTexture(1024, 720, Graphics::DepthFormat::None, Graphics::TextureFormat::BGRA32, 2, Graphics::MSAASamples::One);
        //cam2->setRenderTarget(renderTex);
        //go2->addComponent<Components::GUI>();
        //go2->addComponent<Components::GUIDemoWindow>();

        //auto rtGO = createGameObject("GUIScreen");
        //auto guiScreenMat = RESOURCES.createMaterial(ASSETS.getShader("/shaders/tex.shader"));
        //guiScreenMat->setTexture("tex", cam2->getRenderTarget()->getColorBuffer());
        //guiScreenMat->setColor("tintColor", Color::WHITE);
        //rtGO->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), guiScreenMat);
        //rtGO->getTransform()->position = { 0, 1, 0 };

        LOG("SceneGUI initialized!", Color::RED);
    }

    void shutdown() override { LOG("SceneGUI Shutdown!", Color::RED); }
};

class ShadowScene : public IScene
{
public:
    ShadowScene() : IScene("ShadowScene") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 10, -25);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        cam->setClearColor(Color(66, 134, 244));
        //go->addComponent<AutoOrbiting>(15.0f);

        auto player = createGameObject("Player");
        player->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/normals.material"));
        player->getTransform()->setParent(go->getTransform(), false);
        player->getTransform()->position = { 0, 0, -0.5f };
        player->getTransform()->rotation *= Math::Quat(Math::Vec3::UP, 180.0f);

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

        auto cubeGO2 = createGameObject("GO3");
        cubeGO2->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreateCubeUV(0.3f), ASSETS.getMaterial("/materials/blinn_phong/cube.material"));
        cubeGO2->getTransform()->position = { -8.0f, 0.3001f, 0.0f };

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
                material->setReplacementShader(TAG_SHADOW_PASS, ASSETS.getShadowMapShaderAlpha());
                material->setReplacementShader(TAG_SHADOW_PASS_LINEAR, ASSETS.getShadowMapShaderLinearAlpha());
                mr->setMaterial(material, i);
            }
        }

        // LIGHTS
        auto sun = createGameObject("Sun");
        auto dl = sun->addComponent<Components::DirectionalLight>(0.3f, Color::WHITE, Graphics::ShadowType::CSMSoft, ArrayList<F32>{10.0f, 30.0f, 80.0f, 200.0f});
        //auto dl = sun->addComponent<Components::DirectionalLight>(0.0f, Color::WHITE);
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });
        //sun->addComponent<ConstantRotation>(5.0f, 0.0f, 0.0f);

        //auto sun2 = createGameObject("Sun2");
        //sun2->addComponent<Components::DirectionalLight>(0.3f, Color::WHITE);
        //sun2->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, -1 });

        auto plg = createGameObject("PL");
        auto pl = plg->addComponent<Components::PointLight>(1.0f, Color::ORANGE, 5.0f, true);
        plg->getTransform()->position = { 3, 2, 0 };
        plg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        go->addComponent<Components::Skybox>(pl->getShadowMap());

        auto slg = createGameObject("PL");
        auto sl = slg->addComponent<Components::SpotLight>(1.0f, Color::WHITE, 25.0f, 20.0f);
        slg->getTransform()->position = { -5, 2, -2 };
        slg->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });
        slg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/spotLight.png"), 0.5f);
        //slg->addComponent<DrawFrustum>();

        go->addComponent<Components::GUI>();
        go->addComponent<Components::GUIFPS>();
        go->addComponent<Components::GUICustom>([=] {
            static F32 ambient = 0.4f;
            ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
            Locator::getRenderer().setGlobalFloat(SID("_Ambient"), ambient);

            obj2->getTransform()->position.x = 5.0f + std::sinf((F32)TIME.getTime());

            if (ImGui::CollapsingHeader("Shadows"))
            {
                CString type[] = { "None", "Hard", "Soft" };
                static I32 type_current = 2;
                if (ImGui::Combo("Type", &type_current, type, 3))
                    CONFIG.setShadowType(Graphics::ShadowType(type_current));

                CString qualities[] = { "Low", "Medium", "High", "Insane" };
                static I32 quality_current = 2;
                if (ImGui::Combo("Quality", &quality_current, qualities, 4))
                    CONFIG.setShadowMapQuality(Graphics::ShadowMapQuality(quality_current));
            }

            if (ImGui::CollapsingHeader("Lights"))
            {
                if (ImGui::TreeNode("Directional Light"))
                {
                    static F32 animateSpeed = 0.0f;
                    ImGui::SliderFloat("Speed", &animateSpeed, -20.0f, 20.0f);
                    dl->getGameObject()->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, animateSpeed * (F32)PROFILER.getDelta());

                    static Math::Vec3 deg{ 45.0f, 0.0f, 0.0f };
                    if (ImGui::SliderFloat2("Rotation", &deg.x, 0.0f, 360.0f))
                        sun->getTransform()->rotation = Math::Quat::FromEulerAngles(deg);

                    static F32 color[4] = { 1,1,1,1 };
                    if (ImGui::ColorEdit4("Color", color))
                        dl->setColor(Color(color));

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

                    if (type_current > 0 && type_current <= 2)
                    {
                        static F32 dlRange;
                        dlRange = dl->getShadowRange();
                        if (ImGui::SliderFloat("Shadow Range", &dlRange, 5.0f, 100.0f))
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
                    static Math::Vec3 plPos{ 0, 2.5, -1 };
                    if (ImGui::SliderFloat3("Position", &plPos.x, -3.0f, 3.0f))
                        plg->getTransform()->position = plPos;

                    static F32 plRange;
                    plRange = pl->getRange();
                    if (ImGui::SliderFloat("Range", &plRange, 5.0f, 50.0f))
                        pl->setRange(plRange);

                    static F32 plIntensity = 1.0f;
                    plIntensity = pl->getIntensity();
                    if (ImGui::SliderFloat("Intensity", &plIntensity, 0.1f, 3.0f))
                        pl->setIntensity(plIntensity);

                    CString type[] = { "None", "Hard", "Soft" };
                    static I32 type_current = 1;
                    type_current = (I32)pl->getShadowType();
                    if (ImGui::Combo("Shadow Type", &type_current, type, 3))
                        pl->setShadowType((Graphics::ShadowType)(type_current));

                    CString qualities[] = { "Low", "Medium", "High", "Insane" };
                    static I32 quality_current = 2;
                    quality_current = (I32)pl->getShadowMapQuality();
                    if (ImGui::Combo("Quality", &quality_current, qualities, 4))
                        pl->setShadowMapQuality((Graphics::ShadowMapQuality)(quality_current));
                    static F32 color[4] = { 1,1,1,1 };
                    if (ImGui::ColorEdit4("Color", color))
                        pl->setColor(Color(color));
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Spot Light"))
                {
                    static F32 spotRange;
                    spotRange = sl->getRange();
                    if (ImGui::SliderFloat("Range", &spotRange, 5.0f, 50.0f))
                        sl->setRange(spotRange);

                    static F32 spotIntensity;
                    spotIntensity = sl->getIntensity();
                    if (ImGui::SliderFloat("Intensity", &spotIntensity, 0.1f, 3.0f))
                        sl->setIntensity(spotIntensity);

                    static F32 spotAngle;
                    spotAngle = sl->getAngle();
                    if (ImGui::SliderFloat("Angle", &spotAngle, 5.0f, 90.0f))
                        sl->setAngle(spotAngle);

                    CString type[] = { "None", "Hard", "Soft" };
                    static I32 type_current = 1;
                    type_current = (I32)sl->getShadowType();
                    if (ImGui::Combo("Shadow Type", &type_current, type, 3))
                        sl->setShadowType((Graphics::ShadowType)(type_current));

                    CString qualities[] = { "Low", "Medium", "High", "Insane" };
                    static I32 quality_current = 2;
                    quality_current = (I32)sl->getShadowMapQuality();
                    if (ImGui::Combo("Quality", &quality_current, qualities, 4))
                        sl->setShadowMapQuality((Graphics::ShadowMapQuality)(quality_current));
                    static F32 color[4] = { 1,1,1,1 };
                    if (ImGui::ColorEdit4("Color", color))
                        sl->setColor(Color(color));
                    ImGui::TreePop();
                }
            }
        });

        LOG("ShadowScene initialized!", Color::RED);
    }

    void shutdown() override { LOG("ShadowScene Shutdown!", Color::RED); }
};
