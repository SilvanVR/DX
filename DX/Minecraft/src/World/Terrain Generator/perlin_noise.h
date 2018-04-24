#pragma once
#include "Common/data_types.hpp"
#include "Common/string.h"
#include "Common/macros.hpp"
#include "Math/dxmath_wrapper.h"

struct NoiseParams
{
    F32 scale;
    F32 lacunarity;
    F32 gain;
    I32 octaves;
};

class PerlinNoise
{
public:
    PerlinNoise(I32 seed, const NoiseParams& params);
    ~PerlinNoise() {}

    F32 get(I32 x, I32 y, const Math::Vec2Int& offset) const;

private:
    NoiseParams m_params;
    I32         m_seed;
};