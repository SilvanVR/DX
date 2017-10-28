#include "configuration_manager.h"

/**********************************************************************
    class: ConfigurationManager (configuration_manager.cpp)

    author: S. Hau
    date: October 20, 2017

**********************************************************************/


namespace Core { namespace Config {


    //----------------------------------------------------------------------
    void ConfigurationManager::init()
    {
        m_engineIni = new ConfigFile( "/config/engine.ini" );
    }

    //----------------------------------------------------------------------
    void ConfigurationManager::shutdown()
    {
        delete m_engineIni;
    }


} } // end namespaces