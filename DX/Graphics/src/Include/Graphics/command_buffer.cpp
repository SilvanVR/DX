#include "command_buffer.h"
/**********************************************************************
    class: CommandBuffer (command_buffer.cpp)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

#include "Logging/logging.h"
#include "i_material.h"
#include "i_shader.h"

namespace Graphics {

    //----------------------------------------------------------------------
    I32 getRenderQueue( GPUCommandBase* c )
    {
        switch ( c->getType() )
        {
        case GPUCommand::DRAW_MESH:
        {
            GPUC_DrawMesh* drawCmd = reinterpret_cast<GPUC_DrawMesh*>( c );
            return drawCmd->material->getShader()->getRenderQueue();
        }
        case GPUCommand::DRAW_MESH_INSTANCED:
        {
            GPUC_DrawMeshInstanced* drawCmd = reinterpret_cast<GPUC_DrawMeshInstanced*>( c );
            return drawCmd->material->getShader()->getRenderQueue();
        }
        case GPUCommand::DRAW_MESH_SKINNED:
        {
            GPUC_DrawMeshSkinned* drawCmd = reinterpret_cast<GPUC_DrawMeshSkinned*>( c );
            return drawCmd->material->getShader()->getRenderQueue();
        }
        case GPUCommand::DRAW_LIGHT:
            return -8196; // return low enough so it is always before every draw command
        }
        LOG_WARN_RENDERING( "CommandBuffer::getRenderQueue(): Expected draw command but was not! Consult your local programmer to fix this!" );
        return 0;
    }

    //----------------------------------------------------------------------
    CommandBuffer::CommandBuffer()
    {
        m_gpuCommands.reserve( COMMAND_BUFFER_INITIAL_CAPACITY );
        m_storage.reserve( INITIAL_STORAGE_SIZE );
    }

    //----------------------------------------------------------------------
    CommandBuffer::~CommandBuffer()
    {
        for (GPUCommandBase* cmd : m_gpuCommands)
            cmd->~GPUCommandBase();
    }

    //----------------------------------------------------------------------
    CommandBuffer::CommandBuffer( const CommandBuffer& other )
    {
        m_gpuCommands.reserve( COMMAND_BUFFER_INITIAL_CAPACITY );
        m_storage.reserve( INITIAL_STORAGE_SIZE );
        merge( other );
    }

    //----------------------------------------------------------------------
    CommandBuffer::CommandBuffer( CommandBuffer&& other )
    {
        m_storage = std::move( other.m_storage );
        m_gpuCommands = std::move( other.m_gpuCommands );
        other.m_storage.clear();
        other.m_gpuCommands.clear();
    }

    //----------------------------------------------------------------------
    void CommandBuffer::sortCommands()
    {
        std::sort( m_gpuCommands.begin(), m_gpuCommands.end(), [](GPUCommandBase* c1, GPUCommandBase* c2) {
            return c1->getType() < c2->getType();
        } );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::sortDrawCommands( const Math::Vec3& cameraPos )
    {
        // Sort draw commands by renderqueue first
        auto itBeginDraw = m_gpuCommands.begin();
        while ( itBeginDraw != m_gpuCommands.end() && ((*itBeginDraw)->getType() != GPUCommand::DRAW_MESH && 
                                                       (*itBeginDraw)->getType() != GPUCommand::DRAW_MESH_INSTANCED &&
                                                       (*itBeginDraw)->getType() != GPUCommand::DRAW_MESH_SKINNED &&
                                                       (*itBeginDraw)->getType() != GPUCommand::DRAW_LIGHT) )
            itBeginDraw++;

        auto itEndDraw = itBeginDraw;
        while ( itEndDraw != m_gpuCommands.end() && ((*itEndDraw)->getType() == GPUCommand::DRAW_MESH ||
                                                     (*itEndDraw)->getType() == GPUCommand::DRAW_MESH_INSTANCED || 
                                                     (*itEndDraw)->getType() == GPUCommand::DRAW_MESH_SKINNED || 
                                                     (*itEndDraw)->getType() == GPUCommand::DRAW_LIGHT) )
            itEndDraw++;

        std::sort( itBeginDraw, itEndDraw, [] (GPUCommandBase* c1, GPUCommandBase* c2) {
            return getRenderQueue( c1 ) < getRenderQueue( c2 );
        } );

        // All draw commands are now ordered properly e.g. drawmesh by renderqueue
        auto itBeginDrawTransparent = itBeginDraw;
        while (itBeginDrawTransparent != m_gpuCommands.end() && 
              ((*itBeginDrawTransparent)->getType() == GPUCommand::DRAW_MESH ||
               (*itBeginDrawTransparent)->getType() == GPUCommand::DRAW_MESH_INSTANCED ||
               (*itBeginDrawTransparent)->getType() == GPUCommand::DRAW_MESH_SKINNED ||
               (*itBeginDrawTransparent)->getType() == GPUCommand::DRAW_LIGHT) )
        {
            if ( getRenderQueue(*itBeginDrawTransparent) >= (I32)RenderQueue::BackToFrontBoundary)
                break;
            itBeginDrawTransparent++;
        }

        // Sort transparent draw materials by camera distance
        std::sort( itBeginDrawTransparent, itEndDraw, [cameraPos](GPUCommandBase* c1, GPUCommandBase* c2) {
            GPUC_DrawMesh* d1 = reinterpret_cast<GPUC_DrawMesh*>( c1 );
            GPUC_DrawMesh* d2 = reinterpret_cast<GPUC_DrawMesh*>( c2 );
            auto pos = d1->modelMatrix.r[3];
            auto pos2 = d2->modelMatrix.r[3];

            auto camPos = DirectX::XMLoadFloat3( &cameraPos );
            auto distance1 = DirectX::XMVector4LengthSq( DirectX::XMVectorSubtract( camPos, pos ) );
            auto distance2 = DirectX::XMVector4LengthSq( DirectX::XMVectorSubtract( camPos, pos2 ) );

            return DirectX::XMVector4Greater( distance1, distance2 );
        } );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::merge( const CommandBuffer& other )
    {
        if (other.isEmpty())
            return;

        for (GPUCommandBase* cmd : other.m_gpuCommands)
        {
            switch (cmd->getType())
            {
            case GPUCommand::SET_CAMERA:        setCamera( ((GPUC_SetCamera*)cmd)->camera ); break;
            case GPUCommand::END_CAMERA:        endCamera(); break;
            case GPUCommand::SET_SCISSOR:       setScissor( ((GPUC_SetScissor*)cmd)->rect ); break;
            case GPUCommand::SET_CAMERA_MATRIX: setCameraMatrix( ((GPUC_SetCameraMatrix*)cmd)->member, ((GPUC_SetCameraMatrix*)cmd)->matrix ); break;
            case GPUCommand::SET_RENDER_TARGET: setRenderTarget( ((GPUC_SetRenderTarget*)cmd)->target ); break;
            case GPUCommand::DRAW_LIGHT:        drawLight( ((GPUC_DrawLight*)cmd)->light ); break;
            case GPUCommand::DRAW_MESH:
            {
                GPUC_DrawMesh* c = (GPUC_DrawMesh*)cmd;
                drawMesh( c->mesh, c->material, c->modelMatrix, c->subMeshIndex );
                break;
            }
            case GPUCommand::DRAW_MESH_INSTANCED:
            {
                GPUC_DrawMeshInstanced* c = (GPUC_DrawMeshInstanced*)cmd;
                drawMeshInstanced( c->mesh, c->material, c->modelMatrix, c->instanceCount );
                break;
            }
            case GPUCommand::DRAW_MESH_SKINNED:
            {
                GPUC_DrawMeshSkinned* c = (GPUC_DrawMeshSkinned*)cmd;
                drawMeshSkinned( c->mesh, c->material, c->modelMatrix, c->subMeshIndex, c->matrixPalette );
                break;
            }
            case GPUCommand::COPY_TEXTURE:
            {
                GPUC_CopyTexture* c = (GPUC_CopyTexture*)cmd;
                copyTexture( c->srcTex, c->srcElement, c->srcMip, c->dstTex, c->dstElement, c->dstMip );
                break;
            }
            case GPUCommand::RENDER_CUBEMAP:
            {
                GPUC_RenderCubemap* c = (GPUC_RenderCubemap*)cmd;
                renderCubemap( c->cubemap, c->material, c->dstMip );
                break;
            }
            case GPUCommand::DRAW_FULLSCREEN_QUAD: drawFullscreenQuad( ((GPUC_DrawFullscreenQuad*)cmd)->material ); break;
            case GPUCommand::BLIT:
            {
                GPUC_Blit* c = (GPUC_Blit*)cmd;
                blit( c->src, c->dst, c->material );
                break;
            }
            case GPUCommand::BEGIN_TIME_QUERY: beginTimeQuery( ((GPUC_BeginTimeQuery*)cmd)->name ); break;
            case GPUCommand::END_TIME_QUERY:   endTimeQuery( ((GPUC_BeginTimeQuery*)cmd)->name ); break;
            default:
                LOG_ERROR_RENDERING("CommandBuffer(): Unknown command.");
            }
        }
    }

    //----------------------------------------------------------------------
    void CommandBuffer::reset()
    {
        for (GPUCommandBase* cmd : m_gpuCommands)
            cmd->~GPUCommandBase();
        m_gpuCommands.clear();
        m_storage.clear();
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawMesh( const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
    {
        ASSERT( mesh && "Mesh is null, which is not allowed!" );
        ASSERT( material && "Material is null, which is not allowed!" );

        void* dest = _GetStorageDestination<GPUC_DrawMesh>();
        m_gpuCommands.push_back( new (dest) GPUC_DrawMesh( mesh, material, modelMatrix, subMeshIndex ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawMeshInstanced( const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 instanceCount )
    {
        ASSERT( mesh && "Mesh is null, which is not allowed!" );
        ASSERT( material && "Material is null, which is not allowed!" );
        ASSERT( instanceCount > 0 && "Material is null, which is not allowed!" );

        void* dest = _GetStorageDestination<GPUC_DrawMeshInstanced>();
        m_gpuCommands.push_back( new (dest) GPUC_DrawMeshInstanced( mesh, material, modelMatrix, instanceCount ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawMeshSkinned( const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex, const ArrayList<DirectX::XMMATRIX>& matrixPalette )
    {
        ASSERT( mesh && "Mesh is null, which is not allowed!" );
        ASSERT( material && "Material is null, which is not allowed!" );

        void* dest = _GetStorageDestination<GPUC_DrawMeshSkinned>();
        m_gpuCommands.push_back( new (dest) GPUC_DrawMeshSkinned( mesh, material, modelMatrix, subMeshIndex, matrixPalette ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setCamera( const Camera& camera )
    {
        void* dest = _GetStorageDestination<GPUC_SetCamera>();
        m_gpuCommands.push_back( new (dest) GPUC_SetCamera( camera ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::endCamera()
    {
        void* dest = _GetStorageDestination<GPUC_EndCamera>();
        m_gpuCommands.push_back( new (dest) GPUC_EndCamera() );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::copyTexture( const TexturePtr& srcTex, const TexturePtr& dstTex )
    {
        copyTexture( srcTex, 0, 0, dstTex, 0, 0 );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::copyTexture( const TexturePtr& srcTex, I32 srcElement, I32 srcMip, const TexturePtr& dstTex, I32 dstElement, I32 dstMip )
    {
        ASSERT( srcTex->getWidth() == dstTex->getWidth() && srcTex->getHeight() == dstTex->getHeight() && "Textures must be of same size" );

        void* dest = _GetStorageDestination<GPUC_CopyTexture>();
        m_gpuCommands.push_back( new (dest) GPUC_CopyTexture( srcTex, srcElement, srcMip, dstTex, dstElement, dstMip ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawLight( const Light* light )
    {
        void* dest = _GetStorageDestination<GPUC_DrawLight>();
        m_gpuCommands.push_back( new (dest) GPUC_DrawLight( light ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setRenderTarget( const RenderTexturePtr& target )
    {
        void* dest = _GetStorageDestination<GPUC_SetRenderTarget>();
        m_gpuCommands.push_back( new (dest) GPUC_SetRenderTarget( target ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawFullscreenQuad( const MaterialPtr& material )
    {
        void* dest = _GetStorageDestination<GPUC_DrawFullscreenQuad>();
        m_gpuCommands.push_back( new (dest) GPUC_DrawFullscreenQuad( material ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::renderCubemap( const CubemapPtr& cubemap, const MaterialPtr& material, I32 dstMip )
    {
        void* dest = _GetStorageDestination<GPUC_RenderCubemap>();
        m_gpuCommands.push_back( new (dest) GPUC_RenderCubemap( cubemap, material, dstMip ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::blit( const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material )
    {
        void* dest = _GetStorageDestination<GPUC_Blit>();
        m_gpuCommands.push_back( new (dest) GPUC_Blit( src, dst, material ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setScissor( const Math::Rect& rect )
    {
        void* dest = _GetStorageDestination<GPUC_SetScissor>();
        m_gpuCommands.push_back( new (dest) GPUC_SetScissor( rect ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setCameraMatrix( CameraMember member, const DirectX::XMMATRIX& matrix )
    {
        void* dest = _GetStorageDestination<GPUC_SetCameraMatrix>();
        m_gpuCommands.push_back( new (dest) GPUC_SetCameraMatrix( member, matrix ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::beginTimeQuery( StringID name )
    {
        void* dest = _GetStorageDestination<GPUC_BeginTimeQuery>();
        m_gpuCommands.push_back( new (dest) GPUC_BeginTimeQuery( name ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::endTimeQuery( StringID name )
    {
        void* dest = _GetStorageDestination<GPUC_EndTimeQuery>();
        m_gpuCommands.push_back( new (dest) GPUC_EndTimeQuery( name ) );
    }


    //----------------------------------------------------------------------
    inline void* CommandBuffer::_GetStorageDestination( Size size, Size alignment )
    {
        Byte* storage = m_storage.data();

        Size alignedSize = size + alignment - 1;
        bool resized = (m_storage.size() + alignedSize) > m_storage.capacity();
        m_storage.resize( m_storage.size() + alignedSize );

        // Fix pointers if vector did reallocation
        if (resized)
        {
            for (I32 i = 0; i < m_gpuCommands.size(); i++)
            {
                Size diff = ((Byte*)m_gpuCommands[i] - storage);
                m_gpuCommands[i] = (GPUCommandBase*)(m_storage.data() + diff);
                ASSERT((I32)m_gpuCommands[i]->getType() < (I32)GPUCommand::NUM_COMMANDS);
            }
        }

        Size alignedIndex = (m_storage.size() - alignedSize) + (alignment - 1) & ~(alignment - 1);
        return &m_storage[alignedIndex];
    }

} // End namespaces