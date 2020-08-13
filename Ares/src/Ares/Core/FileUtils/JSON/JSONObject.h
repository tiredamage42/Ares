#pragma once
#include <json.hpp>
#include <glm\glm.hpp>

/*
	===========================================================================
	READING JSON: =============================================================
	===========================================================================
	{
		"id": "Read JSON from file",
		"numeric_array": [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
		"string_array": [ "A", "B", "C", "D", "E", "F", "G" ],
		"object_array": [
			{
				"var1": 3.141,
				"var2": "Lorem ipsum"
			},
			{
				"var1": 1.413,
				"var2": "muspi merol"
			}
		],
		"vector_array_objects": [
			{
				"vec3": [ 0.2, 0.3, 0.8 ],
				"vec4": [ 0.3, 0.4, 0.9, 1.0 ]
			},
			{
				"vec2": [ 0.5, 0.1 ],
				"vec3": [ 0.2, 1.0, 5.0 ]
			}
		]
	}
        

	// Load the entirety of the json file into a "parent" object
	ReadableJsonObject parentObject("path-to-json-file.json");

	// The api considers any element as a "child", each child
	// can have multiple children of their own, or they contain a value
	std::string id = parentObject.GetChildObject("id").GetValue<std::string>();
	ARES_LOG("Id: {0}", id);

	// You can access the entire array as is, and retrieve it into a vector or array
	// Get the entire array
	std::vector<int> numeric_array = parentObject.GetChildObject("numeric_array").GetValue<std::vector<int>>();
	for (size_t i = 0; i < numeric_array.size(); i++)
	{
		ARES_LOG("Index: {0}, Value: {1}", i, numeric_array[i]);
	}

	// You can ofcourse retrieve ANY type of array
	// Get the entire array
	std::vector<std::string> string_array = parentObject.GetChildObject("string_array").GetValue<std::vector<std::string>>();
	for (size_t i = 0; i < string_array.size(); i++)
	{
		ARES_LOG("Index: {0}, Value: {1}", i, string_array[i]);
	}

	// You can easily retrieve any of the children from an object
	ReadableJsonObject object_array = parentObject.GetChildObject("object_array");

	// Get the "var1" value, from the first object in the "object_array" array.
	float var1 = object_array.GetArrayValue<float>(0, "var1");
	ARES_LOG("Object 1, var1: {0}", var1);

	// Get the "var2" value, from the second object in the "object_array" array.
	std::string var2 = object_array.GetArrayValue<std::string>(1, "var2");
	ARES_LOG("Object 2, var2: {0}", var2);

	ReadableJsonObject vector_array_objects = parentObject.GetChildObject("vector_array_objects");

	// You can retrieve certain sized arrays as glm vectors
	glm::vec2 vector2 = vector_array_objects.GetArrayValue<glm::vec2>(1, "vec2");
	glm::vec3 vector3 = vector_array_objects.GetArrayValue<glm::vec3>(0, "vec3");
	glm::vec4 vector4 = vector_array_objects.GetArrayValue<glm::vec4>(0, "vec4");

	ARES_LOG("vector2: x({0}), y({1})", vector2.x, vector2.y);
	ARES_LOG("vector3: x({0}), y({1}), z({2})", vector3.x, vector3.y, vector3.z);
	ARES_LOG("vector4: x({0}), y({1}), z({2}), w({3})", vector4.x, vector4.y, vector4.z, vector4.w);


	===========================================================================
	WRITING JSON: =============================================================
	===========================================================================
	
	// When you call one of the Write methods, it will only add the value to a buffer,
	// it won't write the data to file immediately.

	// Creates a new, empty "parent" json object
	WritableJsonObject writableObject;

	// You can write basic elements using the WriteValue method
	writableObject.WriteValue<std::string>("id", "Write JSON to file");

	// You can create and write to an array by calling WriteArrayValue.
	// The first parameter is the name of the array, if it doesn't exist, it will be created.
	// The second parameter is the value that you want to write to that array (this can be anything)
	for (size_t i = 0; i <= 10; i++)
	{
		writableObject.WriteArrayValue<int>("numeric_array", i); // Just write the number itself
	}

	std::vector<const char*> letters_to_write = { "A", "B", "C", "D", "E", "F", "G" };
	for (size_t i = 0; i < letters_to_write.size(); i++)
	{
		writableObject.WriteArrayValue<const char*>("string_array", letters_to_write[i]);
	}

	// You can create arrays of objects, these are some dummy objects
	WritableJsonObject object1;
	object1.WriteValue<float>("var1", 3.141);
	object1.WriteValue<std::string>("var2", "Lorem ipsum");

	WritableJsonObject object2;
	object2.WriteValue<float>("var1", 1.413);
	object2.WriteValue<std::string>("var2", "muspi merol");

	// In order to write an object to an array, you need to call WriteArrayObject
	// The first parameter is the name of the array, if it doesn't exist, it will be created.
	// The second parameter is the object that you want to write to that array
	writableObject.WriteArrayObject("object_array", object1);
	writableObject.WriteArrayObject("object_array", object2);

	// You can write glm types to json as well
	WritableJsonObject vector_object1;
	vector_object1.WriteValue<glm::vec3>("vec3", { 0.2f, 0.3f, 0.8f });
	vector_object1.WriteValue<glm::vec4>("vec4", { 0.3f, 0.4f, 0.9f, 1.0f });

	WritableJsonObject vector_object2;
	vector_object2.WriteValue<glm::vec2>("vec2", { 0.5f, 0.1f });
	vector_object2.WriteValue<glm::vec3>("vec3", { 0.2f, 1.0f, 5.0f });

	// Nothing new here
	writableObject.WriteArrayObject("vector_array_objects", vector_object1);
	writableObject.WriteArrayObject("vector_array_objects", vector_object2);

	// In order to actually write the buffer to a file, you need to call the Flush method
	// with the file path as the argument
	writableObject.Flush("Assets/WriteJSON.json");
*/


