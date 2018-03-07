#include "c_renderer.h"
/**********************************************************************
    class: CRenderer (c_renderer.cpp)

    author: S. Hau
    date: March 6, 2018
**********************************************************************/

namespace Components {

    //----------------------------------------------------------------------
    ArrayList<CRenderer*> CRenderer::s_renderer;

    //----------------------------------------------------------------------
    CRenderer::CRenderer()
    {
        s_renderer.push_back( this );
    }

    //----------------------------------------------------------------------
    CRenderer::~CRenderer()
    {
        s_renderer.erase( std::remove( s_renderer.begin(), s_renderer.end(), this ) );
    }

}