#pragma once
/**********************************************************************
    class: ConfigFile (config_file.h)

    author: S. Hau
    date: October 18, 2017

    Represents a configuration file on disk, from which data can be
    easily read and written to in a custom format. Example:

    ConfigFile configFile( "/config/engine.ini" );
    configFile["General"]["ResolutionX"] = 1280;
    configFile["General"]["ResolutionY"] = 720;

    << produces (in engine.ini) >>
    [General]
    ResolutionX = 1280
    ResolutionY = 720
**********************************************************************/

#include "Core/OS/FileSystem/file.h"
#include "Core/Misc/variant_type.hpp"

namespace Core { namespace Config {

    //**********************************************************************
    // 
    //**********************************************************************
    class ConfigFile
    {
    public:
        //**********************************************************************
        // 
        //**********************************************************************
        class Category
        {
            friend class ConfigFile;

        public:
            Category(const char* name = "NO NAME") : m_name(name) {}

            VariantType& operator [] (const char* name);

        private:
            const char* m_name;
            std::map<const char*, VariantType> m_entries;
        };
        //----------------------------------------------------------------------

    public:
        ConfigFile(const char* vpath)
            : m_configFile( vpath, OS::EFileMode::READ_WRITE )
        {
            _Read();
        }

        ~ConfigFile(){ flush(); }

        //----------------------------------------------------------------------
        // 
        //----------------------------------------------------------------------
        Category& operator [] (const char* category);

        //----------------------------------------------------------------------
        // 
        //----------------------------------------------------------------------
        void flush();

    private:
        OS::TextFile                    m_configFile;
        std::map<const char*, Category> m_categories;

        void _Read();

        ConfigFile(const ConfigFile& other)                 = delete;
        ConfigFile& operator = (const ConfigFile& other)    = delete;
        ConfigFile(ConfigFile&& other)                      = delete;
        ConfigFile& operator = (ConfigFile&& other)         = delete;
    };





} }