//////////////////////////////////////////////////////////
//			JSON CONVERSION FUNCTIONS					//
//	These functions tells nlohmanns json library		//
//	how to convert between custom C++ types and JSON	//
//////////////////////////////////////////////////////////

namespace glm
{
	void to_json(nlohmann::json& j, const glm::vec2& vector);
	void to_json(nlohmann::json& j, const glm::vec3& vector);
	void to_json(nlohmann::json& j, const glm::vec4& vector);
	void from_json(const nlohmann::json& j, glm::vec2& vector);
	void from_json(const nlohmann::json& j, glm::vec3& vector);
	void from_json(const nlohmann::json& j, glm::vec4& vector);
}

namespace Ares 
{
	class WritableJsonObject
	{
	public:
		WritableJsonObject(const std::string& name = "");
		~WritableJsonObject() = default;

		void Flush(const std::string& filepath);

		template<typename T>
		void WriteValue(const std::string& name, const T& value)
		{
			m_BaseObject[name] = value;
		}
		template<typename T>
		void WriteArrayValue(const std::string& name, const T& value)
		{
			m_BaseObject[name][m_BaseObject[name].size()] = value;
		}
		void WriteArrayObject(const std::string& name, const WritableJsonObject& value)
		{
			m_BaseObject[name][m_BaseObject[name].size()] = value.m_BaseObject;
		}
		void WriteObject(const WritableJsonObject& object)
		{
			m_BaseObject[object.m_Name] = object.m_BaseObject;
		}

	private:
		std::string m_Name;
		nlohmann::json m_BaseObject;
	};

	class ReadableJsonObject
	{
	public:
		ReadableJsonObject(const std::string& filepath = "");
		~ReadableJsonObject() = default;

		template<typename T>
		T GetValue()
		{
			ARES_CORE_ASSERT(!m_BaseObject.is_null(), "Invalid object! Can't retrive value!");
			return m_BaseObject.get<T>();
		}

		template<typename T>
		T GetArrayValue(unsigned int index, const std::string& name)
		{
			std::vector<nlohmann::json>& arr = GetValue<std::vector<nlohmann::json>>();
			ARES_CORE_ASSERT(index < arr.size(), "");// "Index out of bounds {1}, {2}", index, name);
			return arr[index][name].get<T>();
		}

		ReadableJsonObject& GetChildObject(const std::string name)
		{
			ARES_CORE_ASSERT(m_Objects.find(name) != m_Objects.end(), "");// "No child with the name {1}", name);
			return m_Objects[name];
		}

	private:
		void Load();
		void LoadObjects();

		std::string m_FilePath;
		nlohmann::json m_BaseObject;
		std::unordered_map<std::string, ReadableJsonObject> m_Objects;
	};

}
