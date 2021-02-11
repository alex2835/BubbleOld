
#include "editor_layer.h"


namespace Bubble
{
	EditorLayer::EditorLayer(Window* window)
		: mSceneCamera(glm::vec3(0.0f, 5.0f, 0.0f)),
		  mViewport(800, 800),
	      mWindow(window),
		  mUILoader(window)
	{}

	void EditorLayer::OnAttach()
	{
		// Temp: load scene
		//OpenProject("../../../../scene_test.json", &mScene, &mUILoader);

 	}


    void EditorLayer::OnDetach()
    {
    }

    void EditorLayer::OnUpdate(DeltaTime dt)
	{
		// ====================== Update editor ======================
        mUILoader.mArgs = { &mSceneCamera, &mViewport, &mScene };
        mUILoader.OnUpdate(dt);

		mSceneCamera.Update(dt);

		// ====================== Set scene data ======================
		//mRenderer.SetViewport(mViewport);
		//mRenderer.SetCamera(mSceneCamera);


		// ====================== Rendering ======================
		mClearScreanOption = true;// |= mUILoader.mWireframeOption;

		//if (mClearScreanOption)
		//{
		//	mRenderer.SetClearColor(glm::vec4(1.0f));
		//	mRenderer.ClearColor();
		//}
		//mRenderer.ClearDepth();
		
		// ====================== Draw scene ====================== 
		//mRenderer.DrawScene(mScene);


		// ====================== Draw editor sruff ======================
		//mRenderer.SetCamera(mSceneCamera);
		//Entity selected_entity = mUILoader.mEntityExplorer->SelectedEntity;

		//// Highlight selected entity
		//if (selected_entity)
		//{
		//	draw_selected_model(selected_entity);
		//}
		
		// Draw bounding boxes
        //if (mUILoader.mBoundingBoxOption)
        //{
        //    draw_scene_boundingbox(mScene);
        //}

		// Temp: Hot keys
		// Quit combination
		//if (Input::IsKeyClick(SDLK_LALT) && Input::IsKeyClick(SDLK_F4))
		//{
		//	mWindow->Close();
		//}
	}

	void EditorLayer::OnEvent(SDL_Event& event)
	{
        mUILoader.OnEvent(event);
	}

}