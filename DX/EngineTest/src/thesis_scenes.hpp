#pragma once
#include <DX.h>
#include "components.hpp"

class TSceneAlphaProblemDemo : public IScene
{
    Components::Camera* cam;
    GameObject* go;

    GameObject* test;

public:
    TSceneAlphaProblemDemo() : IScene("TSceneAlphaProblemDemo") {}

    void init() override
    {
        // Cameras
        go = createGameObject("Camera");
        cam = go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);
        cam->setClearColor(Color(175, 181, 191));
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -5);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        go->addComponent<Components::AudioListener>();

        cam->getViewport().width = 0.3f;

        auto cam2 = createGameObject("Camera2")->addComponent<Components::Camera>(45.0f, 0.1f, 6.0f, Graphics::MSAASamples::One);
        cam2->setClearColor(Color(175, 181, 191));
        cam2->getViewport().width = 0.3f;
        cam2->getViewport().topLeftX = 0.31f;
        cam2->getGameObject()->getTransform()->position = go->getTransform()->position;
        cam2->getGameObject()->addComponent<DrawFrustum>();

        auto cam3 = createGameObject("Camera2")->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);
        cam3->setClearColor(Color(175, 181, 191));
        cam3->getViewport().width = 0.3f;
        cam3->getViewport().topLeftX = 0.62f;
        cam3->getGameObject()->getTransform()->position = go->getTransform()->position + Math::Vec3{ 20.0f, 0, 0 };

        // MATERIALS
        auto texMat = RESOURCES.createMaterial(ASSETS.getShader("/shaders/tex.shader"));
        texMat->setColor("tintColor", Color::WHITE);
        texMat->setTexture("tex", ASSETS.getTexture2D("/textures/yvonne.png"));

        auto alphaMat = RESOURCES.createMaterial(ASSETS.getShader("/shaders/transparent.shader"));
        alphaMat->setFloat("opacity", 0.5f);
        alphaMat->setTexture("tex", ASSETS.getTexture2D("/engine/textures/checker.jpg"));

        auto alphaMatWrong = RESOURCES.createMaterial(ASSETS.getShader("/shaders/transparentWrong.shader"));
        alphaMatWrong->setFloat("opacity", 0.5f);
        alphaMatWrong->setTexture("tex", ASSETS.getTexture2D("/engine/textures/checker.jpg"));

        // OBJECTS
        auto cube = Core::MeshGenerator::CreateCubeUV(1.0f);
        createGameObject("Cube")->addComponent<Components::MeshRenderer>(cube, texMat);

        auto plane = Core::MeshGenerator::CreatePlane();
        auto mr = createGameObject("GO")->addComponent<Components::MeshRenderer>(plane, alphaMat);
        mr->getGameObject()->getTransform()->position.z = -2.0f;

        auto mr22 = createGameObject("Cube")->addComponent<Components::MeshRenderer>(cube, texMat);
        mr22->getGameObject()->getTransform()->position.x = 20.0f;

        auto mr33 = createGameObject("GO")->addComponent<Components::MeshRenderer>(plane, alphaMatWrong);
        mr33->getGameObject()->getTransform()->position.x = 20.0f;
        mr33->getGameObject()->getTransform()->position.z = -2.0f;
    }
};

class TSceneBlinnPhong
    : public IScene
{
public:
    TSceneBlinnPhong() : IScene("TSceneBlinnPhong") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -3);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        go->addComponent<Components::AudioListener>();

        RENDERER.setGlobalFloat(SID("_Ambient"), 0.1f);

        auto world = createGameObject("World");
        world->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/box_n_inside.obj"), ASSETS.getMaterial("/materials/blinn_phong/cellar.material"));
        world->getTransform()->position.y = 5.0f;
        world->getTransform()->scale = 5.0f;

        auto obj = createGameObject("obj");
        obj->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/blinn_phong/monkey.material"));
        obj->getTransform()->scale = {0.5f};
        obj->getTransform()->position.y = 0.5f;

        auto plg = createGameObject("PL");
        plg->addComponent<Components::PointLight>(2.0f, Color::ORANGE, 15.0f);
        plg->getTransform()->position = { 0, 1.5f, 0 };
        plg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        auto plg2 = createGameObject("PL");
        plg2->addComponent<Components::PointLight>(2.0f, Color::BLUE, 15.0f);
        plg2->getTransform()->position = { 1.0f, 1.5f,
 0 };
        plg2->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        auto plg3 = createGameObject("PL");
        plg3->addComponent<Components::PointLight>(2.0f, Color::RED, 15.0f);
        plg3->getTransform()->position = { -1.0f, 1.5f, 0 };
        plg3->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
    }
};

