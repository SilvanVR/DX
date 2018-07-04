#include "configuration_manager.h"

/**********************************************************************
    class: ConfigurationManager (configuration_manager.cpp)

    author: S. Hau
    date: October 20, 2017

**********************************************************************/

#include "Common/string_utils.h"
#include "Core/locator.h"
#include "GameplayLayer/i_scene.h"
#include "GameplayLayer/Components/Rendering/i_light_component.h"

#define CATEGORY_QUALITY    "Quality"
#define SHADOW_MAP_QUALITY  "ShadowQuality"

namespace Core { namespace Config {


    //----------------------------------------------------------------------
    void ConfigurationManager::init()
    {
        m_engineIni = new ConfigFile( "/engine/engine.ini" );
        m_shadowMapQuality = _ReadShadowQuality();
    }

    //----------------------------------------------------------------------
    void ConfigurationManager::shutdown()
    {
        delete m_engineIni;
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void ConfigurationManager::setShadowMapQuality( Graphics::ShadowMapQuality quality )
    {
        m_shadowMapQuality = quality;
        for (auto& light : SCENE.getComponentManager().getLights() )
            light->setShadowMapQuality( m_shadowMapQuality );
    }

    //----------------------------------------------------------------------
    void ConfigurationManager::setShadowType( Graphics::ShadowType shadowType )
    {
        for (auto& light : SCENE.getComponentManager().getLights() )
            light->setShadowType( shadowType );
    }

    //----------------------------------------------------------------------
    void ConfigurationManager::setShadowTypeAndQuality( Graphics::ShadowType shadowType, Graphics::ShadowMapQuality quality )
    {
        for (auto& light : SCENE.getComponentManager().getLights() )
            light->setShadowTypeAndQuality( shadowType, quality );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    Graphics::ShadowMapQuality ConfigurationManager::_ReadShadowQuality()
    {
        auto quality = getEngineIni()[CATEGORY_QUALITY][SHADOW_MAP_QUALITY];
        if ( not quality.isValid() )
            return Graphics::ShadowMapQuality::High;

        if (quality.getType() == Common::EVariantType::String)
        {
            String name = StringUtils::toLower( quality.get<CString>() );
            if (name == "low")      return Graphics::ShadowMapQuality::Low;
            if (name == "medium")   return Graphics::ShadowMapQuality::Medium;
            if (name == "high")     return Graphics::ShadowMapQuality::High;
            if (name == "insane")   return Graphics::ShadowMapQuality::Insane;
        }

        return Graphics::ShadowMapQuality::High;
    }


} } // end namespaces