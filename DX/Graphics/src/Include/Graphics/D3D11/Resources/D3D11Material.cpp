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
            m_materialDataVS.pushToGPU();

        // Update fragment-buffer
        if ( not m_materialDataPS.isUpToDate() )
            m_materialDataPS.pushToGPU();


        auto& bufferInfo = m_shader->getMaterialBufferInfo();

        // @TODO: Get binding slot from shader
        auto pConstantBufferVS = m_materialDataVS.getBuffer();
        if (pConstantBufferVS)
            pConstantBufferVS->bindToVertexShader(2);

        // @TODO: binding slot from shader
        auto pConstantBufferPS = m_materialDataPS.getBuffer();
        if (pConstantBufferPS)
            pConstantBufferPS->bindToPixelShader(0);
    }

    //----------------------------------------------------------------------
    void Material::_ChangedShader()
    {
        _CreateConstantBuffers();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::_SetFloat( CString name, F32 val )
    {
        //@Get offset from shader for this name
        //@TODO: Updating depending on shader, e.g. can be vertex or fragment
        auto& binding = m_shader->getMemberInfo( name );

        U32 offset = 16;
        m_materialDataVS.push( offset, &val );
    }

    //----------------------------------------------------------------------
    void Material::_SetVec4( CString name, const Math::Vec4& vec )
    {
        //@Get offset from shader for this name
        U32 offset = 0;
        m_materialDataVS.push( offset, &vec );
    }


    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::_CreateConstantBuffers()
    {
        // @TODO Get Size from Shader
        U32 size = 32;
        m_materialDataVS.resize( size );
    }

    //----------------------------------------------------------------------
    void Material::_UpdateConstantBuffer()
    {
    }


} } // End namespaces