#include "noise_map.h"

#define STB_PERLIN_IMPLEMENTATION 
#include "ext/stb_perlin.hpp"

NoiseMap::NoiseMap(I32 width, I32 height, const NoiseMapParams& params, Math::Vec2Int offset)
    : m_width(width), m_height(height)
{
    m_noiseMap = new F32[m_width * m_height];

    F32 scale = params.scale < 0.0f ? 0.0001f : params.scale;

    F32 halfWidth = m_width * 0.5f;
    F32 halfHeight = m_height * 0.5f;

    for (I32 y = 0; y < m_height; y++)
    {
        for (I32 x = 0; x < m_width; x++)
        {
            F32 sampleX = (x - halfWidth + offset.x) / scale;
            F32 sampleY = (y - halfHeight + offset.y) / scale;

            F32 noiseValue = stb_perlin_turbulence_noise3(sampleX, sampleY, 0.0f, params.lacunarity, params.gain, params.octaves, 0, 0, 0);
            ASSERT(noiseValue <= 1.0f);
            m_noiseMap[x + y * m_width] = Math::clamp(noiseValue, 0.0f, 1.0f);
        }
    }
}