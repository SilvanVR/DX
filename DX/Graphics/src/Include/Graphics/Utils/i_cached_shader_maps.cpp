#include "i_material.h"
/**********************************************************************
    class: ICachedShaderMaps

    author: S. Hau
    date: October 26, 2018
**********************************************************************/

namespace Graphics {

    //**********************************************************************
    // Public
    //**********************************************************************

    //----------------------------------------------------------------------
    I32 ICachedShaderMaps::getInt( StringID name ) const
    { 
        if ( m_intMap.find( name ) != m_intMap.end() )
            return m_intMap.at( name );

        _WarnMissingInt( name );
        return 0;
    }

    //----------------------------------------------------------------------
    F32 ICachedShaderMaps::getFloat( StringID name ) const 
    { 
        if ( m_floatMap.find( name ) != m_floatMap.end() )
            return m_floatMap.at( name );

        _WarnMissingFloat( name );
        return 0.0f;
    }

    //----------------------------------------------------------------------
    Math::Vec4 ICachedShaderMaps::getVec4( StringID name ) const
    {
        if ( m_vec4Map.find( name ) != m_vec4Map.end() )
            return m_vec4Map.at( name );

        _WarnMissingVec4( name );
        return Math::Vec4( 0.0f );
    }

    //----------------------------------------------------------------------
    DirectX::XMMATRIX ICachedShaderMaps::getMatrix( StringID name ) const
    {
        if ( m_matrixMap.find( name ) != m_matrixMap.end() )
            return m_matrixMap.at( name );

        _WarnMissingMatrix( name );
        return DirectX::XMMatrixIdentity();
    }

    //----------------------------------------------------------------------
    Color ICachedShaderMaps::getColor( StringID name ) const
    {
        if ( m_vec4Map.find( name ) != m_vec4Map.end() )
        {
            Math::Vec4 colorAsVec = m_vec4Map.at( name );
            return Color( (Byte) (colorAsVec.x * 255.0f), (Byte) (colorAsVec.y * 255.0f), (Byte) (colorAsVec.z * 255.0f), (Byte) (colorAsVec.w * 255.0f) );
        }

        _WarnMissingColor( name );
        return Color::BLACK;
    }

    //----------------------------------------------------------------------
    TexturePtr ICachedShaderMaps::getTexture( StringID name ) const
    {
        if ( m_textureMap.find( name ) != m_textureMap.end() )
            return m_textureMap.at( name );

        _WarnMissingTexture( name );
        return nullptr;
    }

    //----------------------------------------------------------------------
    void ICachedShaderMaps::setInt( StringID name, I32 val )
    { 
        if ( not _HasShaderInt( name ) )
            return;

        m_intMap[ name ] = val;
        _SetInt( name, val ); 
    }

    //----------------------------------------------------------------------
    void ICachedShaderMaps::setFloat( StringID name, F32 val )
    { 
        if ( not _HasShaderFloat( name ) )
            return;

        m_floatMap[ name ] = val;
        _SetFloat( name, val );
    }

    //----------------------------------------------------------------------
    void ICachedShaderMaps::setVec4( StringID name, const Math::Vec4& vec )
    { 
        if ( not _HasShaderVec4( name ) )
            return;

        m_vec4Map[ name ] = vec;
        _SetVec4( name, vec );
    }

    //----------------------------------------------------------------------
    void ICachedShaderMaps::setMatrix( StringID name, const DirectX::XMMATRIX& matrix )
    { 
        if ( not _HasShaderMatrix( name ) )
            return;

        m_matrixMap[ name ] = matrix;
        _SetMatrix( name, matrix ); 
    }

    //----------------------------------------------------------------------
    void ICachedShaderMaps::setColor( StringID name, Color color )
    { 
        if ( not _HasShaderColor( name ) )
            return;

        auto normalized = color.normalized();
        Math::Vec4 colorAsVec( normalized[0], normalized[1], normalized[2], normalized[3] );
        m_vec4Map[ name ] = colorAsVec;
        _SetVec4( name, colorAsVec );
    }

    //----------------------------------------------------------------------
    void ICachedShaderMaps::setTexture( StringID name, const TexturePtr& texture )
    { 
        ASSERT( texture && "It's not allowed to set a null texture in a material" );
        if ( not _HasShaderTexture( name ) )
            return;

        m_textureMap[ name ] = texture;
    }

    //**********************************************************************
    // Protected
    //**********************************************************************

    //----------------------------------------------------------------------
    void ICachedShaderMaps::_ClearAllMaps()
    {
        m_intMap.clear();
        m_floatMap.clear();
        m_vec4Map.clear();
        m_matrixMap.clear();
        m_textureMap.clear();
    }

} // End namespaces