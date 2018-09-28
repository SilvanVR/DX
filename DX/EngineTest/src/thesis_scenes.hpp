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
bool AddVRCameraComponent(GameObject* go, Graphics::MSAASamples samples = Graphics::MSAASamples::Four)
{
    if (RENDERER.hasHMD())
    {
        go->addComponent<Components::VRCamera>(Components::ScreenDisplay::BothEyes, samples);
        go->addComponent<Components::VRFPSCamera>();
        return true;
    }
    else
    {
        go->addComponent<Components::Camera>();
        go->addComponent<Components::FPSCamera>(Components::FPSCamera::MAYA, 0.1f);
        return false;
    }
}

class TPerfScene0Empty : public IScene
{
public:
    TPerfScene0Empty() : IScene("TPerfScene0Empty") {}

    void init() override
    {
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -1);

        if (not AddVRCameraComponent(go))
            LOG_WARN("VR is disabled or no VR headset found.");
    }
};

class TPerfScene1DrawCalls : public IScene
{
    static const I32 OBJECT_COUNT = 10000;
public:
    TPerfScene1DrawCalls() : IScene("TPerfScene1DrawCalls") {}

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

class TPerfSceneSponza : public IScene
{
public:
    TPerfSceneSponza() : IScene("TPerfSceneSponza") {}

    void init() override
    {
        auto go = createGameObject("Camera");
        go->addComponent<Components::AudioListener>();
        go->getComponent<Components::Transform>()->position = Math::Vec3(0, 1, -1);

        if (not AddVRCameraComponent(go))
            LOG_WARN("VR is disabled or no VR headset found.");

        auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
            "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
            "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        createGameObject("Skybox")->addComponent<Components::Skybox>(cubemap);

        Assets::MeshMaterialInfo materialImportInfo;
        auto mesh = ASSETS.getMesh("/models/sponza/sponza.obj", &materialImportInfo);

        auto obj = createGameObject("Sponza");
        auto mr = obj->addComponent<Components::MeshRenderer>(mesh, nullptr);
        obj->getTransform()->scale = { 0.03f };

        // LIGHTS
        auto sun = createGameObject("Sun");
        auto dl = sun->addComponent<Components::DirectionalLight>(0.5f, Color::WHITE, Graphics::ShadowType::CSMSoft, ArrayList<F32>{10.0f, 30.0f, 80.0f, 200.0f});
        sun->getTransform()->rotation = Math::Quat::LookRotation(Math::Vec3{ 0,-1, 0.1f });

        if (materialImportInfo.isValid())
        {
            for (I32 i = 0; i < mesh->getSubMeshCount(); i++)
            {
                auto mat = RESOURCES.createMaterial(ASSETS.getShader("/shaders/phong_shadow.shader"));
                mat->setFloat("uvScale", 1.0f);

                for (auto& texture : materialImportInfo[i].textures)
                {
                    switch (texture.type)
                    {
                    case Assets::MaterialTextureType::Albedo: mat->setTexture("_MainTex", ASSETS.getTexture2D(texture.filePath)); break;
                    }
                }
                mr->setMaterial(mat, i);
            }
        }
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
        guiSceneMenu->registerScene<TPerfScene0Empty>("VR #0: Empty");
        guiSceneMenu->registerScene<TPerfScene1DrawCalls>("VR #1: Draw Calls");
    }
};