class TScenePointLightShadow : public IScene
{
public:
    TScenePointLightShadow() : IScene("TSceneSpotLightShadow") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -5);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        cam->setClearColor(Color(66, 134, 244));

        RENDERER.setGlobalFloat(SID("_Ambient"), 0.2f);

        auto world = createGameObject("World");
        world->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/box_n_inside.obj"), ASSETS.getMaterial("/materials/blinn_phong/cellar.material"));
        world->getTransform()->position.y = 5.0f;
        world->getTransform()->scale = 5.0f;

        auto obj = createGameObject("obj");
        obj->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/blinn_phong/white.material"));
        obj->getTransform()->scale = { 0.5f };
        obj->getTransform()->position.y = 0.5f;

        auto obj2 = createGameObject("obj");
        obj2->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/teapot.obj"), ASSETS.getMaterial("/materials/blinn_phong/white.material"));
        obj2->getTransform()->scale = { 0.2f };
        obj2->getTransform()->position.x = 1.5f;

        auto obj3 = createGameObject("obj");
        obj3->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/sphere.obj"), ASSETS.getMaterial("/materials/blinn_phong/white.material"));
        obj3->getTransform()->scale = { 0.5f };
        obj3->getTransform()->position.y = 0.5f;
        obj3->getTransform()->position.x = -1.5f;

        // LIGHTS
        auto plg = createGameObject("PL");
        auto pl = plg->addComponent<Components::PointLight>(1.0f, Color::ORANGE, 10.0f, true);
        plg->getTransform()->position = { 0, 2, 0 };
        plg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
    }
};

class TSceneSpotLightShadow : public IScene
{
public:
    TSceneSpotLightShadow() : IScene("TSceneSpotLightShadow") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -5);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);

        RENDERER.setGlobalFloat(SID("_Ambient"), 0.2f);

        auto world = createGameObject("World");
        world->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/box_n_inside.obj"), ASSETS.getMaterial("/materials/blinn_phong/cellar.material"));
        world->getTransform()->position.y = 7.0f;
        world->getTransform()->scale = 7.0f;

        auto obj = createGameObject("obj");
        obj->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/blinn_phong/white.material"));
        obj->getTransform()->scale = { 0.5f };
        obj->getTransform()->position.y = 0.5f;

        auto obj2 = createGameObject("obj");
        obj2->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/teapot.obj"), ASSETS.getMaterial("/materials/blinn_phong/white.material"));
        obj2->getTransform()->scale = { 0.2f };
        obj2->getTransform()->position.x = 1.5f;

        auto obj3 = createGameObject("obj");
        obj3->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/sphere.obj"), ASSETS.getMaterial("/materials/blinn_phong/white.material"));
        obj3->getTransform()->scale = { 0.5f };
        obj3->getTransform()->position.y = 0.5f;
        obj3->getTransform()->position.x = -1.5f;

        // LIGHTS
        auto slg = createGameObject("PL");
        auto sl = slg->addComponent<Components::SpotLight>(1.0f, Color::WHITE, 45.0f, 20.0f, true);
        slg->getTransform()->position = { 0, 2, -2 };
        slg->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });
        slg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/spotLight.png"), 0.5f);
    }
};

class TSceneSunShadow : public IScene
{
    Components::GUI* gui;

public:
    TSceneSunShadow() : IScene("TSceneSunShadow") {}

