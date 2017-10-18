#include "config_file.h"
/**********************************************************************
    class: ConfigFile (config_file.cpp)

    author: S. Hau
    date: October 18, 2017
**********************************************************************/

#include "Core/locator.h"

namespace Core { namespace Config {

    static VariantType NULL_TYPE;

    //----------------------------------------------------------------------
    VariantType& ConfigFile::Category::operator [] (const char* name)
    {
        bool existing = (m_entries.count( name ) != 0);

        if (not existing)
        {

            return m_entries[name];
        }

        return m_entries[name];
    }

    //----------------------------------------------------------------------
    ConfigFile::Category& ConfigFile::operator [] ( const char* category )
    {
        bool existing = (m_categories.count( category ) != 0);

        if (existing)
            return m_categories[category];

        // Category does not exist, so create it
        Category newCategory( category );

        m_categories[category] = newCategory;

        return m_categories[category];
    }

    //----------------------------------------------------------------------
    void ConfigFile::flush()
    {
        for (auto& categoryPair : m_categories)
        {
            const char* categoryName = categoryPair.first;
            Category& category = categoryPair.second;

            m_configFile.write( "[%s]\n", categoryName );
            for (auto& linePair : category.m_entries)
            {
                const char* name = linePair.first;
                m_configFile.write( "%s = ", name );

                VariantType& value = linePair.second;
                switch ( value.getType() )
                {
                case EVariantType::I32:     m_configFile << value.get<I32>(); break;
                case EVariantType::U32:     m_configFile << value.get<U32>(); break;
                case EVariantType::I64:     m_configFile << value.get<I64>(); break;
                case EVariantType::U64:     m_configFile << value.get<U64>(); break;
                case EVariantType::F32:     m_configFile << value.get<F32>(); break;
                case EVariantType::F64:     m_configFile << value.get<F64>(); break;
                case EVariantType::String:  m_configFile << value.get<const char*>(); break;
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
        while ( not m_configFile.eof() )
        {
            String line = m_configFile.readLine();

            LOG( line, Color::RED );
        }
    }

} }