#include "config_file.h"
/**********************************************************************
    class: ConfigFile (config_file.cpp)

    author: S. Hau
    date: October 18, 2017
**********************************************************************/

#include "Core/locator.h"
#include "Utils/string_utils.h"

namespace Core { namespace Config {

    static VariantType NULL_TYPE;

    //----------------------------------------------------------------------
    const char* ConfigFile::DEFAULT_CATEGORY_NAME = "Default";

    //----------------------------------------------------------------------
    VariantType& ConfigFile::Category::operator [] (const String& name)
    {
        bool existing = (m_entries.count( name ) != 0);

        if (not existing)
        {

            return m_entries[name];
        }

        return m_entries[name];
    }

    //----------------------------------------------------------------------
    ConfigFile::Category& ConfigFile::operator [] ( const String& category )
    {
        bool existing = (m_categories.count( category ) != 0);

        if (existing)
            return m_categories[category];

        // Category does not exist, so create it
        Category newCategory;

        m_categories[category] = newCategory;

        return m_categories[category];
    }

    //----------------------------------------------------------------------
    void ConfigFile::flush()
    {
        return;

        m_configFile.clear();

        for (auto& categoryPair : m_categories)
        {
            const String& categoryName = categoryPair.first;

            if (categoryName == DEFAULT_CATEGORY_NAME)
                continue;

            Category& category = categoryPair.second;

            m_configFile.write( "[%s]\n", categoryName.c_str());
            for (auto& linePair : category.m_entries)
            {
                const String& name = linePair.first;
                m_configFile.write( "%s = ", name.c_str() );

                VariantType& value = linePair.second;
                switch ( value.getType() )
                {
                case EVariantType::I32:     m_configFile << value.get<I32>(); break;
                case EVariantType::U32:     m_configFile << value.get<U32>(); break;
                case EVariantType::I64:     m_configFile << value.get<I64>(); break;
                case EVariantType::U64:     m_configFile << value.get<U64>(); break;
                case EVariantType::F32:     m_configFile << value.get<F32>(); break;
                case EVariantType::F64:     m_configFile << value.get<F64>(); break;
                case EVariantType::String:  m_configFile << "\"" << value.get<const char*>() << "\""; break;
                }
                m_configFile.write( "\n" );
            }
            m_configFile.write( "\n" );
        }

        m_configFile.flush();
    }

    //----------------------------------------------------------------------
    void ConfigFile::_Read()
    {
        String currentCategory = DEFAULT_CATEGORY_NAME;
        m_categories[currentCategory] = Category();

        while ( not m_configFile.eof() )
        {
            String line = m_configFile.readLine();

            // Skip empty lines
            if (line.size() == 0)
                continue;

            // Check if line is a category ("[Category]")
            String categoryName = StringUtils::substringBetween( line, '[', ']' );
            StringUtils::trim( categoryName );
            if (categoryName.size() > 0 )
            {
                // Add new category
                m_categories[categoryName] = Category();
                currentCategory = categoryName;

                continue;
            }

            // Try to read line as a name-value pair (e.g. "life=42")
            auto strings = StringUtils::splitString( line, '=' );
            if (strings.size() == 2)
            {
                const String& valueName = strings[0];
                const String& value     = strings[1];

                // Add value to current category
                m_categories[currentCategory][valueName] = value;

                continue;
            }

            WARN( "ConfigFile: Could not read line: '" + line + "'" );
        }
    }

} }