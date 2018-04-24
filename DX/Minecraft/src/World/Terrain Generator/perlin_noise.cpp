#include "perlin_noise.h"

#define STB_PERLIN_IMPLEMENTATION 
#include "../../ext/stb_perlin.hpp"

#include "Math/math_utils.h"

PerlinNoise::PerlinNoise(I32 seed, const NoiseParams& params)
    : m_seed(seed), m_params(params)
{
}

F32 PerlinNoise::get(I32 x, I32 y, const Math::Vec2Int& offset) const
{
    F32 scale = m_params.scale < 0.0f ? 0.0001f : m_params.scale;

    F32 sampleX = (m_seed + x + offset.x) / scale;
    F32 sampleY = (m_seed + y + offset.y) / scale;

    F32 noiseValue = stb_perlin_turbulence_noise3(sampleX, sampleY, 0.0f, m_params.lacunarity, m_params.gain, m_params.octaves, 0, 0, 0);
    ASSERT(noiseValue <= 1.0f);

    return noiseValue;
}