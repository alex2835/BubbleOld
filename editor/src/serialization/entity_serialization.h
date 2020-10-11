#pragma once

#include "entt/entt.hpp"
#include "scene/scene.h"
#include "scene/components.h"
#include "nlohmann/json.hpp"


namespace Bubble
{
	nlohmann::json SerializeScene(const entt::registry& registry);

	void DeserializeScene(entt::registry& scene, const nlohmann::json& json);

}