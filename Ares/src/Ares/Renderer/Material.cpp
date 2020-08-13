#include "AresPCH.h"
#include "Material.h"

/*
	materials only have one buffer for multiple variations
	as the uniforms should be the same, 
	(and we dont use shader program specific things like locaation integers)
*/
namespace Ares {

	Material::Material(Ref<Shader> shader)
		: m_Shader(shader)
	{
		m_Shader->AddShaderReloadedCallback(std::bind(&Material::OnShaderReloaded, this));
		AllocateStorage();

		m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
		m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;
	}

	Material::~Material()
	{
	}

	void Material::AllocateStorage()
	{
		if (m_Shader->HasVSMaterialUniformBuffer(ShaderVariant::Static))
		{

			const auto& vsBuffer = m_Shader->GetVSMaterialUniformBuffer(ShaderVariant::Static);
			m_VSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
			m_VSUniformStorageBuffer.ZeroInitialize();
		}
		if (m_Shader->HasPSMaterialUniformBuffer(ShaderVariant::Static))
		{
			const auto& psBuffer = m_Shader->GetPSMaterialUniformBuffer(ShaderVariant::Static);
			m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
			m_PSUniformStorageBuffer.ZeroInitialize();

		}

	}

	void Material::OnShaderReloaded()
	{
		AllocateStorage();

		for (auto mi : m_MaterialInstances)
			mi->OnShaderReloaded();
	}

	ShaderUniformDeclaration* Material::FindUniformDeclaration(const std::string& name)
	{
		if (m_VSUniformStorageBuffer)
		{
			auto& declarations = m_Shader->GetVSMaterialUniformBuffer(ShaderVariant::Static).GetUniformDeclarations();
			for (ShaderUniformDeclaration* uniform : declarations)
			{
				if (uniform->GetName() == name)
					return uniform;
			}
		}

		if (m_PSUniformStorageBuffer)
		{
			auto& declarations = m_Shader->GetPSMaterialUniformBuffer(ShaderVariant::Static).GetUniformDeclarations();
			for (ShaderUniformDeclaration* uniform : declarations)
			{
				if (uniform->GetName() == name)
					return uniform;
			}
		}
		return nullptr;
	}

	ShaderResourceDeclaration* Material::FindResourceDeclaration(const std::string& name, uint8_t& samplerSlot)
	{



		samplerSlot = 0;
		
		auto& resources = m_Shader->GetResources(ShaderVariant::Static);
		for (ShaderResourceDeclaration* resource : resources)
		{
			if (resource->GetName() == name)
				return resource;

			if (resource->GetCount() == 1)
				samplerSlot++;
			
		}
		return nullptr;
	}

	Buffer& Material::GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration)
	{
		switch (uniformDeclaration->GetDomain())
		{
		case ShaderDomain::Vertex:    return m_VSUniformStorageBuffer;
		case ShaderDomain::Pixel:     return m_PSUniformStorageBuffer;
		}

		ARES_CORE_ASSERT(false, "Invalid uniform declaration domain! Material does not support this shader type.");
		return m_VSUniformStorageBuffer;
	}

	void Material::Bind(ShaderVariant variant)
	{
		m_Shader->Bind(variant);

		if (m_VSUniformStorageBuffer)
			m_Shader->SetVSMaterialUniformBuffer(m_VSUniformStorageBuffer, variant);

		if (m_PSUniformStorageBuffer)
			m_Shader->SetPSMaterialUniformBuffer(m_PSUniformStorageBuffer, variant);

		BindTextures();
	}

	void Material::BindTextures()
	{
		for (size_t i = 0; i < m_Textures.size(); i++)
		{
			auto& texture = m_Textures[i];
			if (texture)
				texture->Bind(i);
		}
	}

	// ============================================================
	// MATERIAL INSTANCE
	// ============================================================


	MaterialInstance::MaterialInstance(Ref<Material> material, const std::string& name)
		: m_Material(material), m_Name(name)
	{
		m_Material->m_MaterialInstances.insert(this);
		AllocateStorage();
	}

	MaterialInstance::~MaterialInstance()
	{
		m_Material->m_MaterialInstances.erase(this);
	}

	void MaterialInstance::OnShaderReloaded()
	{
		AllocateStorage();
		m_OverriddenValues.clear();
	}

	void MaterialInstance::AllocateStorage()
	{
		if (m_Material->m_Shader->HasVSMaterialUniformBuffer(ShaderVariant::Static))
		{

			const auto& vsBuffer = m_Material->m_Shader->GetVSMaterialUniformBuffer(ShaderVariant::Static);
			m_VSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
			memcpy(m_VSUniformStorageBuffer.Data, m_Material->m_VSUniformStorageBuffer.Data, vsBuffer.GetSize());
		}
		if (m_Material->m_Shader->HasPSMaterialUniformBuffer(ShaderVariant::Static))
		{

			const auto& psBuffer = m_Material->m_Shader->GetPSMaterialUniformBuffer(ShaderVariant::Static);
			m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
			memcpy(m_PSUniformStorageBuffer.Data, m_Material->m_PSUniformStorageBuffer.Data, psBuffer.GetSize());
		}
	}

	void MaterialInstance::SetFlag(MaterialFlag flag, bool value)
	{
		if (value)
		{
			m_Material->m_MaterialFlags |= (uint32_t)flag;
		}
		else
		{
			m_Material->m_MaterialFlags &= ~(uint32_t)flag;
		}
	}


	void MaterialInstance::OnMaterialValueUpdated(ShaderUniformDeclaration* decl)
	{
		if (m_OverriddenValues.find(decl->GetName()) == m_OverriddenValues.end())
		{
			auto& buffer = GetUniformBufferTarget(decl);
			auto& materialBuffer = m_Material->GetUniformBufferTarget(decl);
			buffer.Write(materialBuffer.Data + decl->GetOffset(), decl->GetSize(), decl->GetOffset());
		}
	}

	Buffer& MaterialInstance::GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration)
	{
		switch (uniformDeclaration->GetDomain())
		{
		case ShaderDomain::Vertex:    return m_VSUniformStorageBuffer;
		case ShaderDomain::Pixel:     return m_PSUniformStorageBuffer;
		}

		ARES_CORE_ASSERT(false, "Invalid uniform declaration domain! Material does not support this shader type.");
		return m_VSUniformStorageBuffer;
	}

	void MaterialInstance::Bind(ShaderVariant variant)
	{

		m_Material->m_Shader->Bind(variant);

		if (m_VSUniformStorageBuffer)
			m_Material->m_Shader->SetVSMaterialUniformBuffer(m_VSUniformStorageBuffer, variant);

		if (m_PSUniformStorageBuffer)
			m_Material->m_Shader->SetPSMaterialUniformBuffer(m_PSUniformStorageBuffer, variant);

		m_Material->BindTextures();
		for (size_t i = 0; i < m_Textures.size(); i++)
		{
			auto& texture = m_Textures[i];
			if (texture)
				texture->Bind(i);
		}
	}
}