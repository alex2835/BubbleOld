#pragma once

#include "imgui.h"

#include "file_dialog.h"
#include "model/model_loader.h"


namespace Bubble
{
	//void ModelLoaderUI();

	class ModelExplorer
	{
		Ref<Model>* SelectedModel;

	public:
		void Draw(bool* is_open);
	};

}