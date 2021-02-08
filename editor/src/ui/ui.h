#pragma once

#include "ui_module.h"
#include "imgui_control.h"

#include "ui_windows/model_explorer.h"
#include "ui_windows/entity_explorer.h"
#include "ui_windows/main_viewport.h"
#include "ui_windows/skybox_exolorer.h"
#include "ui_windows/scene_panel.h"
#include "ui_options/scene_camera_options.h"

#include <ranges>


namespace Bubble
{
	struct UI
	{
        static Scope<std::vector<Ref<UIModule>>> sModules;

		ImGuiControl mImGuiControl;
        UIArgs mArgs;

		// Rendering options
		bool mWireframeOption = false;
		bool mBoundingBoxOption = false;

        // Default modules
        Ref<MainViewport>   mMainViewport;
        Ref<ModelExplorer>  mModelExploerer;
        Ref<EntityExplorer> mEntityExplorer;
        Ref<SkyboxExplorer> mSkyboxExplorer;
        Ref<ScenePanel>     mScenePanel;

        template <typename T, typename ...Args>
        static Ref<T> AddModule(Args&& ...args);
        static int    AddModule(const Ref<UIModule>& ui_module);
        static void   RemoveModule(int module_id);

		UI(Window* window);
        ~UI();
		void OnUpdate(DeltaTime dt);
		void OnEvent(SDL_Event& event);
		void DrawMenuBar();
	};


    template <typename T, typename ...Args>
    Ref<T> Bubble::UI::AddModule(Args&& ...args)
    {
        Ref<T> temp = CreateRef<T>(std::forward<Args>(args)...);
        sModules->push_back(temp);
        return temp;
    }

}