#include "AresPCH.h"
#include "JsonObject.h"
#include <fstream>
namespace glm 
{
	void to_json(nlohmann::json& j, const glm::vec2& vector)
	{
		j = nlohmann::json();
		j.push_back(vector.x);
		j.push_back(vector.y);
	}
	void to_json(nlohmann::json& j, const glm::vec3& vector)
	{
		j = nlohmann::json();
		j.push_back(vector.x);
		j.push_back(vector.y);
		j.push_back(vector.z);
	}
	void to_json(nlohmann::json& j, const glm::vec4& vector)
	{
		j = nlohmann::json();
		j.push_back(vector.x);
		j.push_back(vector.y);
		j.push_back(vector.z);
		j.push_back(vector.w);
	}

	void from_json(const nlohmann::json& j, glm::vec2& vector)
	{
		j.at(0).get_to(vector.x);
		j.at(1).get_to(vector.y);
	}
	void from_json(const nlohmann::json& j, glm::vec3& vector)
	{
		j.at(0).get_to(vector.x);
		j.at(1).get_to(vector.y);
		j.at(2).get_to(vector.z);
	}
	void from_json(const nlohmann::json& j, glm::vec4& vector)
	{
		j.at(0).get_to(vector.x);
		j.at(1).get_to(vector.y);
		j.at(2).get_to(vector.z);
		j.at(3).get_to(vector.w);
	}
}
namespace Ares {

	WritableJsonObject::WritableJsonObject(const std::string& name)
		: m_Name(name)
	{
	}

	void WritableJsonObject::Flush(const std::string& filepath)
	{
		std::ofstream out(filepath);
		out << m_BaseObject.dump() << std::endl;
		out.close();
		m_BaseObject.clear();
	}

	ReadableJsonObject::ReadableJsonObject(const std::string& filepath)
		: m_FilePath(filepath)
	{
		if (!filepath.empty())
			Load();
	}

	void ReadableJsonObject::Load()
	{
		std::ifstream stream(m_FilePath);

		if (!stream.is_open())
		{
			ARES_CORE_ERROR("Failed to open file '{0}', no such file exists!", m_FilePath);
			return;
		}

		stream >> m_BaseObject;
		stream.close();

		LoadObjects();
	}

	void ReadableJsonObject::LoadObjects()
	{
		for (auto& obj : m_BaseObject.items())
		{
			ReadableJsonObject childObject;
			childObject.m_FilePath = m_FilePath;
			childObject.m_BaseObject = obj.value();

			if (childObject.m_BaseObject.is_object())
				childObject.LoadObjects();

			m_Objects[obj.key()] = childObject;
		}
	}
}