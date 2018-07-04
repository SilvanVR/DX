#pragma once
/**********************************************************************
    class: ConfigurationManager (configuration_manager.h)

    author: S. Hau
    date: October 20, 2017

    Manages initial configuration stuff and files.
**********************************************************************/

#include "Common/i_subsystem.hpp"
#include "config_file.h"
#include "Graphics/Lighting/lights.h"

namespace Core { namespace Config {

    //*********************************************************************
    // Manages a configuration file on disk.
    //*********************************************************************
    class ConfigurationManager : public ISubSystem
    {
    public:
        ConfigurationManager() = default;
        ~ConfigurationManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // Return the main engine.ini file-object
        //----------------------------------------------------------------------
        ConfigFile& getEngineIni() { return (*m_engineIni); }

        //----------------------------------------------------------------------
        // @Return: Shadowmap quality read from engine.ini
        //----------------------------------------------------------------------
        const Graphics::ShadowMapQuality getShadowMapQuality() const { return m_shadowMapQuality; }

        //----------------------------------------------------------------------
        // Sets the shadowmap quality for ALL shadowmaps.
        //----------------------------------------------------------------------
        void setShadowMapQuality(Graphics::ShadowMapQuality quality);

        //----------------------------------------------------------------------
        // Actives/Deactives shadows for all lights in the scene.
        //----------------------------------------------------------------------        
        void setShadows(Graphics::ShadowType shadowType);

    private:
        ConfigFile*                 m_engineIni = nullptr;
        Graphics::ShadowMapQuality  m_shadowMapQuality;

        //----------------------------------------------------------------------
        Graphics::ShadowMapQuality _ReadShadowQuality();

        NULL_COPY_AND_ASSIGN(ConfigurationManager)
    };






} }