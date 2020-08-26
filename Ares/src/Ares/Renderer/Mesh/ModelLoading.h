#pragma once
#include "Ares/Core/Core.h"
#include "Ares/Math/Math.h"
#include "Ares/Renderer/Material.h"
#include "Ares/Renderer/Mesh.h"
#include "Ares/Renderer/Animation.h"
#include "ModelNodeMap.h"

struct aiNode;
struct aiAnimation;
struct aiScene;

namespace Ares
{
	
	class ModelLoading
	{
	public:
		static void LoadModel(const std::string& modelPath, Ref<Mesh>& mesh, Ref<ModelNodeMap>& modelNodeMap, std::vector<Ref<Material>>& materials, std::vector<Ref<Animation>>& animations);
	private:
		static void LoadAnimations(const aiScene* scene, std::vector<Ref<Animation>>& animations);
		static void LoadMaterials(const aiScene* scene, const std::string& modelFilePath, std::vector<Ref<Material>>& materials);
		static Ref<Animation> AssimpAnimation2AresAnimation(const aiScene* aiScene, const aiAnimation* aiAnimation);
		static Ref<ModelNodeMap> CreateModelNodeMap(aiNode* rootNode, std::vector<Submesh>& m_Submeshes, const std::vector<Matrix4>& boneInfo, const std::unordered_map<std::string, uint32_t>& boneMapping);
		static Ref<ModelNode> CreateModelNodes(aiNode* aiNode, std::vector<Submesh>& m_Submeshes, Ref<ModelNode> parent, const std::vector<Matrix4>& boneInfo, const std::unordered_map<std::string, uint32_t>& boneMapping);
	};
}