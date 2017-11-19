#include "variant_type.h"

/**********************************************************************
    class: VariantType (variant_type.cpp)

    author: S. Hau
    date: October 20, 2017
**********************************************************************/

#include "Utils/string_utils.h"
#include "locator.h"

namespace Core {

    //----------------------------------------------------------------------
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

    //----------------------------------------------------------------------
    void VariantType::log() const
    {
        switch ( getType() )
        {
        case EVariantType::BOOL:    LOG( get<bool>() ); break;
        case EVariantType::I32:     LOG( get<I32>() ); break;
        case EVariantType::U32:     LOG( get<U32>() ); break;
        case EVariantType::I64:     LOG( get<I64>() ); break;
        case EVariantType::U64:     LOG( get<U64>() ); break;
        case EVariantType::F32:     LOG( get<F32>() ); break;
        case EVariantType::F64:     LOG( get<F64>() ); break;
        case EVariantType::String:  LOG( get<const char*>() ); break;
        }
    }


} // end namespaces