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
#include "Core/Misc/variant_type.h"

namespace Core { namespace Config {

    //**********************************************************************
    // 
    //**********************************************************************
    class ConfigFile
    {
        static const char* DEFAULT_CATEGORY_NAME;

    public:
        //**********************************************************************
        // 
        //**********************************************************************
        class Category
        {
            friend class ConfigFile;

        public:
            Category() {}

            VariantType& operator [] (const String& name);

        private:
            std::map<String, VariantType> m_entries;
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
        Category& operator [] (const String& category);

        //----------------------------------------------------------------------
        // 
        //----------------------------------------------------------------------
        void flush();

    private:
        OS::TextFile                    m_configFile; // AS POINTER?
        std::map<String, Category>      m_categories;

        void _Read();

        ConfigFile(const ConfigFile& other)                 = delete;
        ConfigFile& operator = (const ConfigFile& other)    = delete;
        ConfigFile(ConfigFile&& other)                      = delete;
        ConfigFile& operator = (ConfigFile&& other)         = delete;
    };





} }