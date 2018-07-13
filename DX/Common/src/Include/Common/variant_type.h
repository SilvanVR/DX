#pragma once
/**********************************************************************
    class: VariantType (variant_type.h)

    author: S. Hau
    date: October 18, 2017

    Represents a class which can be different types. Illegal
    conversions from one type to another asserts at runtime.
    @Considerations:
      - add more datatypes
      - Compile-Time checking of conversions (possible?)
      - Objects
      - Move-Semantics
**********************************************************************/

namespace Common {

    //**********************************************************************
    // All Datatypes supported by the Variant
    //**********************************************************************
    enum class EVariantType
    {
        UNKNOWN,
        BOOL,
        I32, 
        U32, 
        I64, 
        U64, 
        F32, 
        F64,
        String
    };

    //**********************************************************************
    // Encapsulates one type of data.
    //**********************************************************************
    class VariantType 
    {
    public:
        //----------------------------------------------------------------------
        // Extract an appropriate data-type manually from the given string.
        // If the given string represents a number it will be used as it.
        //----------------------------------------------------------------------
        VariantType(const String& string);

        //----------------------------------------------------------------------
        VariantType()                   : m_i64(0), m_type( EVariantType::UNKNOWN ) {}
        VariantType(bool val)           : m_bool( val ), m_type( EVariantType::BOOL ) {}
        VariantType(I32 val)            : m_i32( val ),  m_type( EVariantType::I32 ) {}
        VariantType(U32 val)            : m_u32( val ),  m_type( EVariantType::U32 ) {}
        VariantType(I64 val)            : m_i64( val ),  m_type( EVariantType::I64 ) {}
        VariantType(U64 val)            : m_u64( val ),  m_type( EVariantType::U64 ) {}
        VariantType(F32 val)            : m_f32( val ),  m_type( EVariantType::F32 ) {}
        VariantType(F64 val)            : m_f64( val ),  m_type( EVariantType::F64 ) {}
        VariantType(StringID val)       : m_str( val ),  m_type( EVariantType::String ) {}
        VariantType(const char* val)    : m_str( SID(val) ), m_type(EVariantType::String) {}
        ~VariantType() {}

        //----------------------------------------------------------------------
        EVariantType    getType() const { return m_type; }
        bool            isValid() const { return m_type != EVariantType::UNKNOWN; }

        operator bool() const { return isValid(); }

        //----------------------------------------------------------------------
        template <typename T> T get() const { return _GetVal<T>(); }
        template <typename T> operator T () const { return _GetVal<T>(); }

        // Log to console
        void log() const;

    private:
        EVariantType    m_type;

        union
        {
            bool        m_bool;
            I32         m_i32;
            U32         m_u32;
            I64         m_i64;
            U64         m_u64;
            F32         m_f32;
            F64         m_f64;
            StringID    m_str;
        };

        //----------------------------------------------------------------------
        template <typename T>
        T _GetVal() const
        {
            switch (m_type)
            {
            case EVariantType::BOOL:   return static_cast<T>( m_bool );
            case EVariantType::I32:    return static_cast<T>( m_i32 );
            case EVariantType::U32:    return static_cast<T>( m_u32 );
            case EVariantType::I64:    return static_cast<T>( m_i64 );
            case EVariantType::U64:    return static_cast<T>( m_u64 );
            case EVariantType::F32:    return static_cast<T>( m_f32 );
            case EVariantType::F64:    return static_cast<T>( m_f64 );
            }

            ASSERT( false && "Runtime datatype error. No conversion possible or unknown type." );
            return 0;
        }

        template <>
        const char* _GetVal() const
        {
            switch (m_type)
            {
            case EVariantType::String: return m_str.c_str();
            }

            ASSERT( false && "Runtime datatype error. No conversion possible or unknown type." );
            return 0;
        }

    };


} // end namespaces