    void init() override
    {
        // Camera 1
        auto go = createGameObject("Camera");
        auto cam = go->addComponent<Components::Camera>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 10, -25);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        cam->setClearColor(Color(66, 134, 244));

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        go->addComponent<Components::Skybox>(cubemap);

        auto obj = createGameObject("GO");
        auto grassMat = ASSETS.getMaterial("/materials/blinn_phong/grass.material");
        grassMat->setFloat("uvScale", 10.0f);
        obj->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), grassMat);
        obj->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90.0f);
        obj->getTransform()->scale = { 20,20,20 };

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
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        gui = go->addComponent<Components::GUI>();
        go->addComponent<Components::GUIFPS>();
        go->addComponent<Components::GUICustom>([this, dl, sun] {
            static F32 ambient = 0.4f;
            ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
            Locator::getRenderer().setGlobalFloat(SID("_Ambient"), ambient);

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
                    ImGui::Image(dl->getShadowMap(), Math::Vec2{ 200, 200 });
                }
                else if (type_current > 2)
                {
                    ImGui::Text("Shadow-maps");
                    for (I32 i = 0; i < dl->getCascadeCount(); ++i)
                        ImGui::Image(dl->getShadowMap(), i, Math::Vec2{ 200, 200 });
                }
                ImGui::TreePop();
            }
        });

        LOG("Press G to enable/disable the GUI", Color::BLUE);
    }

    void tick(Time::Seconds d) override
    {
        if (KEYBOARD.wasKeyPressed(Key::G))
            gui->setActive(not gui->isActive());
    }

};

// Add a vr camera component if vr is enabled, otherwise a basic camera
bool AddVRCameraComponent(GameObject* go, bool hdr = false, Graphics::MSAASamples samples = Graphics::MSAASamples::Four)
{
    if (RENDERER.hasHMD())
    {
        go->addComponent<Components::VRCamera>(Components::ScreenDisplay::BothEyes, samples, hdr);
        go->addComponent<Components::VRFPSCamera>();
        return true;
    }
    else
    {
        go->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, samples, hdr);
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        return false;
    }
}

class TPerfScene1Empty : public IScene
{
public:
    TPerfScene1Empty() : IScene("TPerfScene1Empty") {}

    void init() override
    {
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -1);

        if (not AddVRCameraComponent(go))
            LOG_WARN("VR is disabled or no VR headset found.");
    }
};

class TPerfScene2DrawCalls : public IScene
{
    static const I32 OBJECT_COUNT = 10000;
public:
    TPerfScene2DrawCalls() : IScene("TPerfScene2DrawCalls") {}

    void init() override
    {
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 10, -20);

        if (not AddVRCameraComponent(go))
            LOG_WARN("VR is disabled or no VR headset found.");

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        createGameObject("Skybox")->addComponent<Components::Skybox>(cubemap);

        // MESH
        auto cube = Core::MeshGenerator::CreateCube(1.0f);
        cube->setColors(cubeColors);

        float spacing = 3.0f;
        auto amt = (I32)std::pow(OBJECT_COUNT, 1.0 / 3.0) + 1;
        for (I32 x = 0; x < amt; ++x)
        {
            for (I32 y = 0; y < amt; ++y)
            {
                for (I32 z = 0; z < amt; ++z)
                {
                    auto go = createGameObject("Test");
                    go->addComponent<Components::MeshRenderer>(cube, ASSETS.getColorMaterial());
                    go->getComponent<Components::Transform>()->position = { (F32)x * spacing, (F32)y * spacing, (F32)z * spacing };
                }
            }
        }
    }
};

class TPerfScene3ComplexModels : public IScene
{
public:
    TPerfScene3ComplexModels() : IScene("TPerfScene3ComplexModels") {}

