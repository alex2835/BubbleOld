#pragma once

#include "engine.h"
#include "tools/file_dialog.h"
#include "ui_module.h"
#include "viewport.h"
#include <algorithm>

namespace Bubble
{
	struct ModelExplorer : UIModule
    {
        ThirdPersonCamera mCamera;
		Ref<Model>        mSelectedModel;
		Viewport          mViewport;
		Light             mLight;
        glm::mat4         mTransforms;
	
        inline ModelExplorer()
            : UIModule("Model explorer"),
              mSelectedModel(nullptr),
              mViewport(800, 600),
			  mCamera(0),
              mTransforms(glm::mat4(1.0f))
        {
            mLight  = Light::CreateDirLight();
        }
		
		inline void Draw(UIArgs args, DeltaTime dt)
		{
            ImGui::Begin("Models explorer", &mIsOpen);
            {
                ImVec2 window_size = ImGui::GetContentRegionAvail();
                DrawViewport(window_size, args, dt);
                DrawModelInfo(window_size, args, dt);
                DrawModelsList(window_size, args, dt);
            }
            ImGui::End();
		}

        void OnUpdate(UIArgs args, DeltaTime dt) override
        {
            mViewport.OnUpdate();
        }


	private:
		inline void RenderSelectedModel(Renderer* renderer)
		{
            renderer->SetViewport(mViewport);
            renderer->SetCamera(mCamera);
            renderer->SetLights(&mLight, 1);
            
            renderer->SetClearColor(glm::vec4(0.4f));
            renderer->Clear();
            
            renderer->DrawModel(mSelectedModel, mTransforms, renderer->mStorage.mPhongShader);
		}


        inline void DrawViewport(ImVec2 window_size, UIArgs args, DeltaTime dt)
        {
            uint32_t texture_id   = mViewport.GetColorAttachmentRendererID();
            ImVec2 viewport_size = ImVec2(window_size.x * 0.7f, window_size.y * 0.75f);
            ImGui::Image((void*)texture_id, viewport_size, ImVec2(1, 1), ImVec2(0, 0));
            mViewport.mNewSize = *(glm::vec2*)&viewport_size;

            if (mSelectedModel && ImGui::IsItemVisible())
            {
                RenderSelectedModel(args.mRenderer);
            }

            // Rotate camera
            if (ImGui::IsWindowFocused() && ImGui::IsMouseDragging(0))
            {
                mCamera.ProcessMouseMovementShift(args.mInput->fGetMouseRelX(), -args.mInput->fGetMouseRelY());
                mCamera.Update(dt);
            }
            // Rotate model
            if (ImGui::IsWindowFocused() && ImGui::IsMouseDragging(1))
            {
                if (args.mInput->IsKeyPressed(SDLK_LSHIFT))
                {
                    mTransforms = glm::rotate(mTransforms, 5 * args.mInput->fGetMouseRelY(), glm::vec3(1, 0, 0));
                }
                else {
                    mTransforms = glm::rotate(mTransforms, 5 * args.mInput->fGetMouseRelX(), glm::vec3(0, 1, 0));
                }
            }
            // Change camera radius by mouse wheel scrolling
            if (ImGui::IsWindowHovered())
            {
                if (mSelectedModel)
                {
                    float longest_edge = mSelectedModel->mBoundingBox.getLongestEdge();
                    mCamera.Radius -= args.mInput->GetMouseWheelOffset() * longest_edge / 5;
                    mCamera.Radius  = glm::clamp(mCamera.Radius, longest_edge, mCamera.Far);
                    mCamera.Update(dt);
                }
            }
        }


