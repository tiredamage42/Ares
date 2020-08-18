#pragma once

#include "Ares/Renderer/ShaderUniform.h"
#include "Ares/Renderer/Shader.h"
namespace Ares {

	class OpenGLShaderResourceDeclaration : public ShaderResourceDeclaration
	{
	public:
		/*enum class Type
		{
			NONE, TEXTURE2D, TEXTURECUBE
		};*/
	private:
		friend class OpenGLShader;
	private:
		std::string m_Name;
		//size_t m_HashName;
		uint32_t m_TexSlot = 0;
		//int32_t m_Location;
		//int32_t m_Locations[2];
		//uint32_t m_Count;
		Type m_Type;
	public:
		OpenGLShaderResourceDeclaration(Type type, const std::string& name);// , uint32_t count);

		//inline virtual const size_t GetHashName() const override { return m_HashName; }

		inline const std::string& GetName() const override { return m_Name; }
		inline uint32_t GetRegister() const override { return m_TexSlot; }
		//inline int32_t GetLocation(ShaderVariant variant) const { return m_Locations[(size_t)variant]; }

		//inline uint32_t GetCount() const override { return m_Count; }

		//inline Type GetType() const { return m_Type; }
		inline virtual Type GetType() const override { return m_Type; }

	public:
		static Type StringToType(const std::string& type);
		static std::string TypeToString(Type type);
	};


	class OpenGLShaderResourceArrayDeclaration : public ShaderResourceArrayDeclaration
	{
	public:
		/*enum class Type
		{
			NONE, TEXTURE2D, TEXTURECUBE
		};*/
	private:
		friend class OpenGLShader;
	private:
		std::string m_Name;
		//size_t m_HashName;
		//uint32_t m_TexSlot = 0;
		//int32_t m_Location;
		//int32_t m_Locations[2];
		uint32_t m_Count;
		ShaderResourceDeclaration::Type m_Type;
	public:
		OpenGLShaderResourceArrayDeclaration(ShaderResourceDeclaration::Type type, const std::string& name, uint32_t count);

		//inline virtual const size_t GetHashName() const override { return m_HashName; }

		inline const std::string& GetName() const override { return m_Name; }
		//inline uint32_t GetRegister() const override { return m_TexSlot; }
		//inline int32_t GetLocation(ShaderVariant variant) const { return m_Locations[(size_t)variant]; }

		inline uint32_t GetCount() const override { return m_Count; }

		//inline Type GetType() const { return m_Type; }
		inline virtual ShaderResourceDeclaration::Type GetType() const override { return m_Type; }

	};






	/*
	
	
	class ShaderResourceArrayDeclaration
	{

	public:
		
	UniformAttributes m_Attributes;
	virtual ShaderResourceDeclaration::Type GetType() const = 0;

	virtual const std::string& GetName() const = 0;
	//virtual const size_t GetHashName() const = 0;

	//virtual uint32_t GetRegister() const = 0;
	virtual uint32_t GetCount() const = 0;
};


typedef std::vector<ShaderResourceDeclaration*> ShaderResourceList;
typedef std::vector<ShaderResourceArrayDeclaration*> ShaderResourceArrayList;

	
	*/



























	class OpenGLShaderUniformDeclaration : public ShaderUniformDeclaration
	{
	private:
		friend class OpenGLShader;
		friend class OpenGLShaderUniformBufferDeclaration;
	public:
		/*enum class Type
		{
			NONE, FLOAT32, VEC2, VEC3, VEC4, MAT3, MAT4, INT32, STRUCT
		};*/
	private:
		std::string m_Name;
		size_t m_HashName;

		uint32_t m_Size;
		uint32_t m_Count;
		uint32_t m_Offset;
		//ShaderDomain m_Domain;

		//bool m_HasDefaultValue = false;
		//byte* m_DefaultValue;

		Type m_Type;


		//ShaderStruct* m_Struct;
		
		
		//mutable int32_t m_Location;

		int32_t m_Locations[2];
	public:
		//OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const std::string& name, uint32_t count = 1);
		//OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* uniformStruct, const std::string& name, uint32_t count = 1);

		
		OpenGLShaderUniformDeclaration(Type type, const std::string& name, uint32_t count = 1);
		//OpenGLShaderUniformDeclaration(ShaderStruct* uniformStruct, const std::string& name, uint32_t count = 1);


		//inline virtual bool HasDefaultValue() const override { return m_HasDefaultValue; }
		//inline virtual byte* GetDefaultValue() const override { return m_DefaultValue; }


		inline const std::string& GetName() const override { return m_Name; }
		//inline virtual const size_t GetHashName() const override { return m_HashName; }

		inline uint32_t GetSize() const override { return m_Size; }
		inline uint32_t GetCount() const override { return m_Count; }
		inline uint32_t GetOffset() const override { return m_Offset; }
		
		//inline uint32_t GetAbsoluteOffset() const { return m_Struct ? m_Struct->GetOffset() + m_Offset : m_Offset; }
		inline uint32_t GetAbsoluteOffset() const { return m_Offset; }

		//inline ShaderDomain GetDomain() const { return m_Domain; }


		inline virtual Type GetType() const override { return m_Type; }

		int32_t GetLocation(ShaderVariant variant) const { return m_Locations[(size_t)variant]; }

		//inline Type GetType() const { return m_Type; }
		inline bool IsArray() const { return m_Count > 1; }

		//inline const ShaderStruct& GetShaderUniformStruct() const { ARES_CORE_ASSERT(m_Struct, ""); return *m_Struct; }
	protected:
		void SetOffset(uint32_t offset) override;
	public:
		static uint32_t SizeOfUniformType(Type type);
		static Type StringToType(const std::string& type);
		static std::string TypeToString(Type type);
	};
	/*

	struct GLShaderUniformField
	{
		OpenGLShaderUniformDeclaration::Type type;
		std::string name;
		uint32_t count;
		mutable uint32_t size;
		//mutable int32_t location;
	};

	*/
	class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration
	{
	private:
		friend class Shader;
	private:
		std::string m_Name;
		ShaderUniformList m_Uniforms;
		//uint32_t m_Register;
		uint32_t m_Size;
		//ShaderDomain m_Domain;
	public:
		//OpenGLShaderUniformBufferDeclaration(const std::string& name, ShaderDomain domain);
		OpenGLShaderUniformBufferDeclaration(const std::string& name);

		void PushUniform(OpenGLShaderUniformDeclaration* uniform);

		inline const std::string& GetName() const override { return m_Name; }
		//inline uint32_t GetRegister() const override { return m_Register; }
		inline uint32_t GetSize() const override { return m_Size; }
		//virtual ShaderDomain GetDomain() const { return m_Domain; }
		inline const ShaderUniformList& GetUniformDeclarations() const override { return m_Uniforms; }

		ShaderUniformDeclaration* FindUniform(const std::string& name);
	};

}
