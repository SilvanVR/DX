#include "gui.h"
/**********************************************************************
    class: GUI (gui.cpp)
    
    author: S. Hau
    date: June 21, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Core/locator.h"
#include "camera.h"

#define SHADER_GUI_TEX_NAME "tex"

namespace ImGui
{
    void Image(const TexturePtr& tex)
    {
        ImGui::Image((void*)&tex, { (F32)tex->getWidth(), (F32)tex->getHeight() });
    }

    void Image(const TexturePtr& tex, const Math::Vec2& size)
    {
        ImGui::Image((void*)&tex, { size.x, size.y });
    }
}

namespace Components {

    //----------------------------------------------------------------------
    GUI::GUI()  // Listen on the master + gui-channel. Switch to GUI-channel if imgui wants input
        : IMouseListener( (Core::Input::InputChannels)Core::Input::EInputChannel::GUI | 
                          (Core::Input::InputChannels)Core::Input::EInputChannel::Master)
    {
        m_imguiContext = ImGui::CreateContext();
        MOUSE.setChannel( Core::Input::EInputChannel::GUI );
        KEYBOARD.setChannel( Core::Input::EInputChannel::GUI );
    }

    //----------------------------------------------------------------------
    GUI::~GUI()
    {
        ImGui::DestroyContext( m_imguiContext );
    }

    //**********************************************************************
    // PROTECTED
    //**********************************************************************

    //----------------------------------------------------------------------
    void GUI::addedToGameObject( GameObject* go )
    {
        // Retrieve camera which is mandatory
        m_camera = go->getComponent<Components::Camera>();
        ASSERT( m_camera && "This component requires a camera component attached to the gameobject!" );
        m_camera->addCommandBuffer( &m_cmd, Components::CameraEvent::Overlay );

        // IMGUI
        ImGui::SetCurrentContext(m_imguiContext);
        ImGuiIO& io = ImGui::GetIO();

        // Build and load the texture atlas into a texture
        unsigned char* pixels = NULL;
        I32 width, height;
        io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );
        m_fontAtlas = RESOURCES.createTexture2D( width, height, Graphics::TextureFormat::RGBA32, pixels );
        io.Fonts->TexID = &m_fontAtlas;

        // Mesh containing all the vertex/index data
        m_dynamicMesh = RESOURCES.createMesh();
        m_dynamicMesh->setBufferUsage( Graphics::BufferUsage::Frequently );

        // Retrieve GUI shader
        m_guiShader = ASSETS.getShader( "/shaders/gui.shader" );
        m_guiShader->setName( "GUI" );
    }

    //----------------------------------------------------------------------
    void GUI::preTick( Time::Seconds d )
    {
        _UpdateIMGUI((F32)d);
    }

    //----------------------------------------------------------------------
    void GUI::lateTick( Time::Seconds d )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGui::NewFrame();
        for (auto renderComponent : getGameObject()->getComponents<ImGUIRenderComponent>())
            renderComponent->OnImGUI();
        ImGui::EndFrame();
        ImGui::Render();

        m_cmd.reset();
        ImDrawData* draw_data = ImGui::GetDrawData();
        auto proj = DirectX::XMMatrixOrthographicOffCenterLH( draw_data->DisplayPos.x, draw_data->DisplayPos.x + draw_data->DisplaySize.x,
                                                              draw_data->DisplayPos.y + draw_data->DisplaySize.y, draw_data->DisplayPos.y,
                                                              -1, 1 );

        static const StringID PROJ_NAME = SID( "_Proj" );
        m_cmd.setCameraMatrix( PROJ_NAME, proj );

        ArrayList<Math::Vec3>   vertices;
        ArrayList<Math::Vec2>   uvs;
        ArrayList<Color>        colors;

        I32 subMesh = 0;
        U32 baseVertex = 0;
        for (I32 n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList*   cmd_list   = draw_data->CmdLists[n];
            const ImDrawVert*   vtx_buffer = cmd_list->VtxBuffer.Data;
            const ImDrawIdx*    idx_buffer = cmd_list->IdxBuffer.Data;

            for (I32 v = 0; v < cmd_list->VtxBuffer.Size; v++)
            {
                auto& vertex = vtx_buffer[v];
                vertices.push_back( { vertex.pos.x, vertex.pos.y, 0.0f } );
                uvs.push_back( { vertex.uv.x, vertex.uv.y } );
                colors.push_back( Color( vertex.col ) );
            }

            for (I32 cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback)
                {
                    pcmd->UserCallback( cmd_list, pcmd );
                }
                else
                {
                    ArrayList<U32> indices( pcmd->ElemCount );
                    for (U32 i = 0; i < pcmd->ElemCount; i++)
                        indices[i] = idx_buffer[i];

                    ImVec2 pos = draw_data->DisplayPos;
                    Math::Rect r = { (long)(pcmd->ClipRect.x - pos.x), (long)(pcmd->ClipRect.y - pos.y), 
                                     (long)(pcmd->ClipRect.z - pos.x), (long)(pcmd->ClipRect.w - pos.y) };
                    m_cmd.setScissor( r );

                    // Create a new material and set texture
                    auto mat = RESOURCES.createMaterial( m_guiShader );

                    TexturePtr* texture = static_cast<TexturePtr*>( pcmd->TextureId );
                    mat->setTexture( SHADER_GUI_TEX_NAME, *texture );

                    m_dynamicMesh->setIndices( indices, subMesh, Graphics::MeshTopology::Triangles, baseVertex );
                    m_cmd.drawMesh( m_dynamicMesh, mat, DirectX::XMMatrixIdentity(), subMesh );
                    subMesh++;
                }
                idx_buffer += pcmd->ElemCount;
            }
            baseVertex += cmd_list->VtxBuffer.Size;
        }

        if (vertices.size() > 0)
        {
            m_dynamicMesh->setVertices( vertices );
            m_dynamicMesh->setColors( colors );
            m_dynamicMesh->setUVs( uvs );
        }
    }

    //**********************************************************************
    // LISTENER
    //**********************************************************************

    //----------------------------------------------------------------------
    void GUI::OnMouseMoved( I16 x, I16 y )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();

        auto& vp = m_camera->getViewport();

        F32 mouseX = (F32)MOUSE.getMousePos().x - m_camera->getRenderTarget()->getWidth() * vp.topLeftX;
        F32 mouseY = (F32)MOUSE.getMousePos().y - m_camera->getRenderTarget()->getHeight() * vp.topLeftY;

        // @TODO: Transform camera ray to render-target if camera dont render to screen
        io.MousePos.x = mouseX * (1.0f/vp.width);
        io.MousePos.y = mouseY * (1.0f/vp.height);
    }

    //----------------------------------------------------------------------
    void GUI::OnMousePressed( MouseKey key, KeyMod mod )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();
        switch (key)
        {
        case MouseKey::LButton: io.MouseDown[0] = true; break;
        case MouseKey::RButton: io.MouseDown[1] = true; break;
        case MouseKey::MButton: io.MouseDown[2] = true; break;
        }
    }

    //----------------------------------------------------------------------
    void GUI::OnMouseReleased( MouseKey key, KeyMod mod )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();
        switch (key)
        {
        case MouseKey::LButton: io.MouseDown[0] = false; break;
        case MouseKey::RButton: io.MouseDown[1] = false; break;
        case MouseKey::MButton: io.MouseDown[2] = false; break;
        }
    }

    //----------------------------------------------------------------------
    void GUI::OnMouseWheel( I16 delta )
    {
        ImGui::SetCurrentContext( m_imguiContext );
        ImGuiIO& io = ImGui::GetIO();

        io.MouseWheel = delta;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void GUI::_UpdateIMGUI( F32 delta )
    {
        ImGui::SetCurrentContext(m_imguiContext);
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = delta;

        auto& vp = m_camera->getViewport();
        auto& rt = m_camera->getRenderTarget();
        io.DisplaySize.x = (F32)rt->getWidth();
        io.DisplaySize.y = (F32)rt->getHeight();

        using namespace Core::Input;
        bool consoleIsOpen = MOUSE.getChannelMask() & (InputChannels)EInputChannel::Console;
        if ( m_camera->isRenderingToScreen() && not consoleIsOpen )
        {
            if (io.WantCaptureMouse) // Disable master channel
                MOUSE.unsetChannel( EInputChannel::Master );
            else
                MOUSE.setChannel( EInputChannel::Master );

            if (io.WantCaptureKeyboard) // Disable master channel
                KEYBOARD.unsetChannel( EInputChannel::Master );
            else
                KEYBOARD.setChannel( EInputChannel::Master );
        }
    }


} // End namespaces