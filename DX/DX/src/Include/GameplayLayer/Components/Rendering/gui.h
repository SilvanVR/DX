#pragma once
/**********************************************************************
    class: GUI (gui.h)
    
    author: S. Hau
    date: June 21, 2018
**********************************************************************/

#include "../i_component.h"
#include "Graphics/command_buffer.h"
#include "Graphics/i_texture2d.hpp"
#include "Core/Input/listener/input_listener.h"

#include "Ext/ImGUI/imgui.h"

namespace ImGui
{
    void Image(const TexturePtr& tex);
    void Image(const TexturePtr& tex, const Math::Vec2& size);
}

namespace Components {

    class Camera;

    //**********************************************************************
    class GUI : public Components::IComponent, public Core::Input::IMouseListener, public Core::Input::IKeyListener
    {
    public:
        GUI();
        ~GUI();

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void addedToGameObject(GameObject* go) override;
        void preTick(Time::Seconds d) override;
        void lateTick(Time::Seconds d) override;

        //----------------------------------------------------------------------
        // IMouseListener Interface
        //----------------------------------------------------------------------
        void OnMouseMoved(I16 x, I16 y) override;
        void OnMousePressed(MouseKey key, KeyMod mod) override;
        void OnMouseReleased(MouseKey key, KeyMod mod) override;
        void OnMouseWheel(I16 delta) override;

        //----------------------------------------------------------------------
        // IKeyListener Interface
        //----------------------------------------------------------------------
        void OnKeyPressed(Key key, KeyMod mod) override;
        void OnKeyReleased(Key key, KeyMod mod) override;
        void OnChar(char c) override;

    private:
        ImGuiContext*           m_imguiContext = nullptr;
        Texture2DPtr            m_fontAtlas;
        MeshPtr                 m_dynamicMesh;
        ShaderPtr               m_guiShader;
        Graphics::CommandBuffer m_cmd;
        Components::Camera*     m_camera;

        void _UpdateIMGUI(F32 delta);

        NULL_COPY_AND_ASSIGN(GUI)
    };


    //**********************************************************************
    class ImGUIRenderComponent : public Components::IComponent
    {
    public:
        ImGUIRenderComponent() = default;
        virtual ~ImGUIRenderComponent() {}

        virtual void OnImGUI() = 0;

    private:
        NULL_COPY_AND_ASSIGN(ImGUIRenderComponent)
    };

    //**********************************************************************
    class GUIImage : public ImGUIRenderComponent
    {
        TexturePtr m_tex;
        F32 m_scale;

    public:
        GUIImage(TexturePtr tex, F32 scale = 1.0f) : m_tex(tex), m_scale(scale) {}

        void OnImGUI() override
        {
            ImGui::Image(m_tex, { m_tex->getWidth() * m_scale, m_tex->getHeight() * m_scale });
        }
    };

    //**********************************************************************
    class GUIDemoWindow : public ImGUIRenderComponent
    {
    public:
        void OnImGUI() override
        {
            ImGui::ShowDemoWindow();
        }
    };

    //**********************************************************************
    class GUICustom : public Components::ImGUIRenderComponent
    {
        std::function<void()> m_fn;
    public:
        GUICustom(const std::function<void()>& fn) 
            : m_fn(fn) {}

        void OnImGUI() override
        {
            m_fn();
        }
    };


} // End namespaces