#include "AresPCH.h"
#include "OpenGLShaderUniform.h"
#include "Ares/Core/StringUtils.h"
namespace Ares {

	OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(Type type, const std::string& name, uint32_t count)
		: m_Type(type), m_Name(name), m_Count(count)
	{
		m_Size = SizeOfUniformType(type) * count;
	}

	void OpenGLShaderUniformDeclaration::SetOffset(uint32_t offset)
	{
		m_Offset = offset;
	}

	uint32_t OpenGLShaderUniformDeclaration::SizeOfUniformType(Type type)
	{
		switch (type)
		{
		case OpenGLShaderUniformDeclaration::Type::INT32:      return 4;
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:    return 4;
		case OpenGLShaderUniformDeclaration::Type::VEC2:       return 4 * 2;
		case OpenGLShaderUniformDeclaration::Type::VEC3:       return 4 * 3;
		case OpenGLShaderUniformDeclaration::Type::VEC4:       return 4 * 4;
		case OpenGLShaderUniformDeclaration::Type::MAT3:       return 4 * 3 * 3;
		case OpenGLShaderUniformDeclaration::Type::MAT4:       return 4 * 4 * 4;
		}
		return 0;
	}

	OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::StringToType(const std::string& type)
	{
		if (type == "int")    return Type::INT32;
		if (type == "float")    return Type::FLOAT32;
		if (type == "vec2")     return Type::VEC2;
		if (type == "vec3")     return Type::VEC3;
		if (type == "vec4")     return Type::VEC4;
		if (type == "mat3")     return Type::MAT3;
		if (type == "mat4")     return Type::MAT4;
		return Type::NONE;
	}

	std::string OpenGLShaderUniformDeclaration::TypeToString(Type type)
	{
		switch (type)
		{
		case OpenGLShaderUniformDeclaration::Type::INT32:      return "int32";
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:    return "float";
		case OpenGLShaderUniformDeclaration::Type::VEC2:       return "vec2";
		case OpenGLShaderUniformDeclaration::Type::VEC3:       return "vec3";
		case OpenGLShaderUniformDeclaration::Type::VEC4:       return "vec4";
		case OpenGLShaderUniformDeclaration::Type::MAT3:       return "mat3";
		case OpenGLShaderUniformDeclaration::Type::MAT4:       return "mat4";
		}
		return "Invalid Type";
	}

	OpenGLShaderUniformBufferDeclaration::OpenGLShaderUniformBufferDeclaration(const std::string& name)
		: m_Name(name), m_Size(0)
	{
	}

	void OpenGLShaderUniformBufferDeclaration::PushUniform(OpenGLShaderUniformDeclaration* uniform)
	{
		uint32_t offset = 0;
		if (m_Uniforms.size())
		{
			OpenGLShaderUniformDeclaration* previous = (OpenGLShaderUniformDeclaration*)m_Uniforms.back();
			offset = previous->m_Offset + previous->m_Size;
		}
		uniform->SetOffset(offset);
		m_Size += uniform->GetSize();
		m_Uniforms.push_back(uniform);
	}

	ShaderUniformDeclaration* OpenGLShaderUniformBufferDeclaration::FindUniform(const std::string& name)
	{
		for (ShaderUniformDeclaration* uniform : m_Uniforms)
		{
			if (uniform->GetName() == name)
				return uniform;
		}
		return nullptr;
	}
	
	OpenGLShaderResourceArrayDeclaration::OpenGLShaderResourceArrayDeclaration(ShaderResourceDeclaration::Type type, const std::string& name, uint32_t count)
		: m_Type(type), m_Name(name), m_Count(count)
	{
	}

	OpenGLShaderResourceDeclaration::OpenGLShaderResourceDeclaration(Type type, const std::string& name)
		: m_Type(type), m_Name(name)
	{
	}

	OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::StringToType(const std::string& type)
	{
		if (type == "sampler2D")		return Type::TEXTURE2D;
		if (type == "sampler2DMS")  return Type::TEXTURE2D;
		if (type == "samplerCube")		return Type::TEXTURECUBE;
		return Type::NONE;
	}
	std::string OpenGLShaderResourceDeclaration::TypeToString(Type type)
	{
		switch (type)
		{
		case Type::TEXTURE2D:	return "sampler2D";
		case Type::TEXTURECUBE:	return "samplerCube";
		}
		return "Invalid Type";
	}

}