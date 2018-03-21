#include "D3D11Material.h"
/**********************************************************************
    class: Material (D3D11Material.cpp)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    void Material::bind()
    {
        // Update vertex-buffer
        if ( not m_materialDataVS.isUpToDate() )
        {
            m_pConstantBufferVS->update( m_materialDataVS.data(), m_materialDataVS.size() );
            m_materialDataVS.setIsUpToDate();
        }

        // Update fragment-buffer
        if ( not m_materialDataPS.isUpToDate() )
        {
            m_pConstantBufferPS->update( m_materialDataPS.data(), m_materialDataPS.size() );
            m_materialDataPS.setIsUpToDate();
        }

        auto& bufferInfo = m_shader->getMaterialBufferInfo();

        // @TODO: Get binding slot from shader
        if (m_pConstantBufferVS)
            m_pConstantBufferVS->bindToVertexShader(2);

        // @TODO: binding slot from shader
        if (m_pConstantBufferPS)
            m_pConstantBufferPS->bindToPixelShader(0);
    }

    //----------------------------------------------------------------------
    void Material::_ChangedShader()
    {
        _DestroyConstantBuffers();
        _CreateConstantBuffers();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::setFloat( CString name, F32 val )
    {
        //@Get offset from shader for this name
        //@TODO: Updating depending on shader, e.g. can be vertex or fragment
        auto& binding = m_shader->getMemberInfo( name );

        U32 offset = 16;
        m_materialDataVS.push( offset, val );
    }

    //----------------------------------------------------------------------
    void Material::setVec4( CString name, const Math::Vec4& vec )
    {
        //@Get offset from shader for this name
        U32 offset = 0;
        m_materialDataVS.push( offset, vec );
    }


    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::_UpdateConstantBuffer()
    {
    }

    //----------------------------------------------------------------------
    void Material::_DestroyConstantBuffers()
    {
        if (m_pConstantBufferVS)
            SAFE_DELETE( m_pConstantBufferVS );
        if (m_pConstantBufferPS)
            SAFE_DELETE( m_pConstantBufferPS );
    }

    //----------------------------------------------------------------------
    void Material::_CreateConstantBuffers()
    {
        // @TODO Get Size from Shader
        U32 size = 32;

        m_materialDataVS.resize( size );
        m_pConstantBufferVS = new D3D11::ConstantBuffer( size, BufferUsage::LONG_LIVED );
    }

} } // End namespaces