    void init() override
    {
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(50, 50, -50);

        if (not AddVRCameraComponent(go, true))
            LOG_WARN("VR is disabled or no VR headset found.");

        go->addComponent<Tonemap>();

        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/malibu.hdr", 2048, true);
        go->addComponent<Components::Skybox>(cubemapHDR);

        // SHADERS
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

        // Gameobjects
        F32 xSpacing = 20.0f;
        for (auto y = 0; y < 5; ++y)
        {
            F32 ySpacing = 10.0f;
            for (auto i = 0; i < 5; ++i)
            {
                auto pistolMesh = ASSETS.getMesh("/models/pistol.fbx");
                auto pistol = createGameObject("Pistol");
                pistol->addComponent<Components::MeshRenderer>(pistolMesh, ASSETS.getMaterial("/materials/pbr/pistol.pbrmaterial"));
                pistol->getTransform()->scale = { 0.1f };
                pistol->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, -90.0f);
                pistol->getTransform()->rotation *= Math::Quat(Math::Vec3::UP, -90.0f);
                pistol->getTransform()->position = { 5 + (i * xSpacing), (y * ySpacing), 0 };

                auto daggerMesh = ASSETS.getMesh("/models/dagger.obj");
                auto dagger = createGameObject("Dagger");
                dagger->addComponent<Components::MeshRenderer>(daggerMesh, ASSETS.getMaterial("/materials/pbr/dagger.pbrmaterial"));
                dagger->getTransform()->scale = { 0.1f };
                dagger->getTransform()->rotation *= Math::Quat(Math::Vec3::FORWARD, -90.0f);
                dagger->getTransform()->position = { 0 + (i * xSpacing), 4 + (y * ySpacing), 0 };
            }
        }
    }
};

class TPerfScene4PostProcessing : public IScene
{
public:
    TPerfScene4PostProcessing() : IScene("TPerfScene4PostProcessing") {}

    void init() override
    {
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);

        if (not AddVRCameraComponent(go, true))
            LOG_WARN("VR is disabled or no VR headset found.");

        go->addComponent<Tonemap>();
        go->addComponent<PostProcess<void>>(ASSETS.getMaterial("/materials/post processing/color_grading.material"));
        go->addComponent<PostProcess<F32>>(ASSETS.getShader("/shaders/post processing/pp0.shader"));
        go->addComponent<PostProcess<I64>>(ASSETS.getShader("/shaders/post processing/pp1.shader"));
        go->addComponent<PostProcess<F64>>(ASSETS.getShader("/shaders/post processing/pp2.shader"));
        go->addComponent<PostProcess<U16>>(ASSETS.getShader("/shaders/post processing/pp3.shader"));
        go->addComponent<PostProcess<I16>>(ASSETS.getShader("/shaders/post processing/pp4.shader"));
        go->addComponent<PostProcess<U32>>(ASSETS.getMaterial("/materials/post processing/gaussian_blur_horizontal.material"));
        go->addComponent<PostProcess<U64>>(ASSETS.getMaterial("/materials/post processing/gaussian_blur_vertical.material"));

        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/canyon.hdr", 2048, true);
        go->addComponent<Components::Skybox>(cubemapHDR);

        // SHADERS
        auto shader = ASSETS.getShader("/shaders/reflection.shader");

        // MESHES
        auto mesh0 = ASSETS.getMesh("/models/teapot.obj");

        // MATERIALS
        auto mat = RESOURCES.createMaterial(shader);
        mat->setTexture("Cubemap", cubemapHDR);

        // GAME OBJECTS
        auto mr = createGameObject("GO0")->addComponent<Components::MeshRenderer>(mesh0, mat);
    }
};

class TPerfScene5Lights : public IScene
{
public:
    TPerfScene5Lights() : IScene("TPerfScene5Lights") {}

    void init() override
    {
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 0, -10);

        if (not AddVRCameraComponent(go, true))
            LOG_WARN("VR is disabled or no VR headset found.");

        RENDERER.setGlobalFloat(SID("_Ambient"), 0.15f);

        // Headlight
        if (auto vrCam = go->getComponent<Components::VRCamera>())
        {
            vrCam->getCameraForEye(Graphics::VR::LeftEye).getGameObject()->
                addComponent<Components::SpotLight>(2.0f, Color::WHITE, 25.0f, 20.0f, false);
        }

