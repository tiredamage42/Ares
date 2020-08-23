#include "AresPCH.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include "yaml-cpp/yaml.h"

#include "Ares/Math/Math.h"
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/quaternion.hpp>
//#include <glm/gtx/matrix_decompose.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

// TODO: save parents / children
namespace YAML {

	template<>
	struct convert<Ares::Vector2>
	{
		static Node encode(const Ares::Vector2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, Ares::Vector2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Ares::Vector3>
	{
		static Node encode(const Ares::Vector3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, Ares::Vector3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Ares::Vector4>
	{
		static Node encode(const Ares::Vector4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, Ares::Vector4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Ares::Quaternion>
	{
		static Node encode(const Ares::Quaternion& rhs)
		{
			Node node;
			node.push_back(rhs.w);
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, Ares::Quaternion& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.w = node[0].as<float>();
			rhs.x = node[1].as<float>();
			rhs.y = node[2].as<float>();
			rhs.z = node[3].as<float>();
			return true;
		}
	};
}

namespace Ares {

	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}


	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Quaternion& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.w << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	/*static std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4& transform)
	{
		glm::vec3 scale, translation, skew;
		glm::vec4 perspective;
		glm::quat orientation;
		glm::decompose(transform, scale, orientation, translation, skew, perspective);

		return { translation, orientation, scale };
	}*/

	typedef YAML::Emitter SerializedScene;

	static void StartObject(SerializedScene& serializedScene, const std::string& key)
	{
		serializedScene << YAML::Key << key;
		serializedScene << YAML::BeginMap; // TagComponent
	}
	static void EndObject(SerializedScene& serializedScene)
	{
		serializedScene << YAML::EndMap;
	}

	template <typename T>
	static void SetKeyValue(SerializedScene& serializedScene, const std::string& key, const T& value)
	{
		serializedScene << YAML::Key << key << YAML::Value << value;
	}


	static void SerializeEntity(SerializedScene& out, Entity entity)
	{
		EntityComponent* c = entity.GetComponent<EntityComponent>();
		
		out << YAML::BeginMap; // Entity

		SetKeyValue(out, "Entity", c->ID);
		SetKeyValue(out, "Tag", c->Tag);
		SetKeyValue(out, "Name", c->Name);

		
		if (entity.HasComponent<TransformComponent>())
		{
			StartObject(out, "TransformComponent");

			auto& transform = entity.GetComponent<TransformComponent>()->LocalTransform;
			auto [pos, rot, scale] = Math::GetTransformDecomposition(transform);
			SetKeyValue(out, "Position", pos);
			SetKeyValue(out, "Rotation", rot);
			SetKeyValue(out, "Scale", scale);
			EndObject(out);
		}

		
		if (entity.HasComponent<MeshRendererComponent>())
		{
			StartObject(out, "MeshComponent");
			auto mesh = entity.GetComponent<MeshRendererComponent>()->Mesh;
			SetKeyValue(out, "AssetPath", mesh->GetFilePath());
			EndObject(out);
		}

		if (entity.HasComponent<CameraComponent>())
		{
			StartObject(out, "CameraComponent");
			auto* cameraComponent = entity.GetComponent<CameraComponent>();
			SetKeyValue(out, "Camera", "some camera data...");
			SetKeyValue(out, "Primary", cameraComponent->Primary);
			EndObject(out);
		}

		//if (entity.HasComponent<SpriteRendererComponent>())
		//{
		//	out << YAML::Key << "SpriteRendererComponent";
		//	out << YAML::BeginMap; // SpriteRendererComponent

		//	auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
		//	out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
		//	if (spriteRendererComponent.Texture)
		//		out << YAML::Key << "TextureAssetPath" << YAML::Value << "path/to/asset";
		//	out << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent.TilingFactor;

		//	out << YAML::EndMap; // SpriteRendererComponent
		//}

		out << YAML::EndMap; // Entity
	}

	static void SerializeEnvironment(SerializedScene& out, const Ref<Scene>& scene)
	{
		out << YAML::Key << "Environment";
		out << YAML::Value;
		out << YAML::BeginMap; // Environment
		SetKeyValue(out, "AssetPath", scene->GetEnvironment().FilePath);

		const auto& light = scene->GetLight();
		out << YAML::Key << "Light" << YAML::Value;
		out << YAML::BeginMap; // Light

		SetKeyValue(out, "Direction", light.Direction);
		SetKeyValue(out, "Radiance", light.Radiance);
		SetKeyValue(out, "Multiplier", light.Multiplier);

		out << YAML::EndMap; // Light
		out << YAML::EndMap; // Environment
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		SerializedScene  out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::Value << "Scene Name";
		SerializeEnvironment(out, m_Scene);
		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };

				if (!entity)
					return;

				SerializeEntity(out, entity);

			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		ARES_CORE_INFO("Deserializing scene '{0}'", sceneName);

		auto environment = data["Environment"];
		if (environment)
		{
			std::string envPath = environment["AssetPath"].as<std::string>();
			m_Scene->SetEnvironment(Environment::Load(envPath));

			auto lightNode = environment["Light"];
			if (lightNode)
			{
				auto& light = m_Scene->GetLight();
				light.Direction = lightNode["Direction"].as<glm::vec3>();
				light.Radiance = lightNode["Radiance"].as<glm::vec3>();
				light.Multiplier = lightNode["Multiplier"].as<float>();
			}
		}

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();
				std::string name = entity["Name"].as<std::string>();

				/*
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();
				*/

				ARES_CORE_INFO("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& transform = deserializedEntity.GetComponent<TransformComponent>()->LocalTransform;
					glm::vec3 translation = transformComponent["Position"].as<glm::vec3>();
					glm::quat rotation = transformComponent["Rotation"].as<glm::quat>();
					glm::vec3 scale = transformComponent["Scale"].as<glm::vec3>();

					transform = glm::translate(glm::mat4(1.0f), translation) *
						glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);

					ARES_CORE_LOG("  Entity Transform:");
					ARES_CORE_LOG("    Translation: {0}, {1}, {2}", translation.x, translation.y, translation.z);
					ARES_CORE_LOG("    Rotation: {0}, {1}, {2}, {3}", rotation.w, rotation.x, rotation.y, rotation.z);
					ARES_CORE_LOG("    Scale: {0}, {1}, {2}", scale.x, scale.y, scale.z);
				}


				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					std::string meshPath = meshComponent["AssetPath"].as<std::string>();
					// TEMP (because script creates mesh component...)
					if (!deserializedEntity.HasComponent<MeshRendererComponent>())
					{
						MeshRendererComponent* mrComponent = deserializedEntity.AddComponent<MeshRendererComponent>();
						std::vector<Ref<Material>> materials;
						mrComponent->Mesh = CreateRef<Mesh>(meshPath, materials);
						mrComponent->Materials = materials;

					}

					ARES_CORE_INFO("  Mesh Asset Path: {0}", meshPath);
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto* component = deserializedEntity.AddComponent<CameraComponent>();
					component->Camera = SceneCamera();
					component->Primary = cameraComponent["Primary"].as<bool>();

					ARES_CORE_INFO("  Primary Camera: {0}", component->Primary);
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto* component = deserializedEntity.AddComponent<SpriteRendererComponent>();
					component->Color = spriteRendererComponent["Color"].as<glm::vec4>();
					//component.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();

					ARES_CORE_INFO("  SpriteRendererComponent present.");
				}
			}
		}
		return true;
	}

}