#include "D3D11Material.h"
/**********************************************************************
    class: Material (D3D11Material.cpp)

    author: S. Hau
    date: March 12, 2018
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Material::~Material()
    {
        if (m_pConstantBuffer)
            SAFE_DELETE( m_pConstantBuffer );
    }

    //----------------------------------------------------------------------
    void Material::bind()
    {
        if ( not m_materialDataVS.isUpToDate() )
        {
            m_pConstantBuffer->update( m_materialDataVS.data(), m_materialDataVS.size() );
            m_materialDataVS.setIsUpToDate();
        }

        // @TODO: Get binding slot from shader
        if (m_pConstantBuffer)
            m_pConstantBuffer->bind(2);
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************


    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::_ChangedShader()
    {
        if (m_pConstantBuffer)
            SAFE_DELETE( m_pConstantBuffer );

        // @TODO Get Size from Shader
        U32 size = 32;

        m_materialDataVS.resize( size );
        m_pConstantBuffer = new D3D11::ConstantBuffer( size, BufferUsage::LONG_LIVED );
    }

    //----------------------------------------------------------------------
    void Material::_UpdateConstantBuffer()
    {
    }

} } // End namespaces