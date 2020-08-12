#pragma once

#include "base.h"
#include "window/window.h"
#include "layers/layer_array.h"
#include "delta_time/delta_time.h"

#include "SDL2/SDL_timer.h"


namespace Bubble
{
	class Application
	{
	protected:
		Scope<Window> m_Window;
		LayerArray m_LayerArray;
		DeltaTime m_DeltaTime;
		
	public:
		Application(Window* window = nullptr);
		virtual ~Application() {}

		void Run();

		void PushLayer(Layer* layer);
		void InsertLayer(int id, Layer* layer);
		void RemoveLayer(int id);
		void SwapLayers(int id_1, int id_2);
	};
}