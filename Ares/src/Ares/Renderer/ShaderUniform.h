#pragma once
#include "Ares/Core/Core.h"
#include "Ares/Core/Log.h"
#include <vector>
#include <string>
//#include <glm/glm.hpp>
#include "Ares/Math/Math.h"
namespace Ares
{

	enum class UniformAttribute
	{
		None = BIT(0),
		Public = BIT(1),
		Color = BIT(2),
		Range = BIT(3),
		Toggle = BIT(4),
		BumpMap = BIT(5),
		DefaultValue = BIT(6)
	};

	struct UniformAttributes
	{
		uint32_t Attributes = 0;
		glm::vec2 Range{ 0 };
		byte* DefaultValue = nullptr;

		bool HasAttribute(UniformAttribute attribute)
		{
			return (uint32_t)attribute & Attributes;
		}
	};

	class ShaderUniformDeclaration
	{
	public:
		enum class Type
		{
			NONE, FLOAT32, VEC2, VEC3, VEC4, MAT3, MAT4, INT32
		};

		UniformAttributes m_Attributes;
		virtual const std::string& GetName() const = 0;
		virtual Type GetType() const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual uint32_t GetCount() const = 0;
		virtual uint32_t GetOffset() const = 0;
	protected:
		virtual void SetOffset(uint32_t offset) = 0;
	};
	typedef std::vector<ShaderUniformDeclaration*> ShaderUniformList;

	class ShaderUniformBufferDeclaration
	{
	public:
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual const ShaderUniformList& GetUniformDeclarations() const = 0;
		virtual ShaderUniformDeclaration* FindUniform(const std::string& name) = 0;
	};


	class ShaderResourceDeclaration
	{

	public:
		enum class Type
		{
			NONE, TEXTURE2D, TEXTURECUBE
		};
		UniformAttributes m_Attributes;
		virtual Type GetType() const = 0;
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetRegister() const = 0;
	};

	class ShaderResourceArrayDeclaration
	{
	public:
		UniformAttributes m_Attributes;
		virtual ShaderResourceDeclaration::Type GetType() const = 0;
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetCount() const = 0;
	};

	typedef std::vector<ShaderResourceDeclaration*> ShaderResourceList;
	typedef std::vector<ShaderResourceArrayDeclaration*> ShaderResourceArrayList;

}