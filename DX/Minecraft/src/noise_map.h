#pragma once
#include <DX.h>

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
    NoiseMap(I32 width, I32 height, const NoiseMapParams& params, Math::Vec2Int offset = { 0, 0 });
    ~NoiseMap() { delete m_noiseMap; }

    I32 getWidth() const { return m_width; }
    I32 getHeight() const { return m_height; }
    F32 get(I32 x, I32 y) const { return m_noiseMap[x + y * m_width]; }

private:
    F32*    m_noiseMap = nullptr;
    I32     m_width, m_height;
};