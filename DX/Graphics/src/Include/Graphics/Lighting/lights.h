#pragma once
/**********************************************************************
    class: Several

    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "../i_renderbuffer.hpp"

namespace Graphics
{
    //----------------------------------------------------------------------
    enum class LightType
    {
        Unknown     = -1,
        Directional = 0,
        Point       = 1,
        Spot        = 2
    };

    //----------------------------------------------------------------------
    enum class ShadowMapQuality
    {
        Low,
        Medium,
        High,
        Insane
    };

    //**********************************************************************
    class Light
    {
    public:
        Light(LightType lightType, F32 intensity, Color color)
            : m_lightType(lightType), m_color(color), m_intensity(intensity) {}
        virtual ~Light() {}

        //----------------------------------------------------------------------
        LightType                   getLightType()              const { return m_lightType; }
        Color                       getColor()                  const { return m_color; }
        F32                         getIntensity()              const { return m_intensity; }
        bool                        shadowsEnabled()            const { return m_shadowsEnabled; }
        const TexturePtr&           getShadowMap()              const { return m_shadowMap; }
        const DirectX::XMMATRIX&    getShadowViewProjection()   const { return m_shadowViewProjection; }

        //----------------------------------------------------------------------
        void        setColor            (Color color)   { m_color = color; }
        void        setIntensity        (F32 intensity) { m_intensity = intensity; }
        void        setShadows          (bool enabled)  { m_shadowsEnabled = enabled; }
        void        enableShadows       ()              { setShadows(true); }
        void        disableShadows      ()              { setShadows(false); }

        void        setShadowMap            (const TexturePtr& shadowMap) { m_shadowsEnabled = true; m_shadowMap = shadowMap; }
        void        setShadowViewProjection (const DirectX::XMMATRIX& vp) { m_shadowViewProjection  = vp; }

    protected:
        F32                 m_intensity         = 1.0f;
        Color               m_color             = Color::WHITE;
        LightType           m_lightType         = LightType::Unknown;
        bool                m_shadowsEnabled    = false;
        TexturePtr          m_shadowMap         = nullptr;
        DirectX::XMMATRIX   m_shadowViewProjection;

    private:
        NULL_COPY_AND_ASSIGN(Light)
    };

    //**********************************************************************
    class DirectionalLight : public Light
    {
    public:
        DirectionalLight(F32 intensity, Color color, const Math::Vec3& direction = { 0, 0, 0 })
            : Light(LightType::Directional, intensity, color), m_direction(direction) {}
        ~DirectionalLight() = default;

        //----------------------------------------------------------------------
        const Math::Vec3&   getDirection()      const { return m_direction; }
        F32                 getShadowRange()    const { return m_shadowRange; }

        //----------------------------------------------------------------------
        void setDirection(const Math::Vec3& dir) { m_direction = dir; }
        void setShadowRange(F32 shadowRange) { m_shadowRange = shadowRange; }

    private:
        Math::Vec3  m_direction;
        F32         m_shadowRange = 30.0f;

        NULL_COPY_AND_ASSIGN(DirectionalLight)
    };

    //**********************************************************************
    class PointLight : public Light
    {
    public:
        PointLight(F32 intensity, Color color, const Math::Vec3& position = { 0, 0, 0 }, F32 range = 5.0f )
            : Light( LightType::Point, intensity, color ), m_position( position ), m_range( range ) {}
        ~PointLight() = default;

        //----------------------------------------------------------------------
        const Math::Vec3& getPosition()   const { return m_position; }
        F32               getRange()      const { return m_range; }

        //----------------------------------------------------------------------
        void setPosition (const Math::Vec3& pos) { m_position = pos; }
        void setRange    (F32 range)             { m_range = range; }

    protected:
        // Constructor for spot-light
        PointLight(LightType lightType, F32 intensity, Color color, const Math::Vec3& position, F32 range)
            : Light( lightType, intensity, color ), m_position( position ), m_range( range ) {}

    private:
        Math::Vec3   m_position = { 0, 0, 0 };
        F32          m_range    = 5.0f;

        NULL_COPY_AND_ASSIGN(PointLight)
    };

    //**********************************************************************
    class SpotLight : public PointLight
    {
    public:
        SpotLight(F32 intensity, Color color, const Math::Vec3& position = { 0, 0, 0 }, F32 angleInDeg = 90.0f, const Math::Vec3& direction = { 0, 0, 1 }, F32 range = 10.0f);
        ~SpotLight() = default;

        //----------------------------------------------------------------------
        F32                  getAngle()          const { return m_angle; }
        const Math::Vec3&    getDirection()      const { return m_direction; }

        //----------------------------------------------------------------------
        void setAngle    (F32 angleInDegree);
        void setDirection(const Math::Vec3& direction) { m_direction = direction; }

    private:
        Math::Vec3  m_direction;
        F32         m_angle;

        NULL_COPY_AND_ASSIGN(SpotLight)
    };

} // End namespaces