#include "variant_type.h"

/**********************************************************************
    class: VariantType (variant_type.cpp)

    author: S. Hau
    date: October 20, 2017
**********************************************************************/

#include "Utils/string_utils.h"

namespace Core {


    VariantType::VariantType( const String& string ) 
    {
        StringUtils::IStringStream ss( string );

        // Try to parse the given string as a number
        if ( ss >> m_f64 )
        {
            m_type = EVariantType::F64;
            return;
        }

        // Could not parse the given string as a number, so just use it as a string
        m_type = EVariantType::String;
        m_str = StringID( string.c_str() );
    }



} // end namespaces