        // GAME OBJECTS
        auto world = createGameObject("World");
        world->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/box_n_inside.obj"), ASSETS.getMaterial("/materials/blinn_phong/cellar.material"));
        world->getTransform()->position.y = 0.0f;
        world->getTransform()->scale = { 30.0f, 3.0f, 30.0f };

        // LIGHTING
        F32 spacing = 4.0f;
        for (auto i = 0; i < 8; ++i)
        {
            auto plg = createGameObject("PL");
            plg->addComponent<Components::PointLight>(2.0f, Math::Random::Color(), 15.0f);
            plg->getTransform()->position = { (-4 * spacing) + i * spacing, 0, -5.0f };
            plg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        }

        for (auto i = 0; i < 7; ++i)
        {
            auto plg = createGameObject("PL");
            plg->addComponent<Components::SpotLight>(4.0f, Math::Random::Color(), 45.0f, 15.0f);
            plg->getTransform()->position = { (-4 * spacing) + i * spacing, 0, 0 };
            plg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/spotlight.png"), 0.5f);
        }
    }
};

class TPerfScene6Shadows : public IScene
{
public:
    TPerfScene6Shadows() : IScene("TPerfScene6Shadows") {}

    void init() override
    {
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, -5);

        if (not AddVRCameraComponent(go, true))
            LOG_WARN("VR is disabled or no VR headset found.");

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        createGameObject("Skybox")->addComponent<Components::Skybox>(cubemap);

        auto obj = createGameObject("GO");
        obj->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(), ASSETS.getMaterial("/materials/blinn_phong/grass.material"));
        obj->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, 90.0f);
        obj->getTransform()->scale = { 25,25,25 };

        auto obj2 = createGameObject("GO2");
        obj2->addComponent<Components::MeshRenderer>(ASSETS.getMesh("/models/monkey.obj"), ASSETS.getMaterial("/materials/blinn_phong/white.material"));
        obj2->getTransform()->position = { 5, 1, 0 };

        auto cubeGO = createGameObject("GO3");
        cubeGO->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreateCubeUV(0.3f), ASSETS.getMaterial("/materials/blinn_phong/cube.material"));
        cubeGO->getTransform()->position = { -5.0f, 0.3001f, 0.0f };
        cubeGO->addComponent<ConstantRotation>(0.0f, 10.0f, 0.0f);

        Assets::MeshMaterialInfo matInfo;
        auto treeMesh = ASSETS.getMesh("/models/tree/tree.obj", &matInfo);

        ArrayList<Math::Vec3> treePositions;
        I32 treeSpacing = 10;
        for (I32 x = -20; x <= 20; x += treeSpacing)
            for (I32 z = -20; z <= 20;  z += treeSpacing)
                treePositions.emplace_back((F32)x, 0.0f, (F32)z);

        ArrayList<Components::MeshRenderer*> treeMeshRenderer;

        ArrayList<GameObject*> trees;
        for (I32 i = 0; i < (I32)treePositions.size(); ++i)
        {
            auto tree = createGameObject("Tree");
            tree->getTransform()->position = treePositions[i];
            tree->getTransform()->scale = Math::Random::Vec3(0.8f, 1.2f);
            trees.push_back(tree);

            auto mr = tree->addComponent<Components::MeshRenderer>(treeMesh);
            treeMeshRenderer.push_back(mr);
        }

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
                for (auto& mr : treeMeshRenderer)
                    mr->setMaterial(material, i);
            }
        }

        RENDERER.setGlobalFloat(SID("_Ambient"), 0.3f);

        // LIGHTS
        auto sun = createGameObject("Sun");
        sun->addComponent<Components::DirectionalLight>(0.3f, Color::WHITE, Graphics::ShadowType::CSMSoft, ArrayList<F32>{10.0f, 30.0f, 80.0f, 200.0f});
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });

        auto sun2 = createGameObject("Sun");
        sun2->addComponent<Components::DirectionalLight>(0.3f, Color::WHITE, Graphics::ShadowType::Soft);
        sun2->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 1,-1, 0 });

        auto plg = createGameObject("PL");
        plg->addComponent<Components::PointLight>(1.0f, Color::ORANGE, 10.0f, true);
        plg->getTransform()->position = { 3, 4, 0 };
        plg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);

        auto slg = createGameObject("SL");
        slg->addComponent<Components::SpotLight>(1.0f, Color::WHITE, 25.0f, 20.0f);
        slg->getTransform()->position = { -5, 2, -2 };
        slg->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 1 });
        slg->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/spotLight.png"), 0.5f);

        auto slg2 = createGameObject("SL");
        slg2->addComponent<Components::SpotLight>(2.5f, Color::RED, 25.0f, 20.0f);
        slg2->getTransform()->position = { 0, 10, 0 };
        slg2->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 0 }, Math::Vec3::FORWARD);
        slg2->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/spotLight.png"), 0.5f);

        auto slg3 = createGameObject("SL");
        slg3->addComponent<Components::SpotLight>(2.5f, Color::VIOLET, 25.0f, 20.0f);
        slg3->getTransform()->position = { 0, 10, 10 };
        slg3->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 0 }, Math::Vec3::FORWARD);
        slg3->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/spotLight.png"), 0.5f);
    }
};

