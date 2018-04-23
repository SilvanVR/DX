#pragma once

#include <DX.h>
#include "Terrain Generator/noise_map.h"
#include "world_constants.h"

//**********************************************************************
class NoiseMapVisualizer : public Components::IComponent
{
    MaterialPtr m_noiseMapMaterial;

    NoiseMapParams m_noiseParams = {
        50.0f, 2.0f, 0.3f, 4
    };
    F32 m_speed = 10.0f;

public:
    //----------------------------------------------------------------------
    void init() override
    {
        // VISUALIZATION OF PERLIN NOISE ON A FLAT PLANE
        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");
        m_noiseMapMaterial = RESOURCES.createMaterial(texShader);
        SCENE.createGameObject()->addComponent<Components::MeshRenderer>(Core::Assets::MeshGenerator::CreatePlane(1, Color::GREEN), m_noiseMapMaterial);
    }

    F32 lastScale = 0.0f;
    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        if (KEYBOARD.isKeyDown(Key::Up))
            m_noiseParams.scale += (F32)delta.value * m_speed * 10.0f;
        if (KEYBOARD.isKeyDown(Key::Down))
            m_noiseParams.scale -= (F32)delta.value * m_speed * 10.0f;

        static F32 lastLacunarity = 0.0f;
        if (KEYBOARD.isKeyDown(Key::T))
            m_noiseParams.lacunarity += (F32)delta.value * m_speed;
        if (KEYBOARD.isKeyDown(Key::G))
            m_noiseParams.lacunarity -= (F32)delta.value * m_speed;

        static F32 lastGain = 0.0f;
        if (KEYBOARD.isKeyDown(Key::Z))
            m_noiseParams.gain += (F32)delta.value * m_speed * 0.1f;
        if (KEYBOARD.isKeyDown(Key::H))
            m_noiseParams.gain -= (F32)delta.value * m_speed * 0.1f;

        static I32 lastOctaves = 0;
        if (KEYBOARD.wasKeyPressed(Key::U))
            m_noiseParams.octaves++;
        if (KEYBOARD.wasKeyPressed(Key::J))
            m_noiseParams.octaves--;

        if ( (lastScale != m_noiseParams.scale || lastLacunarity != m_noiseParams.lacunarity || lastGain != m_noiseParams.gain
            || lastOctaves != m_noiseParams.octaves))
        {
            lastScale = m_noiseParams.scale; lastLacunarity = m_noiseParams.lacunarity; lastGain = m_noiseParams.gain;
            lastOctaves = m_noiseParams.octaves;

            ASYNC_JOB([&] {
                NoiseMap noiseMap(CHUNK_SIZE * 10, CHUNK_SIZE * 10, m_noiseParams);
                m_noiseMapMaterial->setTexture("tex", _GenerateNoiseTextureFromNoiseMap(noiseMap));
            });

            LOG("Scale: " + TS(m_noiseParams.scale));
            LOG("Lacunarity: " + TS(m_noiseParams.lacunarity));
            LOG("Gain: " + TS(m_noiseParams.gain));
            LOG("Octaves: " + TS(m_noiseParams.octaves));
        }
    }

private:
    //----------------------------------------------------------------------
    Texture2DPtr _GenerateNoiseTextureFromNoiseMap(const NoiseMap& noiseMap)
    {
        auto tex2D = RESOURCES.createTexture2D(noiseMap.getWidth(), noiseMap.getHeight(), Graphics::TextureFormat::BGRA32);
        for (U32 x = 0; x < tex2D->getWidth(); x++)
        {
            for (U32 y = 0; y < tex2D->getHeight(); y++)
            {
                F32 curHeight = noiseMap.get(x, y);
                tex2D->setPixel(x, y, Color::Lerp(Color::BLACK, Color::WHITE, curHeight));
            }
        }
        tex2D->setAnisoLevel(1);
        tex2D->setFilter(Graphics::TextureFilter::Point);
        tex2D->apply();

        return tex2D;
    }
};
