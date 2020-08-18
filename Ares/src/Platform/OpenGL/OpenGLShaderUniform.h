#pragma once

#include "Ares/Renderer/ShaderUniform.h"
#include "Ares/Renderer/Shader.h"
namespace Ares {

	class OpenGLShaderResourceDeclaration : public ShaderResourceDeclaration
	{
		friend class OpenGLShader;
	private:
		std::string m_Name;
		uint32_t m_TexSlot = 0;
		Type m_Type;
	public:
		OpenGLShaderResourceDeclaration(Type type, const std::string& name);

		inline const std::string& GetName() const override { return m_Name; }
		inline uint32_t GetRegister() const override { return m_TexSlot; }
		inline virtual Type GetType() const override { return m_Type; }

		static Type StringToType(const std::string& type);
		static std::string TypeToString(Type type);
	};

	class OpenGLShaderResourceArrayDeclaration : public ShaderResourceArrayDeclaration
	{
		friend class OpenGLShader;
	private:
		std::string m_Name;
		uint32_t m_Count;
		ShaderResourceDeclaration::Type m_Type;
	public:
		OpenGLShaderResourceArrayDeclaration(ShaderResourceDeclaration::Type type, const std::string& name, uint32_t count);
		inline const std::string& GetName() const override { return m_Name; }
		inline uint32_t GetCount() const override { return m_Count; }
		inline virtual ShaderResourceDeclaration::Type GetType() const override { return m_Type; }
	};


	class OpenGLShaderUniformDeclaration : public ShaderUniformDeclaration
	{
		friend class OpenGLShader;
		friend class OpenGLShaderUniformBufferDeclaration;
	private:
		std::string m_Name;
		uint32_t m_Size;
		uint32_t m_Count;
		uint32_t m_Offset;
		Type m_Type;
		int32_t m_Locations[2];
	public:
		OpenGLShaderUniformDeclaration(Type type, const std::string& name, uint32_t count = 1);
		inline const std::string& GetName() const override { return m_Name; }
		inline uint32_t GetSize() const override { return m_Size; }
		inline uint32_t GetCount() const override { return m_Count; }
		inline uint32_t GetOffset() const override { return m_Offset; }
		inline uint32_t GetAbsoluteOffset() const { return m_Offset; }
		inline virtual Type GetType() const override { return m_Type; }
		int32_t GetLocation(ShaderVariant variant) const { return m_Locations[(size_t)variant]; }
		inline bool IsArray() const { return m_Count > 1; }
	protected:
		void SetOffset(uint32_t offset) override;
	public:
		static uint32_t SizeOfUniformType(Type type);
		static Type StringToType(const std::string& type);
		static std::string TypeToString(Type type);
	};
	
	class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration
	{
	private:
		friend class Shader;
	private:
		std::string m_Name;
		ShaderUniformList m_Uniforms;
		uint32_t m_Size;
	public:
		OpenGLShaderUniformBufferDeclaration(const std::string& name);
		void PushUniform(OpenGLShaderUniformDeclaration* uniform);
		inline const std::string& GetName() const override { return m_Name; }
		inline uint32_t GetSize() const override { return m_Size; }
		inline const ShaderUniformList& GetUniformDeclarations() const override { return m_Uniforms; }
		ShaderUniformDeclaration* FindUniform(const std::string& name);
	};

}
