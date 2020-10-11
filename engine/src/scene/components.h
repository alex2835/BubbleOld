#pragma once

#include "nlohmann/json.hpp"
#include "glm/glm.hpp"

#include "light/light.h"

#include <string>


namespace Bubble
{
	inline nlohmann::json to_json(const glm::vec3& value)
	{
		return { value.r, value.g, value.b };
	}

	inline glm::vec3 from_json(const nlohmann::json& value)
	{
		return glm::vec3(value[0], value[1], value[2]);
	}


	// ================
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}

		nlohmann::json Serialize() const
		{
			nlohmann::json j;
			j["Tag"] = Tag;
			return j;
		}

		void Deserialize(const nlohmann::json& j)
		{
			Tag = j["Tag"];
		}
	};


	// =================
	struct PositionComponent
	{
		glm::vec3 Position;

		PositionComponent() = default;
		PositionComponent(const glm::vec3& position)
			: Position(position)
		{}

		nlohmann::json Serialize() const
		{
			nlohmann::json j;
			j["Position"] = to_json(Position);
			return j;
		}

		void Deserialize(const nlohmann::json& j)
		{
			Position = from_json(j["Position"]);
		}

	};

	struct RotationComponent
	{
		glm::vec3 Rotation;

		RotationComponent() = default;
		RotationComponent(const glm::vec3& rotation)
			: Rotation(rotation)
		{}

		nlohmann::json Serialize() const
		{
			nlohmann::json j;
			j["Rotation"] = to_json(Rotation);
			return j;
		}

		void Deserialize(const nlohmann::json& j)
		{
			Rotation = from_json(j["Rotation"]);
		}

	};

	struct ScaleComponent 
	{ 
		glm::vec3 Scale = glm::vec3( 1.0f );
		
		ScaleComponent() = default;
		ScaleComponent(const glm::vec3 & scale)
			: Scale(scale)
		{}

		nlohmann::json Serialize() const
		{
			nlohmann::json j;
			j["Scale"] = to_json(Scale);
			return j;
		}
		
		void Deserialize(const nlohmann::json& j)
		{
			Scale = from_json(j["Scale"]);
		}
	};


	struct TransformComponent
	{
		glm::mat4 Transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(glm::mat4 transform)
			: Transform(transform) {}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }

		nlohmann::json Serialize() const
		{
			nlohmann::json j;
			std::vector<float> temp;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					temp.push_back(Transform[i][j]);
				}
			}
			j["Transform"] = temp;
			return j;
		}

		void Deserialize(const nlohmann::json& j)
		{
			std::vector<float> temp = j["Transform"];
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					Transform[i][j] = temp[i * 4 + j];
				}
			}

		}

	};



	struct LightComponent
	{
		Light light;

		LightComponent() = default;
		//LightComponent(Light light)
		//	: light(light)
		//{}

		nlohmann::json Serialize() const
		{
			nlohmann::json j;
			j["Light"]["Type"] = light.Type;
			j["Light"]["Brightness"] = light.Brightness;
			j["Light"]["Distance"] = light.Distance;
			j["Light"]["Color"] = to_json(light.Color);

			j["Light"]["Direction"] = to_json(light.Direction);
			j["Light"]["Position"] = to_json(light.Position);

			j["Light"]["Constant"] = light.Constant;
			j["Light"]["Linear"] = light.Linear;
			j["Light"]["Quadratic"] = light.Quadratic;

			j["Light"]["CutOff"] = light.CutOff;
			j["Light"]["OuterCutOff"] = light.OuterCutOff;

			return j;
		}


		void Deserialize(const nlohmann::json& j)
		{
			LOG_INFO(j.dump(1));

			light.Type= j["Light"]["Type"];
			light.Brightness = j["Light"]["Brightness"];
			light.Distance = j["Light"]["Distance"];
			light.Color = from_json(j["Light"]["Color"]);

			light.Direction = from_json(j["Light"]["Direction"]);
			light.Position = from_json(j["Light"]["Position"]);

			light.Constant = j["Light"]["Constant"];
			light.Linear= j["Light"]["Linear"];
			light.Quadratic = j["Light"]["Quadratic"];

			light.CutOff = j["Light"]["CutOff"];
			light.OuterCutOff = j["Light"]["OuterCutOff"];
		}

	};


	// =================
	//class ScriptableEntity;
	//
	//class NativeScriptComponent
	//{
	//public:
	//	ScriptableEntity* Instance = nullptr;
	//
	//	ScriptableEntity* (*InstantiateScript)();
	//	void (*DestroyInstanceScript)(NativeScriptComponent*);
	//
	//	template <typename T>
	//	void Bind()
	//	{
	//		InstantiateScript = []() { return (ScriptableEntity*)new T(); };
	//		DestroyInstanceScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
	//	}
	//
	//
	//};

}