class TPerfScene7ParticleSystems : public IScene
{
public:
    TPerfScene7ParticleSystems() : IScene("TPerfScene7ParticleSystems") {}

    void init() override
    {
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, -5);

        if (not AddVRCameraComponent(go, true))
            LOG_WARN("VR is disabled or no VR headset found.");

        auto cubemap = ASSETS.getCubemap("/cubemaps/bkg/blue/right.png", "/cubemaps/bkg/blue/left.png",
            "/cubemaps/bkg/blue/top.png", "/cubemaps/bkg/blue/bot.png",
            "/cubemaps/bkg/blue/front.png", "/cubemaps/bkg/blue/back.png");
        createGameObject("Skybox")->addComponent<Components::Skybox>(cubemap);

        auto celestialNoise = ASSETS.getAudioClip("/audio/Celestial Galaxy.wav");
        celestialNoise->setIsLooping(true);
        go->addComponent<Components::AudioSource>(celestialNoise);

        createGameObject("Particles!")->addComponent<Components::ParticleSystem>("/particles/asteroids.ps");
        createGameObject("Particles!")->addComponent<Components::ParticleSystem>("/particles/stars.ps");
    }
};

class TPerfScene8AllInOne : public IScene
{
public:
    TPerfScene8AllInOne() : IScene("TPerfScene8AllInOne") {}

    void init() override
    {
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 5, -1);

        if (not AddVRCameraComponent(go, true))
            LOG_WARN("VR is disabled or no VR headset found.");

        go->addComponent<Tonemap>();

        RENDERER.setGlobalFloat(SID("_Ambient"), 0.2f);

        // SHADERS
        auto cubemapHDR = ASSETS.getCubemap("/cubemaps/desert_valley.hdr", 2048, true);
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

        // OBJECTS
        createGameObject("Skybox")->addComponent<Components::Skybox>(cubemapHDR);

        Assets::MeshMaterialInfo materialImportInfo;
        auto mesh = ASSETS.getMesh("/models/sponza/sponza.obj", &materialImportInfo);

        auto obj = createGameObject("Obj");
        auto mr = obj->addComponent<Components::MeshRenderer>(mesh, nullptr);
        obj->getTransform()->scale = { 0.01f };

        if (materialImportInfo.isValid())
        {
            auto pbrMaterials = GeneratePBRMaterials(pbrShader, mesh, materialImportInfo);
            for (I32 i = 0; i < pbrMaterials.size(); i++)
                mr->setMaterial(pbrMaterials[i], i);
        }

