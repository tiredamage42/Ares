#include "AresPCH.h"
#include "Material.h"
#include "Ares/Renderer/Renderer.h"
/*
	materials only have one buffer for multiple variations
	as the uniforms should be the same, 
	(and we dont use shader program specific things like locaation integers)
*/
namespace Ares {

	Material::Material(Ref<Shader> shader, const std::string& name)
		: m_Shader(shader), m_Name(name)
	{
		m_Shader->AddShaderReloadedCallback(std::bind(&Material::OnShaderReloaded, this));
		AllocateStorage();

		m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
		m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;
	}
	Material::Material(Ref<Material> other, const std::string& name)
		: m_Name(name)
	{
		CopyMaterial(other);
	}

	Material::~Material()
	{
		// remove shader relaod callback
	}

	void Material::AllocateStorage()
	{
		
		if (m_Shader->HasPSMaterialUniformBuffer())
		{
			const auto& psBuffer = m_Shader->GetPSMaterialUniformBuffer();
			m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
			m_PSUniformStorageBuffer.ZeroInitialize();

			auto& declarations = psBuffer.GetUniformDeclarations();
			for (ShaderUniformDeclaration* uniform : declarations)
			{
				if (uniform->m_Attributes.HasAttribute(UniformAttribute::DefaultValue))
				{
					m_PSUniformStorageBuffer.Write(uniform->m_Attributes.DefaultValue, uniform->GetSize(), uniform->GetOffset());
				}
			}
		}

		// get single resources count
		size_t count = m_Shader->GetResources().size();
		m_Textures.resize(count);
		for (size_t i = 0; i < count; i++)
			m_Textures[i] = nullptr;
		
	}
	void Material::CopyMaterial(Ref<Material> other)
	{
		m_Shader = other->m_Shader;
		m_MaterialFlags = other->m_MaterialFlags;

		m_Shader->AddShaderReloadedCallback(std::bind(&Material::OnShaderReloaded, this));
		
		if (m_Shader->HasPSMaterialUniformBuffer())
		{
			const auto& psBuffer = m_Shader->GetPSMaterialUniformBuffer();
			m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
			memcpy(m_PSUniformStorageBuffer.Data, other->m_PSUniformStorageBuffer.Data, psBuffer.GetSize());
		}

		m_Textures.resize(other->m_Textures.size());

		for (size_t i = 0; i < other->m_Textures.size(); i++)
		{
			m_Textures[i] = other->m_Textures[i];
		}
	}

		
	void Material::OnShaderReloaded()
	{
		AllocateStorage();
	}

	ShaderUniformDeclaration* Material::FindUniformDeclaration(const std::string& name)
	{
		if (m_PSUniformStorageBuffer)
		{
			auto& declarations = m_Shader->GetPSMaterialUniformBuffer().GetUniformDeclarations();
			for (ShaderUniformDeclaration* uniform : declarations)
			{
				if (uniform->GetName() == name)
					return uniform;
			}
		}
		return nullptr;
	}

	// TODO: turn this into map
	ShaderResourceDeclaration* Material::FindResourceDeclaration(const std::string& name)
	{
		auto& resources = m_Shader->GetResources();
		for (ShaderResourceDeclaration* resource : resources)
		{
			if (resource->GetName() == name)
				return resource;
		}
		return nullptr;
	}

	void Material::Bind()
	{
		if (m_PSUniformStorageBuffer)
			m_Shader->SetPSMaterialUniformBuffer(m_PSUniformStorageBuffer);

		BindTextures();
	}

	void Material::BindTextures()
	{
		auto& resources = m_Shader->GetResources();

		uint32_t i = 0;
		for (ShaderResourceDeclaration* resource : resources)
		{
			auto texture = m_Textures[i];

			if (!texture)
			{
				if (resource->m_Attributes.HasAttribute(UniformAttribute::BumpMap))
				{
					texture = Renderer::GetDefaultBumpTexture();
				}
				else
				{
					texture = Renderer::GetWhiteTexture();
				}
			}
			texture->Bind(i++);
		}
	}
}