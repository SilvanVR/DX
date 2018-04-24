#pragma once
#include "Common/data_types.hpp"
#include "Common/string.h"
#include "Common/macros.hpp"
#include "Math/dxmath_wrapper.h"

struct NoiseMapParams
{
    F32 scale;
    F32 lacunarity;
    F32 gain;
    I32 octaves;
};

class NoiseMap
{
public:
    NoiseMap(I32 seed, const NoiseMapParams& params);
    ~NoiseMap() {}

    F32 get(I32 x, I32 y, const Math::Vec2Int& offset) const;

private:
    NoiseMapParams  m_params;
    I32             m_seed;
};