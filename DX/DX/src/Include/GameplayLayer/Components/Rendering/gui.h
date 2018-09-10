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
#include "Graphics/forward_declarations.hpp"
#include "Ext/ImGUI/imgui.h"
#include "Events/event.h"

namespace ImGui
{
    void Image(const MaterialPtr& mat, const Math::Vec2& size);
    void Image(const TexturePtr& tex, const Math::Vec2& size);
    void Image(const TexturePtr& texArray, I32 slice, const Math::Vec2& size);
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
        void init() override;
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
        MaterialPtr             m_fontAtlasMaterial;

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
        MaterialPtr m_mat;
        F32         m_scale = 0;
        Math::Vec2  m_size  = { 0, 0 };

    public:
        GUIImage(const TexturePtr& tex, F32 scale = 1.0f);
        GUIImage(const TexturePtr& tex, const Math::Vec2& size);
        GUIImage(const TexturePtr& tex, I32 arraySlice, const Math::Vec2& size);

        void setTexture(const TexturePtr& tex);

        void OnImGUI() override;
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

    //**********************************************************************
    class GUIFPS : public Components::ImGUIRenderComponent
    {
    public:
        void OnImGUI() override;
    };

    //**********************************************************************
    // This is a little helper class in order to store material creates by
    // custom immediate function e.g. ImGUI::Image(tex). The function
    // creates a material and stores it in this class.
    //**********************************************************************
    class ImGUIMaterialCache
    {
    public:
        static ImGUIMaterialCache& Instance()
        {
            static ImGUIMaterialCache instance;
            return instance;
        }

        const MaterialPtr& AddTexture(const TexturePtr& tex);
        const MaterialPtr& AddTexture(const TexturePtr& tex, I32 slice);

    private:
        ImGUIMaterialCache();

        HashMap<Size, MaterialPtr> m_cachedMaterials;
        Events::EventListener m_gameShutdownListener;

        void _OnGameShutdown();
    };


} // End namespaces