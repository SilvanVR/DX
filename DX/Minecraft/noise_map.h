#pragma once
#include <DX.h>

#define STB_PERLIN_IMPLEMENTATION 
#include "stb_perlin.hpp"

class NoiseMap
{
public:
    NoiseMap(I32 width, I32 height, F32 scale = 0.3f, F32 lacunarity = 2.0f, F32 gain = 0.5f, I32 octaves = 4)
        : m_width(width), m_height(height)
    {
        m_noiseMap = new F32[width * height];

        if (scale < 0.0f)
            scale = 0.0001f;

        F32 halfWidth = width * 0.5f;
        F32 halfHeight = height * 0.5f;

        for (I32 y = 0; y < height; y++)
        {
            for (I32 x = 0; x < width; x++)
            {
                F32 sampleX = (x - halfWidth) / scale;
                F32 sampleY = (y - halfHeight) / scale;

                F32 noiseValue = stb_perlin_turbulence_noise3(sampleX, sampleY, 0.0f, lacunarity, gain, octaves, 0, 0, 0);
                m_noiseMap[x + y * width] = Math::clamp(noiseValue, 0.0f, 1.0f);
            }
        }
    }
    ~NoiseMap() { delete m_noiseMap; }

    I32 getWidth() const { return m_width; }
    I32 getHeight() const { return m_height; }
    F32 getValue(I32 x, I32 y) const { return m_noiseMap[x + y * m_width]; }

private:
    F32*    m_noiseMap = nullptr;
    I32     m_width, m_height;
};