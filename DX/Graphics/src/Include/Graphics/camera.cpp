#include "camera.h"
/**********************************************************************
    class: Camera (camera.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

namespace Graphics {

    //----------------------------------------------------------------------
    Camera::Camera( F32 fovAngleYInDegree, F32 zNear, F32 zFar )
    {
        m_frameInfo.reset(new FrameInfo);
        m_cameraMode = CameraMode::Perspective;
        setPerspectiveParams( fovAngleYInDegree, zNear, zFar );
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
    {
        m_frameInfo.reset(new FrameInfo);
        m_cameraMode = CameraMode::Orthographic;
        setOrthoParams( left, right, bottom, top, zNear, zFar );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::setOrthoParams( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
    {
        m_ortho.left = left; m_ortho.right = right; m_ortho.bottom = bottom; m_ortho.top = top; m_zNear = zNear; m_zFar = zFar;
    }

    //----------------------------------------------------------------------
    void Camera::setPerspectiveParams( F32 fovAngleYInDegree, F32 zNear, F32 zFar )
    {
        m_fov = fovAngleYInDegree; m_zNear = zNear; m_zFar = zFar;
    }

    //----------------------------------------------------------------------
    F32 Camera::getAspectRatio() const
    {
        ASSERT( m_renderTarget && "Camera has no render target! Please assign one to it." );
        return m_renderTarget->getAspectRatio() * (m_viewport.width / m_viewport.height);
    }

    //----------------------------------------------------------------------
    void Camera::setModelMatrix( const DirectX::XMMATRIX& model )
    { 
        m_model = model;
        m_view = DirectX::XMMatrixInverse( nullptr, m_model );
        _UpdateProjectionMatrix();
        m_viewProjection = m_view * m_projection;
        _UpdateCullingPlanes( m_viewProjection );
    }

    //----------------------------------------------------------------------
    void Camera::setViewMatrix( const DirectX::XMMATRIX& view )
    {
        m_view = view;
        m_model = DirectX::XMMatrixInverse( nullptr, m_view );
        _UpdateProjectionMatrix();
        m_viewProjection = m_view * m_projection;
        _UpdateCullingPlanes( m_viewProjection );
    }

    //----------------------------------------------------------------------
    bool Camera::cull( const Math::AABB& aabb, const DirectX::XMMATRIX& modelMatrix ) const
    {
        // Cull against frustum by transforming vertices from AABB into clip space
        auto mvp = modelMatrix * getViewProjectionMatrix();
        auto aabbCorners = aabb.getCorners();

        std::array<DirectX::XMVECTOR, 8> cornersClipSpace;
        for (I32 i = 0; i < cornersClipSpace.size(); i++)
        {
            auto corner = DirectX::XMVectorSet( aabbCorners[i].x, aabbCorners[i].y, aabbCorners[i].z, 1.0f );
            cornersClipSpace[i] = DirectX::XMVector4Transform( corner, mvp );
        }

        I32 c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0;
        for (I32 i = 0; i < cornersClipSpace.size(); i++)
        {
            Math::Vec4 cornerClipSpace;
            DirectX::XMStoreFloat4( &cornerClipSpace, cornersClipSpace[i] );
            if (cornerClipSpace.x < -cornerClipSpace.w) c1++;
            if (cornerClipSpace.x >  cornerClipSpace.w) c2++;
            if (cornerClipSpace.y < -cornerClipSpace.w) c3++;
            if (cornerClipSpace.y >  cornerClipSpace.w) c4++;
            if (cornerClipSpace.z < -cornerClipSpace.w) c5++;
            if (cornerClipSpace.z >  cornerClipSpace.w) c6++;
        }

        if (c1 == 8 || c2 == 8 || c3 == 8 || c4 == 8 || c5 == 8 || c6 == 8)
            return false;

        return true;
    }

    //----------------------------------------------------------------------
    bool Camera::cull( const Math::Vec3& pos, F32 radius ) const
    {
        for ( U32 i = 0; i < m_planes.size(); i++ )
        {
            if ( (m_planes[i].x * pos.x) + (m_planes[i].y * pos.y) + (m_planes[i].z * pos.z) + m_planes[i].w <= -radius )
                return false;
        }
        return true;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::_UpdateProjectionMatrix()
    {
        switch (m_cameraMode)
        {
        case CameraMode::Perspective:
            m_projection = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians( m_fov ), getAspectRatio(), m_zNear, m_zFar );
            break;
        case CameraMode::Orthographic:
            m_projection = DirectX::XMMatrixOrthographicOffCenterLH( m_ortho.left, m_ortho.right, m_ortho.bottom, m_ortho.top, m_zNear, m_zFar );
            break;
        case CameraMode::Custom: break;
        default:
            ASSERT( false && "Invalid camera mode" );
        }
    }

    //----------------------------------------------------------------------
    void Camera::_UpdateCullingPlanes( const DirectX::XMMATRIX& viewProjection )
    {
        Math::Vec4 row0, row1, row2, row3;
        DirectX::XMStoreFloat4( &row0, viewProjection.r[0] );
        DirectX::XMStoreFloat4( &row1, viewProjection.r[1] );
        DirectX::XMStoreFloat4( &row2, viewProjection.r[2] );
        DirectX::XMStoreFloat4( &row3, viewProjection.r[3] );

        m_planes[LEFT].x = row0.w + row0.x;
        m_planes[LEFT].y = row1.w + row1.x;
        m_planes[LEFT].z = row2.w + row2.x;
        m_planes[LEFT].w = row3.w + row3.x;

        m_planes[RIGHT].x = row0.w - row0.x;
        m_planes[RIGHT].y = row1.w - row1.x;
        m_planes[RIGHT].z = row2.w - row2.x;
        m_planes[RIGHT].w = row3.w - row3.x;

        m_planes[TOP].x = row0.w - row0.y;
        m_planes[TOP].y = row1.w - row1.y;
        m_planes[TOP].z = row2.w - row2.y;
        m_planes[TOP].w = row3.w - row3.y;

        m_planes[BOTTOM].x = row0.w + row0.y;
        m_planes[BOTTOM].y = row1.w + row1.y;
        m_planes[BOTTOM].z = row2.w + row2.y;
        m_planes[BOTTOM].w = row3.w + row3.y;

        m_planes[BACK].x = row0.w + row0.z;
        m_planes[BACK].y = row1.w + row1.z;
        m_planes[BACK].z = row2.w + row2.z;
        m_planes[BACK].w = row3.w + row3.z;

        m_planes[FRONT].x = row0.w - row0.z;
        m_planes[FRONT].y = row1.w - row1.z;
        m_planes[FRONT].z = row2.w - row2.z;
        m_planes[FRONT].w = row3.w - row3.z;

        for ( U32 i = 0; i < m_planes.size(); i++ )
        {
            F32 length = sqrtf( m_planes[i].x * m_planes[i].x + m_planes[i].y * m_planes[i].y + m_planes[i].z * m_planes[i].z );
            m_planes[i] /= length;
        }
    }

}