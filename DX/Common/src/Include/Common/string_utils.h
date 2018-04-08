#pragma once
/**********************************************************************
    class: None (string_utils.h)

    author: S. Hau
    date: October 20, 2017

    Contains a bunch of functions for modifying strings.
**********************************************************************/

namespace StringUtils {

    //----------------------------------------------------------------------
    String toLower(const String& s);
    String toUpper(const String& s);

    //----------------------------------------------------------------------
    // Trim from start (in place)
    //----------------------------------------------------------------------
    void ltrim(String& s);

    //----------------------------------------------------------------------
    // Trim from end (in place)
    //----------------------------------------------------------------------
    void rtrim(String& s);

    //----------------------------------------------------------------------
    // Trim from both ends (in place)
    //----------------------------------------------------------------------
    void trim(String& s);

    //----------------------------------------------------------------------
    // Find a substring enclosed by the two given characters.
    // @Return:
    //   Substring enclosed by "first" and "second" if found one.
    //   Empty string ("") if not.
    //----------------------------------------------------------------------
    String substringBetween(const String& str, const char first, const char second);

    //----------------------------------------------------------------------
    // Split a string by the given delimiter.
    // @Return:
    //   Array of splitted strings.
    //----------------------------------------------------------------------
    std::vector<String> splitString(const String& str, const char delim);


    //**********************************************************************
    // Class for extracting different data from a string
    //**********************************************************************
    class IStringStream
    {
    public:
        IStringStream(const String& str);
        ~IStringStream() {}

        //----------------------------------------------------------------------
        bool    eof()           const { return (m_readPos == String::npos); }
        bool    failed()        const { return m_failed; }
        char    peekNextChar();

        //----------------------------------------------------------------------
        template <typename T>
        IStringStream& operator >> (T& data)
        {
            data = _NextValAs<T>();
            return (*this);
        }

        //----------------------------------------------------------------------
        operator bool() const { return !failed(); }

    private:
        String  m_str;
        Size    m_readPos;
        bool    m_failed;

        //----------------------------------------------------------------------
        String  _NextWord();

        //----------------------------------------------------------------------
        template <typename T>
        typename std::enable_if<std::is_floating_point<T>::value, T>::type _NextValAs()
        {
            String nWord = _NextWord();
            const char* cStr = nWord.c_str();

            F64 valAsFloat;
            m_failed = not _ParseAsFloat( cStr, &valAsFloat );
            if (!m_failed)
                return static_cast<T>( valAsFloat );

            return 0;
        }

        //----------------------------------------------------------------------
        template <typename T>
        typename std::enable_if<std::is_integral<T>::value, T>::type _NextValAs()
        {
            String nWord = _NextWord();
            const char* cStr = nWord.c_str();

            I64 valAsIntegral;
            m_failed = not _ParseAsIntegral( cStr, &valAsIntegral);
            if ( !m_failed )
                return static_cast<T>( valAsIntegral );

            return 0;
        }

        //----------------------------------------------------------------------
        template <typename T>
        typename std::enable_if<!std::is_arithmetic<T>::value, T>::type _NextValAs()
        {
            String nWord = _NextWord();

            m_failed = (nWord.size() == 0);
            if ( !m_failed )
                return std::move( nWord );

            return "";
        }

        //----------------------------------------------------------------------
        bool _ParseAsFloat(const char* str, F64* val);
        bool _ParseAsIntegral(const char* str, I64* val);

        IStringStream(const IStringStream& other)               = delete;
        IStringStream& operator = (const IStringStream& other)  = delete;
        IStringStream(IStringStream&& other)                    = delete;
        IStringStream& operator = (IStringStream&& other)       = delete;
    };

}