
#include "editor_layer.h"


namespace Bubble
{
	EditorLayer::EditorLayer()
		: SceneCamera(glm::vec3(0.0f, 5.0f, 0.0f))
	{}

	void EditorLayer::OnAttach()
	{
        ImGuiControll.OnAttach();

        // Temp: test viewport
		//ViewportArray.Push(Viewport(800, 800));
		MainViewport = Viewport(800, 800);

		// ============ Model entities =============
	
		OpenProject("../../../../scene_test.json", &ActiveScene);

		// Temp: skybox
		m_Skybox = CreateRef<Skybox>("resources/skybox/skybox1.jpg");
		m_ShaderSkybox = Shader::Open("resources/shaders/skybox.glsl");

		PhongShader = Shader::Open("resources/shaders/phong.glsl");
		ShaderSelected = Shader::Open("resources/shaders/solid_color.glsl");

		// Temp: Try to simplify mesh
	}


	void EditorLayer::OnDetach()
	{
        ImGuiControll.OnDetach();
	}
	

	void EditorLayer::OnUpdate(DeltaTime dt)
	{
		// Set args for UI
		UserInterface.Args = { &SceneCamera, &ActiveScene };

		// ImGui Scope
		ImGuiControll.Begin();

		// MenuBar
		ImGuiControll.BeginMenuBar();
		DrawMenuBar();
		ImGuiControll.EndMenuBar();

		// User Interface
		UserInterface.Draw();

		// Temp: Veiwport resize

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		{
			ImVec2 imgui_viewport_size = ImGui::GetContentRegionAvail();
			glm::vec2 viewport_size = MainViewport.Size();

			if (viewport_size != *(glm::vec2*) & imgui_viewport_size)
			{
				MainViewport.Resize({ imgui_viewport_size.x, imgui_viewport_size.y });
			}
			uint32_t textureId = MainViewport.GetFramebuffer().GetColorAttachmentRendererID();
			ImGui::Image((void*)textureId, ImVec2{ (float)MainViewport.Size().x, (float)MainViewport.Size().y }, ImVec2(1, 1), ImVec2(0, 0));
		}
		ImGui::End();
		ImGui::PopStyleVar();

		
		ImGui::Begin("Info");
		{
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();

		// DemoWindow
		ImGui::ShowDemoWindow();

        ImGuiControll.End();


		// Scene update scripts
		ActiveScene.OnUpdate(dt);

		// ActiveScene camera update
		SceneCamera.OnUpdate(dt);

		Renderer::SetViewport(MainViewport.GetFramebuffer());
		Renderer::ClearDepth();

		// Temp : Apply lights to shader
		int light_index = 0;
		ActiveScene.m_Registry.view<LightComponent>().each(
			[&] (auto entity, LightComponent& lc)
			{
				lc.light.SetDistance();
				Light::ApplyLight(lc.light, PhongShader, light_index++);
			}
		);
		PhongShader->SetUni1i("nLights", light_index);


		// Temp: Draw scene
		glm::ivec2 window_size = MainViewport.Size();
		glm::mat4 projection = SceneCamera.GetPprojectionMat(window_size.x, window_size.y);
		glm::mat4 view = SceneCamera.GetLookatMat();
		
		PhongShader->SetUniMat4("u_View", view);
		PhongShader->SetUniMat4("u_Projection", projection);

		auto scene_view = ActiveScene.GetView<ModelComponent, TransformComponent>();

		for (auto entity : scene_view)
		{
			auto [mesh, model] = scene_view.get<ModelComponent, TransformComponent>(entity);
			PhongShader->SetUniMat4("u_Model", model);
			Renderer::DrawModel(mesh, PhongShader, UserInterface.DrawTypeOption);

			// Hightlight selected model
			if (UserInterface.SceneExplorerPanel.SelectedEntity == entity)
			{
				glDisable(GL_DEPTH_TEST);
				ShaderSelected->SetUni4f("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 0.1f));
				ShaderSelected->SetUniMat4("u_Transforms", projection * view * (glm::mat4)model);
				Renderer::DrawModelA(mesh, ShaderSelected, DrawType::TRIANGLES);
				glEnable(GL_DEPTH_TEST);
			}
		}

		// Render skybox
		m_ShaderSkybox->SetUniMat4("u_Projection", projection);
		view = glm::rotate(view, glm::radians(Application::GetTime() * 0.5f), glm::vec3(0, 1, 0));
		m_ShaderSkybox->SetUniMat4("u_View", glm::mat3(view));
		m_ShaderSkybox->SetUni1f("u_Brightness", 1.0f);
		Renderer::DrawSkybox(m_Skybox, m_ShaderSkybox);

	}

	void EditorLayer::OnEvent(SDL_Event& event)
	{
        ImGuiControll.OnEvent(event);

		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
			Application::GetWindow()->Close();
		}
	}

	void EditorLayer::DrawMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			// ====== Editor Menu ======
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open"))
				{
					try
					{
						std::string path = OpenFileDialog("json");
						OpenProject(path, &ActiveScene);
					}
					catch (const std::exception& e)
					{
						LOG_ERROR(e.what());
					}
				}

				if (ImGui::MenuItem("Save"))
				{
					SaveProject("../../../../scene_test.json", &ActiveScene);
				}

				ImGui::EndMenu();
			}

			// ====== UI Menu =======
			UserInterface.DrawMenu();

			ImGui::EndMenuBar();
		}
	}

}