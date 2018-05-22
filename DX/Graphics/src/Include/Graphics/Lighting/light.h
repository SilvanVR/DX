#pragma once
/**********************************************************************
    class: Light (light.h)

    author: S. Hau
    date: May 22, 2018
**********************************************************************/

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
    struct LightAttenuation
    {
        F32 constant;
        F32 linear;
        F32 quadratic;
    };

    //**********************************************************************
    class Light
    {
    public:
        Light(LightType lightType, F32 intensity, Color color)
            : m_lightType(lightType), m_color(color), m_intensity(intensity) {}
        virtual ~Light() {}

        //----------------------------------------------------------------------
        LightType   getLightType()  const { return m_lightType; }
        Color       getColor()      const { return m_color; }
        F32         getIntensity()  const { return m_intensity; }

        //----------------------------------------------------------------------
        void        setColor(Color color)       { m_color = color; }
        void        setIntensity(F32 intensity) { m_intensity = intensity; }

    protected:
        F32         m_intensity = 1.0f;
        Color       m_color     = Color::WHITE;
        LightType   m_lightType = LightType::Unknown;

    private:
        //----------------------------------------------------------------------
        Light(const Light& other)               = delete;
        Light& operator = (const Light& other)  = delete;
        Light(Light&& other)                    = delete;
        Light& operator = (Light&& other)       = delete;
    };

} // End namespaces