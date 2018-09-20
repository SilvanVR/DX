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

        LOG("TSceneAlphaProblemDemo initialized!", Color::RED);
    }

    void shutdown() override { LOG("TSceneAlphaProblemDemo Shutdown!", Color::RED); }
};