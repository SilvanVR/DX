#include "subsystem_manager.h"

/**********************************************************************
    class: SubSystemManager (subsystem_manager.cpp)

    author: S. Hau
    date: October 11, 2017
**********************************************************************/

#include "locator.h"

#include "Logging/console_logger.h"


namespace Core
{

    //----------------------------------------------------------------------
    static bool INITIALIZED = false;
    SubSystemManager::SubSystemManager()
    {
        ASSERT( INITIALIZED == false && "Only one instance of this class is allowed!" );
        INITIALIZED = true;
    }


    //----------------------------------------------------------------------
    void SubSystemManager::init()
    {
        // Initialize Logger first before anything
        m_logger = initializeSubSystem( new Logging::ConsoleLogger() );

        LOG( "Initialize Sub-Systems..." );

        // Create all Sub-Systems here


        // Initialize every Sub-System here


        // Provide them to the locator

    }




    //----------------------------------------------------------------------
    void SubSystemManager::shutdown()
    {
        LOG( "Shutting down Sub-Systems..." );
        // Shutdown every Sub-System here


        // Delete every Sub-System here


        // Delete Logger at last
        LOG( "Shutdown Logger..." );
        shutdownSubSystem( m_logger );
        Locator::provide( (Logging::ILogger*) nullptr );
    }


}