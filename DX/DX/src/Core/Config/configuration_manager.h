#pragma once

/**********************************************************************
    class: ConfigurationManager (configuration_manager.h)

    author: S. Hau
    date: October 20, 2017

    Manages initial configuration stuff and files.
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "config_file.h"

namespace Core { namespace Config {

    //*********************************************************************
    // Manages a configuration file on disk.
    //*********************************************************************
    class ConfigurationManager : public ISubSystem
    {
    public:
        ConfigurationManager();

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // Return the main engine.ini file-object
        //----------------------------------------------------------------------
        ConfigFile& getEngineIni() { return (*m_engineIni); }

    private:
        ConfigFile* m_engineIni;

        //----------------------------------------------------------------------
        ConfigurationManager(const ConfigurationManager& other)               = delete;
        ConfigurationManager& operator = (const ConfigurationManager& other)  = delete;
        ConfigurationManager(ConfigurationManager&& other)                    = delete;
        ConfigurationManager& operator = (ConfigurationManager&& other)       = delete;
    };






} }