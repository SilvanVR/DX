#pragma once
#include <DX.h>
using namespace Core;

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

    void tick( Time::Seconds delta ) override
    {
        auto t = getComponent<Components::Transform>();
        t->rotation = Math::Quat::FromEulerAngles( m_curDegrees );
        m_curDegrees += m_speeds * (F32)delta.value;
    }
};

class WorldGeneration : public Components::IComponent
{
    MeshPtr                     mesh;
    Components::MeshRenderer*   mr;

public:
    void addedToGameObject(GameObject* go) override
    {
        mesh = Assets::MeshGenerator::CreatePlane();
        mesh->setColors(planeColors);
        mesh->setBufferUsage(Graphics::BufferUsage::Frequently);

        mr = go->addComponent<Components::MeshRenderer>(mesh, RESOURCES.getColorMaterial());
    }

    void tick(Time::Seconds delta)
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
    void addedToGameObject(GameObject* go) override
    {
        generateMesh();
        auto transform = go->getComponent<Components::Transform>();
        transform->rotation = Math::Quat(Math::Vec3::RIGHT, 90);
        transform->position = Math::Vec3(-(width/2.0f), -2.0f, -(height/2.0f));

        mr = go->addComponent<Components::MeshRenderer>(mesh, RESOURCES.getColorMaterial());
    }

    void tick(Time::Seconds delta)
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

    void addedToGameObject(GameObject* go) override
    {
        auto mesh = Assets::MeshGenerator::CreateGrid(m_size);
        go->addComponent<Components::MeshRenderer>(mesh, RESOURCES.getColorMaterial());
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

    void tick(Time::Seconds delta) override
    {
        auto t = getComponent<Components::Transform>();

        Math::Vec3 vecXZ = t->position;
        vecXZ.y = 0.0f;
        F32 length = vecXZ.magnitude();

        t->position.x = m_center.x + length * cos( Math::Deg2Rad( m_curDegrees ) );
        t->position.z = m_center.z + length * sin( Math::Deg2Rad( m_curDegrees ) );

        t->lookAt(m_center);

        m_curDegrees += m_speed * (F32)delta.value;
    }
};

class DrawFrustum : public Components::IComponent
{
public:
    void addedToGameObject(GameObject* go) override
    {
        auto cam = go->getComponent<Components::Camera>();
        auto mesh = Assets::MeshGenerator::CreateFrustum( Math::Vec3(0), Math::Vec3::UP, Math::Vec3::RIGHT, Math::Vec3::FORWARD, 
                                                          cam->getFOV(), cam->getZNear(), cam->getZFar(), cam->getAspectRatio() );

        auto mr = go->addComponent<Components::MeshRenderer>(mesh, RESOURCES.getColorMaterial());
    }
};