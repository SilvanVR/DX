#include "brdf_lut.h"
/**********************************************************************
    class: EnvironmentMap (brdf_lut.cpp)

    author: S. Hau
    date: May 29, 2018
**********************************************************************/

#include "Core/locator.h"

namespace Assets {

    #define SHADER_NAME_ENV_MAP         "environmentMap"
    #define SHADER_NAME_SPEC_SAMPLES    "numSamples"
    #define SHADER_NAME_RESOLUTION      "resolution"
    #define SHADER_NAME_ROUGHNESS       "roughness"

    //----------------------------------------------------------------------
    BRDFLut::BRDFLut()
    {
        m_brdfLut = RESOURCES.createRenderTexture();
        m_brdfLut->create( 512, 512, 0, Graphics::TextureFormat::RGHalf );
        m_brdfLut->setAnisoLevel( 1 );
        m_brdfLut->setFilter( Graphics::TextureFilter::Bilinear );
        m_brdfLut->setClampMode( Graphics::TextureAddressMode::Clamp );

        Graphics::CommandBuffer cmd;
        cmd.setRenderTarget( m_brdfLut );
        cmd.drawFullscreenQuad( ASSETS.getMaterial( "/materials/pbr_brdfLut.material" ) );
        Locator::getRenderer().dispatch( cmd );
    }

} // End namespaces