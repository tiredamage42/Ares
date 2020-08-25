#include "AresPCH.h" 
#include "Mesh.h"
#include "Ares/Core/Scene.h"
#include "Ares/Core/Entity.h"
#include "Ares/Math/Math.h"
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/ext/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/quaternion.hpp>
//#include <glm/gtx/matrix_decompose.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/pbrmaterial.h>


#include "Ares/Core/FileUtils/FileUtils.h"
#include "Ares/Renderer/Renderer.h"
#include "Ares/Core/Time.h"

#include "imgui.h"
#include <filesystem>
#include <iostream>

#include "Ares/Core/StringUtils.h"
//#include "Ares/Renderer/Mesh/ModelLoadingUtils.h"
#include "Ares/Renderer/Animator.h"

#include <fbxsdk.h>
//#include <fbxfilesdk/fbxio/fbxiosettings.h>

/*
	TODO:
	Seperate model loading into seperate stages for mesh, materials, and build entity with
	mesh renderer component
*/

namespace Ares {
	static const uint32_t s_MeshImportFlags =

		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_GenSmoothNormals |
		aiProcess_Triangulate |             // Make sure we're triangles
		aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_GenUVCoords |             // Convert UVs if required 
		aiProcess_OptimizeMeshes |          // Batch draws where possible
		aiProcess_ValidateDataStructure		// Validation
	;    
	
	glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix)
	{
		glm::mat4 result;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
		result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
		result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
		result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
		return result;
	}
	/*
	*/
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

	
	static void GetSphereVertInfo(std::vector<Vertex>& m_Vertices, std::vector<uint32_t>& m_Indices)
	{
		const float PI = 3.14159265359f;
		const float radius = .5f;
		const uint32_t SECTORS = 64;
		const uint32_t STACKS = 64;

		float lengthInv = 1.0f / radius;    // vertex normal
		float sectorStep = 2.0f * PI / (float)SECTORS;
		float stackStep = PI / (float)STACKS;
		
		m_Vertices.reserve((STACKS + 1) * (SECTORS + 1));
		for (int i = 0; i <= STACKS; i++)
		{
			float yAngle = PI * 0.5f - i * stackStep;  // starting from pi/2 to -pi/2
			float y = radius * sinf(yAngle);              // r * sin(u)
			float xz = radius * cosf(yAngle);             // r * cos(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= SECTORS; j++)
			{
				float xAngle = j * sectorStep;           // starting from 0 to 2pi
				Vertex vertex;
				vertex.Position = glm::vec3(xz * cosf(xAngle), y, xz * sinf(xAngle));
				vertex.Normal = vertex.Position * lengthInv;
				vertex.Texcoord = 1.0f - glm::vec2((float)j / SECTORS, (float)i / STACKS);
				m_Vertices.push_back(vertex);
			}
		}
		
		for (int i = 0; i < STACKS; ++i)
		{
			int k1 = i * (SECTORS + 1);     // beginning of current stack
			int k2 = k1 + SECTORS + 1;      // beginning of next stack

			for (int j = 0; j < SECTORS; j++, k1++, k2++)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					m_Indices.push_back(k1);
					m_Indices.push_back(k2);
					m_Indices.push_back(k1 + 1);
				}
				// k1+1 => k2 => k2+1
				if (i != (STACKS - 1))
				{
					m_Indices.push_back(k1 + 1);
					m_Indices.push_back(k2);
					m_Indices.push_back(k2 + 1);
				}
			}
		}
	}

	static void GetCubeVertInfo(std::vector<Vertex>& m_Vertices, std::vector<uint32_t>& m_Indices)
	{
		glm::vec3 points[] = {
			glm::vec3(-.5f, -.5f,  .5f),
			glm::vec3(.5f,  -.5f,  .5f),
			glm::vec3(.5f,  -.5f, -.5f),
			glm::vec3(-.5f, -.5f, -.5f),
			glm::vec3(-.5f,  .5f,  .5f),
			glm::vec3(.5f,   .5f,  .5f),
			glm::vec3(.5f,   .5f, -.5f),
			glm::vec3(-.5f,  .5f, -.5f)
		};

		glm::vec3 verts[] = {
			points[0], points[1], points[2], points[3], // Bottom
			points[7], points[4], points[0], points[3], // Left
			points[4], points[5], points[1], points[0], // Front
			points[6], points[7], points[3], points[2], // Back
			points[5], points[6], points[2], points[1], // Right
			points[7], points[6], points[5], points[4]  // Top
		};

		glm::vec3 normals[] = {
			{  0, -1,  0 }, // Bottom
			{ -1,  0,  0 }, // Left
			{  0,  0,  1 },	// Front
			{  0,  0, -1 }, // Back
			{  1,  0,  0 }, // Right
			{  0,  1,  0 }  // Top
		};

		glm::vec2 uv00 = { 0, 0 };
		glm::vec2 uv10 = { 1, 0 };
		glm::vec2 uv01 = { 0, 1 };
		glm::vec2 uv11 = { 1, 1 };
		glm::vec2 uvs[] = { uv01, uv11, uv10, uv00 };

		const uint32_t numVertices = 24;
		m_Vertices.reserve(numVertices);
		for (size_t i = 0; i < numVertices; i++)
		{
			Vertex vertex;
			vertex.Position = verts[i];
			vertex.Normal = normals[i / 4];
			vertex.Texcoord = uvs[i % 4];
			m_Vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < 6; i++)
		{
			uint32_t offset = 4 * i;
			m_Indices.push_back(offset + 3);
			m_Indices.push_back(offset + 1);
			m_Indices.push_back(offset + 0);
			m_Indices.push_back(offset + 3);
			m_Indices.push_back(offset + 2);
			m_Indices.push_back(offset + 1);
		}
	}

	static void GetQuadVertInfo(std::vector<Vertex>& m_Vertices, std::vector<uint32_t>& m_Indices)
	{
		glm::vec3 verts[] = {
			{ -.5f, -.5f, 0 },
			{  .5f, -.5f, 0 },
			{  .5f,  .5f, 0 },
			{ -.5f,  .5f, 0 }
		};
		m_Vertices.reserve(4);
		for (size_t i = 0; i < 4; i++)
		{
			Vertex vertex;
			vertex.Position = verts[i];
			vertex.Normal = glm::vec3(0, 0, 1);
			vertex.Texcoord = glm::vec2(verts[i]) + 1.0f;
			m_Vertices.push_back(vertex);
		}

		m_Indices = { 0, 1, 2, 2, 3, 0 };
	}

	static void CalcTangents(std::vector<Vertex>& m_Vertices, std::vector<uint32_t>& m_Indices)
	{
		for (size_t i = 0; i < m_Indices.size(); i += 3) {
			
			Vertex& v0 = m_Vertices[m_Indices[i + 0]];
			Vertex& v1 = m_Vertices[m_Indices[i + 1]];
			Vertex& v2 = m_Vertices[m_Indices[i + 2]];

			glm::vec3 edge1 = v1.Position - v0.Position;
			glm::vec3 edge2 = v2.Position - v0.Position;
			
			float deltaU1 = v1.Texcoord.x - v0.Texcoord.x;
			float deltaU2 = v2.Texcoord.x - v0.Texcoord.x;

			float deltaV1 = v1.Texcoord.y - v0.Texcoord.y;
			float deltaV2 = v2.Texcoord.y - v0.Texcoord.y;

			float dividend = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
			float f = dividend == 0.0f ? 0.0f : 1.0f / dividend;
			
			glm::vec3 tangent = (deltaV2 * edge1 - deltaV1 * edge2) * f;
			//glm::vec3 bitangent = (-deltaU2 * edge1 - deltaU1 * edge2) * f;
			
			v0.Tangent += tangent;
			v1.Tangent += tangent;
			v2.Tangent += tangent;

			/*v0.Binormal += bitangent;
			v1.Binormal += bitangent;
			v2.Binormal += bitangent;*/

		}

		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			Vertex& v = m_Vertices[i];
			v.Tangent = glm::normalize(v.Tangent);
			//v.Binormal = glm::normalize(v.Binormal);
		}
	}

	void CalcNormals(std::vector<Vertex>& m_Vertices, std::vector<uint32_t>& m_Indices)
	{
		for (size_t i = 0; i < m_Indices.size(); i += 3)
		{
			Vertex& v0 = m_Vertices[m_Indices[i + 0]];
			Vertex& v1 = m_Vertices[m_Indices[i + 1]];
			Vertex& v2 = m_Vertices[m_Indices[i + 2]];

			glm::vec3 norm = glm::normalize(glm::cross(
				v1.Position - v0.Position, 
				v2.Position - v0.Position
			));

			v0.Normal += norm;
			v1.Normal += norm;
			v2.Normal += norm;
		}
		for (size_t i = 0; i < m_Vertices.size(); i++)
		{
			Vertex& v = m_Vertices[i];
			v.Normal = glm::normalize(v.Normal);
		}
	}


	Mesh::Mesh(PrimitiveMeshType primitiveType)
	{
		switch (primitiveType)
		{
		case PrimitiveMeshType::Cube:
			GetCubeVertInfo(m_StaticVertices, m_Indices);
			break;
		case PrimitiveMeshType::Quad:
			GetQuadVertInfo(m_StaticVertices, m_Indices);
			break;
		case PrimitiveMeshType::Sphere:
			GetSphereVertInfo(m_StaticVertices, m_Indices);
			break;
		default:
			ARES_CORE_ASSERT(false, "Unknown Primitive Type!");
			break;
		}

		CalcTangents(m_StaticVertices, m_Indices);
		//CalcNormals(m_StaticVertices, m_Indices);
		
		m_VertexArray = VertexArray::Create();

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(m_StaticVertices.data(), (uint32_t)(m_StaticVertices.size() * sizeof(Vertex)));
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
		});

		
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.capacity());
		m_VertexArray->SetIndexBuffer(indexBuffer);

		Submesh& submesh = m_Submeshes.emplace_back();

		submesh.BaseVertex = 0;
		submesh.BaseIndex = 0;
		submesh.MaterialIndex = 0;
		submesh.IndexCount = (uint32_t)m_Indices.capacity();
		submesh.MeshName = "PrimitiveType";

		auto& aabb = submesh.BoundingBox;
		aabb.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
		aabb.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		for (size_t i = 0; i < m_StaticVertices.size(); i++)
		{
			
			aabb.Min.x = glm::min(m_StaticVertices[i].Position.x, aabb.Min.x);
			aabb.Min.y = glm::min(m_StaticVertices[i].Position.y, aabb.Min.y);
			aabb.Min.z = glm::min(m_StaticVertices[i].Position.z, aabb.Min.z);
			aabb.Max.x = glm::max(m_StaticVertices[i].Position.x, aabb.Max.x);
			aabb.Max.y = glm::max(m_StaticVertices[i].Position.y, aabb.Max.y);
			aabb.Max.z = glm::max(m_StaticVertices[i].Position.z, aabb.Max.z);
		}

		for (size_t i = 0; i < m_Indices.size(); i += 3)
		{

			m_TriangleCache[0].emplace_back(
				m_StaticVertices[m_Indices[i + 0]],
				m_StaticVertices[m_Indices[i + 1]],
				m_StaticVertices[m_Indices[i + 2]]
			);
		}

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
	
	Mesh::Mesh(const std::string& filepath, std::vector<Ref<Material>>& m_Materials, std::vector<Ref<Animation>>& m_Animations)
		: m_FilePath(filepath)
	{
		LogStream::Initialize();

		ARES_CORE_INFO("Loading mesh: {0}", filepath.c_str());

		std::unique_ptr<Assimp::Importer>
		m_Importer = std::make_unique<Assimp::Importer>();
		
		const aiScene* scene = m_Importer->ReadFile(filepath, s_MeshImportFlags);
		if (!scene || !scene->HasMeshes())
		{
			ARES_CORE_ERROR("Failed to load mesh file: {0}", filepath);
			ARES_CORE_ASSERT(false, "");
		}

		//double factor;
		//scene->mMetaData->Get("UnitScaleFactor", factor);
		//ARES_CORE_INFO("FBX Scene Scale: {0}", factor);

		//m_Scene = scene;
		
		//m_IsAnimated = scene->mAnimations != nullptr;
		bool m_IsAnimated = false;
		for (size_t m = 0; m < scene->mNumMeshes; m++)
		{
			if (scene->mMeshes[m]->mNumBones)
			{
				m_IsAnimated = true;
				break;
			}
		}

		std::vector<AnimatedVertex> m_AnimatedVertices;


		for (uint32_t i = 0; i < scene->mNumAnimations; i++)
		{
			m_Animations.push_back(ModelLoadingUtils::AssimpAnimation2AresAnimation(scene, scene->mAnimations[i]));
		}


		Ref<Shader> m_MeshShader = Shader::Find("Assets/Shaders/Standard.glsl");

		//m_InverseTransform = glm::inverse(Mat4FromAssimpMat4(scene->mRootNode->mTransformation));

		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		m_Submeshes.reserve(scene->mNumMeshes);
		for (uint32_t m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];

			Submesh& submesh = m_Submeshes.emplace_back();
			submesh.BaseVertex = vertexCount;
			submesh.BaseIndex = indexCount;
			submesh.MaterialIndex = mesh->mMaterialIndex;
			submesh.IndexCount = mesh->mNumFaces * 3;
			submesh.MeshName = mesh->mName.C_Str();

			vertexCount += mesh->mNumVertices;
			indexCount += submesh.IndexCount;



			ARES_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
			ARES_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

			

			{
				auto& aabb = submesh.BoundingBox;
				aabb.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
				aabb.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

				for (size_t i = 0; i < mesh->mNumVertices; i++)
				{
					Vertex vertex;
					vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
					vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

					aabb.Min.x = glm::min(vertex.Position.x, aabb.Min.x);
					aabb.Min.y = glm::min(vertex.Position.y, aabb.Min.y);
					aabb.Min.z = glm::min(vertex.Position.z, aabb.Min.z);
					aabb.Max.x = glm::max(vertex.Position.x, aabb.Max.x);
					aabb.Max.y = glm::max(vertex.Position.y, aabb.Max.y);
					aabb.Max.z = glm::max(vertex.Position.z, aabb.Max.z);

					if (mesh->HasTangentsAndBitangents())
					{
						vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
						//vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
					}

					if (mesh->HasTextureCoords(0))
						vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

					m_StaticVertices.push_back(vertex);

					if (m_IsAnimated)
					{
						m_AnimatedVertices.push_back({});
					}
				}
			}


			//if (m_IsAnimated)
			//{
			//	for (size_t i = 0; i < mesh->mNumVertices; i++)
			//	{
			//		AnimatedVertex vertex;
			//		vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			//		vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			//		if (mesh->HasTangentsAndBitangents())
			//		{
			//			vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
			//			//vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			//		}

			//		if (mesh->HasTextureCoords(0))
			//			vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

			//		m_AnimatedVertices.push_back(vertex);
			//	}
			//}
			//else
			//{
			//	auto& aabb = submesh.BoundingBox;
			//	aabb.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
			//	aabb.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			//	for (size_t i = 0; i < mesh->mNumVertices; i++)
			//	{
			//		Vertex vertex;
			//		vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			//		vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			//		aabb.Min.x = glm::min(vertex.Position.x, aabb.Min.x);
			//		aabb.Min.y = glm::min(vertex.Position.y, aabb.Min.y);
			//		aabb.Min.z = glm::min(vertex.Position.z, aabb.Min.z);
			//		aabb.Max.x = glm::max(vertex.Position.x, aabb.Max.x);
			//		aabb.Max.y = glm::max(vertex.Position.y, aabb.Max.y);
			//		aabb.Max.z = glm::max(vertex.Position.z, aabb.Max.z);

			//		if (mesh->HasTangentsAndBitangents())
			//		{
			//			vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
			//			//vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			//		}

			//		if (mesh->HasTextureCoords(0))
			//			vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

			//		m_StaticVertices.push_back(vertex);
			//	}
			//}

			for (uint32_t i = 0; i < mesh->mNumFaces; i++)
			{
				bool rightIndicies = mesh->mFaces[i].mNumIndices == 3;
				if (!rightIndicies)
				{
					ARES_CORE_ERROR("Need 3 Indicies per face, found: {0}", mesh->mFaces[i].mNumIndices);
					ARES_CORE_ASSERT(false, "Must have 3 indices.");
				}

				m_Indices.push_back(mesh->mFaces[i].mIndices[0]);
				m_Indices.push_back(mesh->mFaces[i].mIndices[1]);
				m_Indices.push_back(mesh->mFaces[i].mIndices[2]);


				//if (!m_IsAnimated)
				{
					m_TriangleCache[m].emplace_back(
						m_StaticVertices[mesh->mFaces[i].mIndices[0] + (size_t)submesh.BaseVertex],
						m_StaticVertices[mesh->mFaces[i].mIndices[1] + (size_t)submesh.BaseVertex],
						m_StaticVertices[mesh->mFaces[i].mIndices[2] + (size_t)submesh.BaseVertex]
					);

				}
			}
		}

		TraverseNodes(scene->mRootNode);
		

		
		// Bones
		//if (m_IsAnimated)
		{

			std::vector<Matrix4> boneInfo;
			std::unordered_map<std::string, uint32_t> boneMapping;

			for (size_t m = 0; m < scene->mNumMeshes; m++)
			{
				aiMesh* mesh = scene->mMeshes[m];
				Submesh& submesh = m_Submeshes[m];

				for (size_t i = 0; i < mesh->mNumBones; i++)
				{
					aiBone* bone = mesh->mBones[i];
					std::string boneName(bone->mName.data);
					int boneIndex = 0;

					if (boneMapping.find(boneName) == boneMapping.end())
					{
						// Allocate an index for a new bone
						boneIndex = m_BoneCount;
						m_BoneCount++;

						boneInfo.push_back(Mat4FromAssimpMat4(bone->mOffsetMatrix));
						boneMapping[boneName] = boneIndex;

						/*
						BoneInfo bi;
						m_BoneInfo.push_back(bi);
						m_BoneInfo[boneIndex].BoneOffset = Mat4FromAssimpMat4(bone->mOffsetMatrix);
						m_BoneMapping[boneName] = boneIndex;
						*/
					}
					else
					{
						ARES_CORE_LOG("Found existing bone in map");
						boneIndex = boneMapping[boneName];
					}

					for (size_t j = 0; j < bone->mNumWeights; j++)
					{
						int VertexID = submesh.BaseVertex + bone->mWeights[j].mVertexId;
						float Weight = bone->mWeights[j].mWeight;
						m_AnimatedVertices[VertexID].AddBoneData((float)boneIndex, Weight);
					}
				}
			}

			for (size_t i = 0; i < m_AnimatedVertices.size(); i++)
			{
				AnimatedVertex& v = m_AnimatedVertices[i];

				for (size_t j = 0; j < 4; j++)
				{
					v.IDs[j] = (v.IDs[j] + .5f) / m_BoneCount;
				}
			}

			//m_BoneMatrixTexture = Texture2D::Create(TextureFormat::Float16, 4, m_BoneCount, TextureWrap::Clamp, FilterType::Point, false);
			//m_BoneMatrixData = new float[16 * (size_t)m_BoneCount];
		m_RootNode = ModelLoadingUtils::CreateModelNodes(scene->mRootNode, boneInfo, boneMapping);
		}





		// materials
		if (scene->HasMaterials())
		{
			bool hasEmbeddedTextures = (bool)scene->mNumTextures;

			// if we have embedded textures, import with fbx sdk
			// to extract .fbm folder
			if (hasEmbeddedTextures)
			{
				std::string fbmPath = filepath;
				fbmPath.replace(filepath.size() - 1, 1, "m");

				if (!FileUtils::PathExists(fbmPath))
				{
					FBXSDKImport(filepath);
				}
			}

			ARES_CORE_LOG("---- Materials - {0} ----", filepath);

			m_Materials.resize(scene->mNumMaterials);

			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto aiMaterial = scene->mMaterials[i];
				auto aiMaterialName = aiMaterial->GetName();

				auto mi = CreateRef<Material>(m_MeshShader, aiMaterialName.data);
				m_Materials[i] = mi;

				ARES_CORE_INFO(" {0}; Index = {1}", aiMaterialName.data, i);
				aiString aiTexPath;
				
				bool hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
				if (hasAlbedoMap)
				{
					std::string imgPath = aiTexPath.data;
					if (hasEmbeddedTextures)
					{
						imgPath = FileUtils::ExtractFileNameFromPath(filepath) + imgPath.substr(imgPath.find(".fbm"));
					}

					// TODO: Temp - this should be handled by our filesystem
					std::filesystem::path path = filepath;
					auto parentPath = path.parent_path();
					parentPath /= imgPath;
					std::string texturePath = parentPath.string();

					ARES_CORE_LOG("  Albedo Texture Path = {0}", texturePath);
					auto texture = Texture2D::Create(texturePath, FilterType::Trilinear, true, true);
					if (texture->Loaded())
					{
						mi->SetTexture("u_AlbedoTex", texture);
					}
					else
					{
						ARES_CORE_ERROR("Could not load texture: {0}", texturePath);
					}
				}
				else
				{
					ARES_CORE_LOG("		Mesh has no albedo map");
				}

				// Normal maps
				if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS)
				{

					std::string imgPath = aiTexPath.data;
					if (hasEmbeddedTextures)
					{
						// remove aitexpath part before ".fbm/..." then add teh filename
						// this is pretty specific to the tests with mixamo models with embedded textures
						imgPath = FileUtils::ExtractFileNameFromPath(filepath) + imgPath.substr(imgPath.find(".fbm"));
					}


					// TODO: Temp - this should be handled by Hazel's filesystem
					std::filesystem::path path = filepath;
					auto parentPath = path.parent_path();
					parentPath /= imgPath;
					std::string texturePath = parentPath.string();
					ARES_CORE_LOG("  Normal map path = {0}", texturePath);

					auto texture = Texture2D::Create(texturePath, FilterType::Trilinear, true);
					if (texture->Loaded())
					{
						mi->SetTexture("u_NormalTex", texture);
					}
					else
					{
						ARES_CORE_ERROR("Could not load texture: {0}", texturePath);
					}
				}
				else
				{
					ARES_CORE_LOG("Mesh has no normal map");
				}
				
			}
			ARES_CORE_LOG("------------------------");
		}

		m_VertexArray = VertexArray::Create();

		Ref<VertexBuffer> vertBuffer = nullptr;
		
		/*if (m_IsAnimated)
		{
			vertBuffer = VertexBuffer::Create(m_AnimatedVertices.data(), (uint32_t)(m_AnimatedVertices.size() * sizeof(AnimatedVertex)));
			vertBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Tangent" },
				{ ShaderDataType::Float4, "a_BoneIDs" },
				{ ShaderDataType::Float4, "a_BoneWeights" },
			});
		}
		else*/
		{
			vertBuffer = VertexBuffer::Create(m_StaticVertices.data(), (uint32_t)(m_StaticVertices.size() * sizeof(Vertex)));
			vertBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Tangent" },
			});
		}

		m_VertexArray->AddVertexBuffer(vertBuffer);

		if (m_IsAnimated)
		{
			Ref<VertexBuffer> vertBufferBones = VertexBuffer::Create(m_AnimatedVertices.data(), (uint32_t)(m_AnimatedVertices.size() * sizeof(AnimatedVertex)));
			vertBufferBones->SetLayout({
				{ ShaderDataType::Float4, "a_BoneIDs" },
				{ ShaderDataType::Float4, "a_BoneWeights" },
			});
			m_VertexArray->AddVertexBuffer(vertBufferBones);
		}


	
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.capacity());
		m_VertexArray->SetIndexBuffer(indexBuffer);

		//m_Scene = scene;
	}

	Mesh::~Mesh()
	{
	}


	static std::string LevelToSpaces(uint32_t level)
	{
		std::string result = "";
		for (uint32_t i = 0; i < level; i++)
			result += "--";
		return result;
	}


	


	void Mesh::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, uint32_t level)
	{
		glm::mat4 transform = parentTransform * Mat4FromAssimpMat4(node->mTransformation);

		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			uint32_t mesh = node->mMeshes[i];
			auto& submesh = m_Submeshes[mesh];
			submesh.NodeName = node->mName.C_Str();
			submesh.Transform = transform;
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			TraverseNodes(node->mChildren[i], transform, level + 1);
		}
	}
	
	/*
	template<typename K>
	static const uint32_t FindKeyIDX(float animTime, const std::vector<K>& keys)
	{
		for (size_t i = 0; i < keys.size() - 1; i++)
			if (animTime < keys[i + 1].Time)
				return i;
		return 0;
	}

	template <typename T, typename K>
	static std::tuple<T, T, float> GetInterpolationBounds(float time, const std::vector<K>& keys)
	{
		uint32_t size = keys.size();
		if (size == 1)
		{
			// No interpolation necessary for single value
			T v = keys[0].Value;
			return { v, v, 0 };
		}

		uint32_t idx0 = FindKeyIDX<K>(time, keys);
		uint32_t idx1 = idx0 + 1;
		ARES_CORE_ASSERT(idx1 < size, "");
		float DeltaTime = keys[idx1].Time - keys[idx0].Time;
		float Factor = glm::clamp((time - keys[idx0].Time) / DeltaTime, 0.0f, 1.0f);
		return { keys[idx0].Value, keys[idx1].Value, Factor };
	}

	glm::vec3 Mesh::InterpolateTranslation(float animationTime, const AnimationNode& pNodeAnim)
	{
		auto [Start, End, Factor] = GetInterpolationBounds<Vector3, PositionKey>(animationTime, pNodeAnim.Positions);
		return Start + Factor * (End - Start);
	}

	glm::quat Mesh::InterpolateRotation(float animationTime, const AnimationNode& pNodeAnim)
	{
		auto [Start, End, Factor] = GetInterpolationBounds<Quaternion, RotationKey>(animationTime, pNodeAnim.Rotations);
		return glm::normalize(glm::slerp(Start, End, Factor));
	}

	glm::vec3 Mesh::InterpolateScale(float animationTime, const AnimationNode& pNodeAnim)
	{
		auto [Start, End, Factor] = GetInterpolationBounds<Vector3, ScaleKey>(animationTime, pNodeAnim.Scales);
		return Start + Factor * (End - Start);
	}
	*/






	/*

	void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode* bone, const glm::mat4& parentTransform)
	{
		std::string name(bone->mName.data);
		Ref<Animation> animation = m_Animations[0];
		glm::mat4 nodeTransform(Mat4FromAssimpMat4(bone->mTransformation));

		bool success;
		const AnimationNode& node = animation->FindNode(name, success);
		if (success)
		{
			glm::vec3 translation = Animator::InterpolateTranslation(AnimationTime, node);
			glm::quat rotation = Animator::InterpolateRotation(AnimationTime, node);
			glm::vec3 scale = Animator::InterpolateScale(AnimationTime, node);
			nodeTransform = Math::GetTRSMatrix(translation, rotation, scale);
		}

		glm::mat4 globalTransform = parentTransform * nodeTransform;

		if (m_BoneMapping.find(name) != m_BoneMapping.end())
		{
			uint32_t BoneIndex = m_BoneMapping[name];
			// m_InverseTransform * globalTransform turns into object space
			
			//m_BoneInfo[BoneIndex].FinalTransformation = m_InverseTransform * globalTransform * m_BoneInfo[BoneIndex].BoneOffset;
			m_BoneInfo[BoneIndex].FinalTransformation = globalTransform * m_BoneInfo[BoneIndex].BoneOffset;

			//globalTransform = globalTransform * m_BoneInfo[BoneIndex].BoneOffset;
		}

		for (uint32_t i = 0; i < bone->mNumChildren; i++)
			ReadNodeHierarchy(AnimationTime, bone->mChildren[i], globalTransform);
	}
	*/

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

	/*

	void Mesh::BoneTransform(float time)
	{
		ReadNodeHierarchy(time, m_Scene->mRootNode, glm::mat4(1.0f));
		for (size_t i = 0; i < m_BoneCount; i++)
		{
			const float* pSource = (const float*)glm::value_ptr(m_BoneInfo[i].FinalTransformation);
			std::copy(pSource, pSource + 16, m_BoneMatrixData + 16 * i);
		}
		m_BoneMatrixTexture->SetData(m_BoneMatrixData);
	}

	const float Mesh::GetAnimationDuration() const { return m_Scene->mAnimations[0]->mDuration; }

	void Mesh::OnUpdate()
	{
		if (m_IsAnimated)
		{

			if (m_AnimationPlaying)
			{
				m_WorldTime += (float)Time::GetDeltaTime();

				float ticksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f) * m_TimeMultiplier;
				m_AnimationTime += (float)Time::GetDeltaTime() * ticksPerSecond;
				m_AnimationTime = fmod(m_AnimationTime, (float)m_Scene->mAnimations[0]->mDuration);
			}

			BoneTransform(m_AnimationTime);
		}

	}

	*/

	/*
	void Mesh::DumpVertexBuffer()
	{
		// TODO: Convert to ImGui
		ARES_CORE_LOG("------------------------------------------------------");
		ARES_CORE_LOG("Vertex Buffer Dump");
		ARES_CORE_LOG("Mesh: {0}", m_FilePath);
		if (m_IsAnimated)
		{
			for (size_t i = 0; i < m_AnimatedVertices.size(); i++)
			{
				auto& vertex = m_AnimatedVertices[i];
				ARES_CORE_LOG("Vertex: {0}", i);
				ARES_CORE_LOG("Position: {0}, {1}, {2}", vertex.Position.x, vertex.Position.y, vertex.Position.z);
				ARES_CORE_LOG("Normal: {0}, {1}, {2}", vertex.Normal.x, vertex.Normal.y, vertex.Normal.z);
				ARES_CORE_LOG("Tangent: {0}, {1}, {2}", vertex.Tangent.x, vertex.Tangent.y, vertex.Tangent.z);
				ARES_CORE_LOG("TexCoord: {0}, {1}", vertex.Texcoord.x, vertex.Texcoord.y);
				ARES_CORE_LOG("--");
			}
		}
		else
		{
			for (size_t i = 0; i < m_StaticVertices.size(); i++)
			{
				auto& vertex = m_StaticVertices[i];
				ARES_CORE_LOG("Vertex: {0}", i);
				ARES_CORE_LOG("Position: {0}, {1}, {2}", vertex.Position.x, vertex.Position.y, vertex.Position.z);
				ARES_CORE_LOG("Normal: {0}, {1}, {2}", vertex.Normal.x, vertex.Normal.y, vertex.Normal.z);
				ARES_CORE_LOG("Tangent: {0}, {1}, {2}", vertex.Tangent.x, vertex.Tangent.y, vertex.Tangent.z);
				ARES_CORE_LOG("TexCoord: {0}, {1}", vertex.Texcoord.x, vertex.Texcoord.y);
				ARES_CORE_LOG("--");
			}
		}
		ARES_CORE_LOG("------------------------------------------------------");
	}
	*/

	

}