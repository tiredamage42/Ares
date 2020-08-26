#include "AresPCH.h"
#include "ModelLoading.h"

#include "Ares/Core/FileUtils/FileUtils.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/pbrmaterial.h>

#include <filesystem>
#include <fbxsdk.h>

namespace Ares
{
	

#if 0

	static Entity BuildTransformHierarchy(Ref<Scene> scene, const aiNode* node, TransformComponent* parent = nullptr, bool isBase = true)
	{
		Entity entity = scene->CreateEntity(node->mName.data);
		TransformComponent* transform = entity.GetComponent<TransformComponent>();
		transform->SetParent(parent);
		transform->SetLocalTransform(Mat4FromAssimpMat4(node->mTransformation));

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			BuildTransformHierarchy(scene, node->mChildren[i], transform, false);
		}

		if (isBase)
		{
			return entity;
		}
		return {};
	}
	static Entity GetRootAndChildrenEntitiesOnLoad(Ref<Scene> scene, const aiScene* aiScene)
	{
		Entity rootNode = BuildTransformHierarchy(scene, aiScene->mRootNode);
		return rootNode;
	}
#endif

	static glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix)
	{
		glm::mat4 result;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
		result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
		result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
		result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
		return result;
	}

	Ref<ModelNodeMap> ModelLoading::CreateModelNodeMap(aiNode* rootNode, std::vector<Submesh>& m_Submeshes, const std::vector<Matrix4>& boneInfo, const std::unordered_map<std::string, uint32_t>& boneMapping)
	{
		Ref<ModelNodeMap> map = CreateRef<ModelNodeMap>();
		map->BoneCount = boneInfo.size();
		map->RootNode = CreateModelNodes(rootNode, m_Submeshes, nullptr, boneInfo, boneMapping);
		return map;
	}

	Ref<ModelNode> ModelLoading::CreateModelNodes(aiNode* aiNode, std::vector<Submesh>& m_Submeshes, Ref<ModelNode> parent, const std::vector<Matrix4>& boneInfo, const std::unordered_map<std::string, uint32_t>& boneMapping)
	{
		Ref<ModelNode> node = CreateRef<ModelNode>();
		node->DefaultLocalTransform = Mat4FromAssimpMat4(aiNode->mTransformation);
		node->Name = aiNode->mName.data;

		if (boneMapping.find(node->Name) != boneMapping.end())
		{
			uint32_t boneIndex = boneMapping.at(node->Name);
			node->IsBone = true;
			node->BoneIndex = boneIndex;
			node->BoneOffset = boneInfo[boneIndex];
		}

		for (uint32_t i = 0; i < aiNode->mNumMeshes; i++)
		{
			uint32_t mesh = aiNode->mMeshes[i];
			auto& submesh = m_Submeshes[mesh];
			submesh.ModelNode = node;
		}

		node->Parent = parent;
		for (uint32_t i = 0; i < aiNode->mNumChildren; i++)
		{
			Ref<ModelNode> child = CreateModelNodes(aiNode->mChildren[i], m_Submeshes, node, boneInfo, boneMapping);
			node->Children.push_back(child);
		}

		return node;
	}

	static void FBXSDKImport(const std::string& filePath)
	{


		/*
		Initializing the importer
			The scene importing functionality of the FBX SDK is abstracted by the FbxImporter class.
			Instances of FbxImporter are created with a reference to the program's FbxManager singleton object.
			A FbxImporter must have its FbxImporter::Initialize() method called with three parameters:

				- The path and filename of the file containing the scene to import.

				- The numeric file format identifier.Typically, this parameter is set to - 1 to
				let the importer automatically detect the file format according to the provided
				filename's extension.

				- The FbxIOSettings object containing the import configuration options.
				See IO Settings for more information.
		*/

		// Create the FBX SDK manager
		FbxManager* lSdkManager = FbxManager::Create();

		// Create an IOSettings object.
		FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		// ... Configure the FbxIOSettings object ...

		// Create an importer.
		FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");


		// Initialize the importer.
		bool lImportStatus = lImporter->Initialize(filePath.c_str(), -1, lSdkManager->GetIOSettings());

		// If any errors occur in the call to FbxImporter::Initialize(), 
		// the method returns false.To retrieve the error, you must call GetStatus().GetErrorString() 
		// from the FbxImporter object.For more information on error handling, see Error Handling.


		if (!lImportStatus)
		{
			ARES_CORE_ERROR("Call to FbxImporter::Initialize() failed.\n");
			ARES_CORE_ERROR("Error returned: {0}\n\n", lImporter->GetStatus().GetErrorString());
			ARES_CORE_ASSERT(false, "");
		}

		/*
		Importing a scene
			Once the importer has been initialized, a scene container must be created to
			load the scene from the file.Scenes in the FBX SDK are abstracted by the FbxScene class.
			For more information on scenes, see Nodesand the Scene Graph.
		*/

		// Create a new scene so it can be populated by the imported file.
		FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

		// Import the contents of the file into the scene.
		lImporter->Import(lScene);

		// After the importer has populated the scene, it is safe to destroy it to reduce memory usage.
		// The file has been imported; we can get rid of the importer.
		lImporter->Destroy();

		/*
		File version number
			The FBX file format version is incremented to reflect newly supported
			features(see Supported file formats).The FBX version of the currently imported file
			can be obtained by calling FbxImporter::GetFileVersion().
		*/

		// File format version numbers to be populated.
		//int lFileMajor, lFileMinor, lFileRevision;

		// Populate the FBX file format version numbers with the import file.
		//lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		/*
		As of FBX SDK 2012.0, the header file fbxfilesdk_version.h defines
		the preprocessor identifer FBXSDK_VERSION_STRING, which represents the version
		information as a string.
		*/

		// Destroy the SDK manager and all the other objects it was handling.
		lSdkManager->Destroy();

	}

	static void LoadTextureType(const std::string& modelPath, const aiMaterial* aiMaterial, Ref<Material> material, aiTextureType textureType, const std::string& typeName, const std::string& materialKey, bool hasEmbeddedTextures, bool srgb)
	{
		aiString aiTexPath;
		if (aiMaterial->GetTexture(textureType, 0, &aiTexPath) == AI_SUCCESS)
		{
			std::string imgPath = aiTexPath.data;
			if (hasEmbeddedTextures)
			{
				// remove aitexpath part before ".fbm/..." then add teh filename
				// this is pretty specific to the tests with mixamo models with embedded textures
				imgPath = FileUtils::ExtractFileNameFromPath(modelPath) + imgPath.substr(imgPath.find(".fbm"));
			}

			// TODO: Temp - this should be handled by Hazel's filesystem
			std::filesystem::path path = modelPath;
			auto parentPath = path.parent_path();
			parentPath /= imgPath;
			std::string texturePath = parentPath.string();
			ARES_CORE_LOG("  {0} map path = {1}", typeName, texturePath);

			auto texture = Texture2D::Create(texturePath, FilterType::Trilinear, true, srgb);
			if (texture->Loaded())
			{
				material->SetTexture(materialKey, texture);
			}
			else
			{
				ARES_CORE_ERROR("Could not load texture: {0}", texturePath);
			}
		}
		else
		{
			ARES_CORE_LOG("Mesh has no {0} map", typeName);
		}
	}
	void ModelLoading::LoadMaterials(const aiScene* scene, const std::string& modelFilePath, std::vector<Ref<Material>>& materials)
	{
		if (!scene->HasMaterials())
			return;

		bool hasEmbeddedTextures = (bool)scene->mNumTextures;

		// if we have embedded textures, import with fbx sdk
		// to extract .fbm folder
		if (hasEmbeddedTextures)
		{
			std::string fbmPath = modelFilePath;
			fbmPath.replace(modelFilePath.size() - 1, 1, "m");
			if (!FileUtils::PathExists(fbmPath))
				FBXSDKImport(modelFilePath);
		}

		ARES_CORE_LOG("---- Materials - {0} ----", modelFilePath);
		materials.resize(scene->mNumMaterials);
		for (uint32_t i = 0; i < scene->mNumMaterials; i++)
		{
			auto aiMaterial = scene->mMaterials[i];
			std::string aiMaterialName = aiMaterial->GetName().data;
			auto material = CreateRef<Material>(Shader::Find("Assets/Shaders/Standard.glsl"), aiMaterialName);
			materials[i] = material;
			ARES_CORE_INFO(" {0}; Index = {1}", aiMaterialName, i);
			LoadTextureType(modelFilePath, aiMaterial, material, aiTextureType_DIFFUSE, "Albedo", "u_AlbedoTex", hasEmbeddedTextures, true);
			LoadTextureType(modelFilePath, aiMaterial, material, aiTextureType_NORMALS, "Normal", "u_NormalTex", hasEmbeddedTextures, false);
		}
		ARES_CORE_LOG("------------------------");
	}

	static Ref<Assimp::Importer> s_Importer = nullptr;
	struct LogStream : public Assimp::LogStream
	{
		static void Initialize()
		{
			if (Assimp::DefaultLogger::isNullLogger())
			{
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);// | Assimp::Logger::Info);
			}
		}

		virtual void write(const char* message) override
		{
			ARES_CORE_ERROR("Assimp error: {0}", message);
		}
	};

	static const uint32_t s_MeshImportFlags =
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_GenSmoothNormals |
		aiProcess_Triangulate |             // Make sure we're triangles
		aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_GenUVCoords |             // Convert UVs if required 
		aiProcess_OptimizeMeshes |          // Batch draws where possible
		aiProcess_ValidateDataStructure		// Validation
		;

	void ModelLoading::LoadModel(const std::string& modelPath, Ref<Mesh>& mesh, Ref<ModelNodeMap>& modelNodeMap, std::vector<Ref<Material>>& materials, std::vector<Ref<Animation>>& animations)
	{

		ARES_CORE_INFO("Loading mesh: {0}", modelPath);
		LogStream::Initialize();

		if (!s_Importer)
			s_Importer = CreateRef<Assimp::Importer>();

		const aiScene* scene = s_Importer->ReadFile(modelPath, s_MeshImportFlags);
		if (!scene || !scene->HasMeshes())
		{
			ARES_CORE_ERROR("Failed to load mesh file: {0}", modelPath);
			ARES_CORE_ASSERT(false, "");
		}

		//double factor;
		//scene->mMetaData->Get("UnitScaleFactor", factor);
		//ARES_CORE_INFO("FBX Scene Scale: {0}", factor);

		std::vector<uint32_t> indicies;
		std::vector<Vertex> vertices;
		std::vector<Submesh> submeshes;
		std::vector<VertexBoneData> vertexBoneData;
		
		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		submeshes.reserve(scene->mNumMeshes);
		for (uint32_t m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];

			Submesh& submesh = submeshes.emplace_back();
			submesh.BaseVertex = vertexCount;
			submesh.BaseIndex = indexCount;
			submesh.MaterialIndex = mesh->mMaterialIndex;
			submesh.IndexCount = mesh->mNumFaces * 3;
			submesh.NumVertices = mesh->mNumVertices;

			vertexCount += mesh->mNumVertices;
			indexCount += submesh.IndexCount;

			ARES_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
			ARES_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

			for (size_t i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
					//vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
				}

				if (mesh->HasTextureCoords(0))
					vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

				vertices.push_back(vertex);
			}

			for (uint32_t i = 0; i < mesh->mNumFaces; i++)
			{
				indicies.push_back(mesh->mFaces[i].mIndices[0]);
				indicies.push_back(mesh->mFaces[i].mIndices[1]);
				indicies.push_back(mesh->mFaces[i].mIndices[2]);
			}
		}



		std::vector<Matrix4> boneOffsets;
		std::unordered_map<std::string, uint32_t> boneMapping;


		// Bones 
		bool hasBones = false;
		for (size_t m = 0; m < scene->mNumMeshes; m++)
		{
			if (scene->mMeshes[m]->mNumBones)
			{
				hasBones = true;
				break;
			}
		}

		if (hasBones)
		{
			vertexBoneData.resize(vertices.size());

			for (size_t m = 0; m < scene->mNumMeshes; m++)
			{
				aiMesh* mesh = scene->mMeshes[m];
				Submesh& submesh = submeshes[m];

				for (size_t i = 0; i < mesh->mNumBones; i++)
				{
					aiBone* bone = mesh->mBones[i];
					std::string boneName(bone->mName.data);
					int boneIndex = 0;

					if (boneMapping.find(boneName) == boneMapping.end())
					{
						boneOffsets.push_back(Mat4FromAssimpMat4(bone->mOffsetMatrix));
						boneMapping[boneName] = boneOffsets.size() - 1;
					}

					boneIndex = boneMapping[boneName];

					for (size_t j = 0; j < bone->mNumWeights; j++)
					{
						vertexBoneData[(size_t)submesh.BaseVertex + bone->mWeights[j].mVertexId].AddBoneData((float)boneIndex + .5f, bone->mWeights[j].mWeight);
					}
				}
			}
		}

		modelNodeMap = CreateModelNodeMap(scene->mRootNode, submeshes, boneOffsets, boneMapping);
		LoadMaterials(scene, modelPath, materials);
		LoadAnimations(scene, animations);


		mesh = CreateRef<Mesh>();
		mesh->SetIndicies(indicies);
		mesh->SetVertices(vertices);
		mesh->SetVertexBoneData(vertexBoneData);
		mesh->SetSubmeshes(submeshes);
		mesh->CalculateAABB();
		mesh->FinalizeMesh();

	}

	
	




	static void CheckNodeForAnimationRecursive(const aiNode* modelNode, const aiAnimation* aiAnimation, Ref<Animation> animation)
	{
		std::string nodeName(modelNode->mName.data);


		// check the animation to see if it animates this particular node
		const aiNodeAnim* animNode = nullptr;
		for (uint32_t i = 0; i < aiAnimation->mNumChannels; i++)
		{
			if (std::string(aiAnimation->mChannels[i]->mNodeName.data) == nodeName)
			{
				animNode = aiAnimation->mChannels[i];
				break;
			}
		}
		if (animNode)
		{
			AnimationNode ares_animNode;
			for (uint32_t i = 0; i < animNode->mNumPositionKeys; i++)
			{
				ares_animNode.Positions.push_back({ (float)animNode->mPositionKeys[i].mTime, { 
					animNode->mPositionKeys[i].mValue.x, animNode->mPositionKeys[i].mValue.y, animNode->mPositionKeys[i].mValue.z 
				} });
			}
			for (uint32_t i = 0; i < animNode->mNumRotationKeys; i++)
			{
				ares_animNode.Rotations.push_back({(float)animNode->mRotationKeys[i].mTime, {
					animNode->mRotationKeys[i].mValue.w,animNode->mRotationKeys[i].mValue.x,animNode->mRotationKeys[i].mValue.y,animNode->mRotationKeys[i].mValue.z
				}});
			}
			for (uint32_t i = 0; i < animNode->mNumScalingKeys; i++)
			{
				ares_animNode.Scales.push_back({(float)animNode->mScalingKeys[i].mTime, {
					animNode->mScalingKeys[i].mValue.x,animNode->mScalingKeys[i].mValue.y,animNode->mScalingKeys[i].mValue.z
				}});
			}
			animation->AddNode(nodeName, ares_animNode);
		}

		for (uint32_t i = 0; i < modelNode->mNumChildren; i++)
			CheckNodeForAnimationRecursive(modelNode->mChildren[i], aiAnimation, animation);
	}
	
	Ref<Animation> ModelLoading::AssimpAnimation2AresAnimation(const aiScene* aiScene, const aiAnimation* aiAnimation)
	{
		Ref<Animation> animation = CreateRef<Animation>();
		animation->m_TicksPerSecond = aiAnimation->mTicksPerSecond;
		animation->m_Duration = aiAnimation->mDuration;
		CheckNodeForAnimationRecursive(aiScene->mRootNode, aiAnimation, animation);
		return animation;
	}
	
	void ModelLoading::LoadAnimations(const aiScene* scene, std::vector<Ref<Animation>>& animations)
	{
		for (uint32_t i = 0; i < scene->mNumAnimations; i++)
		{
			animations.push_back(AssimpAnimation2AresAnimation(scene, scene->mAnimations[i]));
		}
	}











}