        inline void DrawModelInfo(ImVec2 window_size, UIArgs args, DeltaTime dt)
        {
            ImGui::SameLine();
            ImGui::BeginChild("Model info", ImVec2(0, window_size.y * 0.75f), true);
            {
                if (mSelectedModel)
                {
                    // Model info
                    int vertices  = 0;
                    int triangles = 0;
                    for (auto& mesh : mSelectedModel->mMeshes)
                    {
                        vertices += mesh.mVertices.Positions.size();
                        triangles += mesh.mIndices.size() / 3;
                    }
                    ImGui::Text("Model info:");
                    ImGui::Text("Vertices: %d",  vertices);
                    ImGui::Text("Triangles: %d", triangles);
                    ImGui::Separator();


                    // Meshes/Material info
                    if (ImGui::TreeNode((void*)0, "Meshes: %d", mSelectedModel->mMeshes.size()))
                    { 
                        int i = 0;
                        for (auto& mesh : mSelectedModel->mMeshes)
                        {
                            i++;
                            if (ImGui::TreeNode((void*)i, mesh.mName.c_str()))
                            {
                                ImGui::Text("Vertices:  %d", mesh.mVertices.Positions.size());
                                ImGui::Text("Triangles: %d", mesh.mIndices.size() / 3);

                                ImVec2 image_size(100, 100);
                                ImGui::Text("Diffuse "); ImGui::SameLine();
                                ImGui::Image((ImTextureID)mesh.mMaterial.mDiffuseMap->mRendererID, image_size);
                                if (ImGui::IsItemClicked())
                                {
                                    try {
                                        std::string path = OpenFileDialog("png,jpg,tga");
                                        mesh.mMaterial.mDiffuseMap = args.mLoader->LoadTexture2D(path);
                                    }
                                    catch (const std::exception& e)
                                    {
                                        LOG_ERROR("Diffuse map can't be opened");
                                    }
                                }

                                ImGui::Text("Specular"); ImGui::SameLine();
                                ImGui::Image((ImTextureID)mesh.mMaterial.mSpecularMap->mRendererID, image_size);
                                if (ImGui::IsItemClicked())
                                {
                                    try {
                                        std::string path = OpenFileDialog("png,jpg,tga");
                                        mesh.mMaterial.mSpecularMap = args.mLoader->LoadTexture2D(path);
                                    }
                                    catch (const std::exception& e)
                                    {
                                        LOG_ERROR("Specular map can't be opened");
                                    }
                                }

                                ImGui::Text("Normal  "); ImGui::SameLine();
                                if (mesh.mMaterial.mNormalMap)
                                {
                                    ImGui::Image((ImTextureID)mesh.mMaterial.mNormalMap->mRendererID, image_size);
                                }
                                else {
                                    const auto& chess_plane = args.mLoader->LoadTexture2D("resources/images/chess_plane.jpg");
                                    ImGui::Image((ImTextureID)chess_plane->GetRendererID(), image_size);
                                }

                                if (ImGui::IsItemClicked())
                                {
                                    try {
                                        std::string path = OpenFileDialog("png,jpg,tga");
                                        mesh.mMaterial.mNormalMap = args.mLoader->LoadTexture2D(path);
                                    }
                                    catch (const std::exception& e)
                                    {
                                        LOG_ERROR("Normal map can't be opened");
                                    }
                                }
                                ImGui::TreePop();
                                ImGui::Separator();
                            }
                        }
                        
                        ImGui::TreePop();
                    }

                }
            }
            ImGui::EndChild();
        }


        inline void DrawModelsList(ImVec2 window_size, UIArgs args, DeltaTime dt)
        {
            ImGui::BeginChild("Models list", ImVec2(window_size.x * 0.5f, window_size.y * 0.24f), true);
            {
                for (const auto& [path, model] : args.mLoader->mLoadedModels)
                {
                    size_t pos = path.find_last_of("/") + 1;
                    std::string name = path.substr(pos);

                    ImGui::Selectable(name.c_str(), mSelectedModel == model);

                    // Switch
                    if (ImGui::IsItemClicked())
                    {
                        mSelectedModel = model;

                        mTransforms = glm::mat4(1.0f);
                        // Start camera params for this model
                        mCamera.Center = mSelectedModel->mBoundingBox.getCenter();
                        mCamera.Radius = mSelectedModel->mBoundingBox.getLongestEdge() * 1.5f;
                        mCamera.Update(dt);
                    }
                }

                if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
                    mSelectedModel = nullptr;
                }
            }
            ImGui::EndChild();


            // ==================== Buttons ====================
            ImGui::SameLine();
            if (ImGui::Button("Load", { 100, window_size.y * 0.05f }))
            {
                try
                {
                    std::string path = OpenFileDialog();
                    args.mLoader->LoadModel(path);
                }
                catch (const std::exception& e)
                {
                    LOG_ERROR(e.what());
                }
            }

        }
    
    };

}