        // LIGHTS
        auto sun = createGameObject("Sun");
        auto dl = sun->addComponent<Components::DirectionalLight>(10.0f, Color::WHITE, Graphics::ShadowType::CSMSoft, ArrayList<F32>{10.0f, 30.0f, 80.0f, 200.0f});
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 0.1f });

        F32 pointLightIntensity = 10.0f;
        F32 pointLightRange = 5.0f;
        Color pointLightColor = Color::ORANGE;

        auto pl = createGameObject("PointLight");
        pl->addComponent<Components::PointLight>(pointLightIntensity, pointLightColor, pointLightRange);
        pl->getTransform()->position = { 4.9f, 1.55f, 1.4f };
        pl->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        //pl->addComponent<Components::ParticleSystem>("/particles/smoke.ps");

        auto pl2 = createGameObject("PointLight");
        pl2->addComponent<Components::PointLight>(pointLightIntensity, pointLightColor, pointLightRange);
        pl2->getTransform()->position = { 4.9f, 1.55f, -2.2f };
        pl2->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        //pl2->addComponent<Components::ParticleSystem>("/particles/smoke.ps");

        auto pl3 = createGameObject("PointLight");
        pl3->addComponent<Components::PointLight>(pointLightIntensity, pointLightColor, pointLightRange);
        pl3->getTransform()->position = { -6.2f, 1.55f, 1.4f };
        pl3->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        //pl3->addComponent<Components::ParticleSystem>("/particles/smoke.ps");

        auto pl4 = createGameObject("PointLight");
        pl4->addComponent<Components::PointLight>(pointLightIntensity, pointLightColor, pointLightRange, true);
        pl4->getTransform()->position = { -6.2f, 1.55f, -2.2f };
        pl4->addComponent<Components::Billboard>(ASSETS.getTexture2D("/engine/textures/pointLight.png"), 0.5f);
        //pl4->addComponent<Components::ParticleSystem>("/particles/smoke.ps");

        // PARTICLE SYSTEMS
        createGameObject("Particles!")->addComponent<Components::ParticleSystem>("/particles/ambient.ps");
        createGameObject("Particles!")->addComponent<Components::ParticleSystem>("/particles/dust.ps");
    }
};


//----------------------------------------------------------------------
class SceneGUISelectTestMenu : public IScene
{
public:
    SceneGUISelectTestMenu() : IScene("SceneGUISelectTestMenu") {}

    void init() override
    {
        auto gui = createGameObject("GUI");
        gui->addComponent<Components::Camera>(45.0f, 0.1f, 1000.0f, Graphics::MSAASamples::One);
        gui->addComponent<Components::GUI>();

        auto guiSceneMenu = gui->addComponent<GUISceneMenu>("Test Scenes");
        guiSceneMenu->registerScene<TSceneAlphaProblemDemo>("Alpha Problem Demo");
        guiSceneMenu->registerScene<TSceneBlinnPhong>("Blinn Phong Lighting");
        guiSceneMenu->registerScene<TScenePointLightShadow>("Point Light Shadow");
        guiSceneMenu->registerScene<TSceneSpotLightShadow>("Spot Light Shadow");
        guiSceneMenu->registerScene<TSceneSunShadow>("Sun Light Shadow");
        guiSceneMenu->registerScene<TPerfScene1Empty>("VR #1: Empty");
        guiSceneMenu->registerScene<TPerfScene2DrawCalls>("VR #2: Draw Calls");
        guiSceneMenu->registerScene<TPerfScene3ComplexModels>("VR #3: Complex Models");
        guiSceneMenu->registerScene<TPerfScene4PostProcessing>("VR #4: Post Processing");
        guiSceneMenu->registerScene<TPerfScene5Lights>("VR #5: Lighting");
        guiSceneMenu->registerScene<TPerfScene6Shadows>("VR #6: Shadows");
        guiSceneMenu->registerScene<TPerfScene7ParticleSystems>("VR #7: Particle Systems");
        guiSceneMenu->registerScene<TPerfScene8AllInOne>("VR #8: All-In-One");
    }
};