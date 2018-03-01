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

    Read with same mechanism:
    U32 x = configFile["General"]["ResolutionX"];
    U32 y = configFile["General"]["ResolutionY"];
**********************************************************************/

#include "Common/variant_type.h"

namespace OS { class TextFile; }


namespace Core { namespace Config {

    //**********************************************************************
    // Opens or creates a new configuration file.
    //**********************************************************************
    class ConfigFile
    {
        static const char* DEFAULT_CATEGORY_NAME;

        //**********************************************************************
        // Represents a category in the config-file [CATEGORY]
        //**********************************************************************
        class Category
        {
        public:
            Common::VariantType& operator [] (const char* name) { return m_entries[ SID( name ) ]; }

        private:
            friend class ConfigFile;
            HashMap<StringID, Common::VariantType> m_entries;
        };

        //----------------------------------------------------------------------

    public:
        ConfigFile(const char* vpath);
        ~ConfigFile();

        //----------------------------------------------------------------------
        // Return a category object by name.
        //----------------------------------------------------------------------
        Category& operator [] (const char* category);

        //----------------------------------------------------------------------
        // Flush whole data immediately to disk.
        //----------------------------------------------------------------------
        void flush();

    private:
        OS::TextFile*                   m_configFile = nullptr;
        HashMap<StringID, Category>    m_categories;

        // Read config file from disk
        void _Read();

        ConfigFile(const ConfigFile& other)                 = delete;
        ConfigFile& operator = (const ConfigFile& other)    = delete;
        ConfigFile(ConfigFile&& other)                      = delete;
        ConfigFile& operator = (ConfigFile&& other)         = delete;
    };


} }