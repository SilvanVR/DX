#pragma once
/**********************************************************************
    class: Light (light.h)

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
        const RenderBufferPtr&      getShadowMap()              const { return m_shadowMap; }
        const DirectX::XMMATRIX&    getShadowViewProjection()   const { return m_shadowViewProjection; }

        //----------------------------------------------------------------------
        void        setColor            (Color color)   { m_color = color; }
        void        setIntensity        (F32 intensity) { m_intensity = intensity; }
        void        setShadowsEnabled   (bool enabled)  { m_shadowsEnabled = enabled; }
        void        enableShadows       ()              { setShadowsEnabled(true); }
        void        disableShadows      ()              { setShadowsEnabled(false); }

        void        setShadowMap            (const RenderBufferPtr& shadowMap)  { m_shadowsEnabled = true; m_shadowMap = shadowMap; }
        void        setShadowViewProjection (const DirectX::XMMATRIX& vp)       { m_shadowViewProjection  = vp; }

    protected:
        F32                 m_intensity         = 1.0f;
        Color               m_color             = Color::WHITE;
        LightType           m_lightType         = LightType::Unknown;
        bool                m_shadowsEnabled    = false;
        RenderBufferPtr     m_shadowMap         = nullptr;
        DirectX::XMMATRIX   m_shadowViewProjection;

    private:
        NULL_COPY_AND_ASSIGN(Light)
    };

} // End namespaces