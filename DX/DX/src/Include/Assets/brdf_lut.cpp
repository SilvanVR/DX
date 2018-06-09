#include "brdf_lut.h"
/**********************************************************************
    class: EnvironmentMap (brdf_lut.cpp)

    author: S. Hau
    date: May 29, 2018
**********************************************************************/

#include "Core/locator.h"

namespace Assets {

    //----------------------------------------------------------------------
    BRDFLut::BRDFLut()
    {
        m_brdfLut = RESOURCES.createRenderTexture( 512, 512, Graphics::TextureFormat::RGHalf );
        m_brdfLut->setAnisoLevel( 1 );
        m_brdfLut->setFilter( Graphics::TextureFilter::Bilinear );
        m_brdfLut->setClampMode( Graphics::TextureAddressMode::Clamp );

        Graphics::CommandBuffer cmd;
        cmd.setRenderTarget( m_brdfLut );

        auto shader = ASSETS.getShader( "/shaders/pbr_brdfLut.shader" );
        ASSERT( shader != RESOURCES.getErrorShader() && "Please ensure that the given shader file exists." );
        auto brdfMat = RESOURCES.createMaterial( shader );

        cmd.drawFullscreenQuad( brdfMat );
        Locator::getRenderer().dispatch( cmd );
    }

} // End namespaces