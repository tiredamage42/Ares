#include "AresPCH.h" 
#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/pbrmaterial.h>


#include <stb_image.h>

//#include <glad/glad.h>

#include "Ares/Core/FileUtils/FileUtils.h"
#include "Ares/Renderer/Renderer.h"
#include "Ares/Core/Time.h"

#include "imgui.h"
#include <filesystem>
#include <iostream>

#include "Ares/Core/StringUtils.h"


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
		//aiProcess_GenNormals |              // Make sure we have legit normals
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
	//static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from)
	//{
	//	glm::mat4 to;
	//	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	//	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	//	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	//	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	//	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	//	return to;
	//}

	struct LogStream : public Assimp::LogStream
	{
		static void Initialize()
		{
			if (Assimp::DefaultLogger::isNullLogger())
			{
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn | Assimp::Logger::Info);
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



		//const uint32_t X_SEGMENTS = 64;
		//const uint32_t Y_SEGMENTS = 64;
		//
		//const uint32_t numVertices = (Y_SEGMENTS + 1) * (X_SEGMENTS + 1);
		//const uint32_t numIndicies = (Y_SEGMENTS) * (X_SEGMENTS + 1) * 2;

		//m_Vertices.reserve(numVertices);
		//m_Indices.reserve(numIndicies);

		//for (uint32_t y = 0; y <= Y_SEGMENTS; y++)
		//{
		//	for (uint32_t x = 0; x <= X_SEGMENTS; ++x)
		//	{
		//		float xSegment = (float)x / (float)X_SEGMENTS;
		//		float ySegment = (float)y / (float)Y_SEGMENTS;

		//		float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
		//		float yPos = std::cos(ySegment * PI);
		//		float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

		//		Vertex vertex;
		//		vertex.Position = glm::vec3(xPos, yPos, zPos);
		//		vertex.Normal = glm::vec3(xPos, yPos, zPos);
		//		vertex.Texcoord = glm::vec2(xSegment, ySegment);
		//		m_Vertices.push_back(vertex);
		//	}
		//	if (y < Y_SEGMENTS)
		//	{
		//		if (y % 2 == 0) // even rows
		//		{
		//			for (uint32_t x = 0; x <= X_SEGMENTS; x++)
		//			{
		//				m_Indices.push_back(y * (X_SEGMENTS + 1) + x);
		//				m_Indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
		//			}
		//		}
		//		else
		//		{
		//			for (int x = X_SEGMENTS; x >= 0; x--)
		//			{
		//				m_Indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
		//				m_Indices.push_back(y * (X_SEGMENTS + 1) + x);
		//			}
		//		}
		//	}
		//}
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

		glm::vec2 uvs[] = {
			uv01, uv11, uv10, uv00, // Bottom
			//uv01, uv11, uv10, uv00, // Left
			//uv01, uv11, uv10, uv00, // Front
			//uv01, uv11, uv10, uv00, // Back	        
			//uv01, uv11, uv10, uv00, // Right 
			//uv01, uv11, uv10, uv00  // Top
		};

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

		for (size_t i = 0; i < 6; i++)
		{
			size_t offset = 4 * i;
			m_Indices.push_back(offset + 3);
			m_Indices.push_back(offset + 1);
			m_Indices.push_back(offset + 0);
			m_Indices.push_back(offset + 3);
			m_Indices.push_back(offset + 2);
			m_Indices.push_back(offset + 1);
		}
		//m_Indices = {
		//	3,  1,  0,		3,  2,  1,      // Bottom	
		//	7,  5,  4,		7,  6,  5,      // Left
		//	11, 9,  8,		11, 10, 9,      // Front
		//	15, 13, 12,		15, 14, 13,     // Back
		//	19, 17, 16,		19, 18, 17,	    // Right
		//	23, 21, 20,		23, 22, 21,	    // Top
		//};
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
		for (uint32_t i = 0; i < m_Indices.size(); i += 3) {
			
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
		for (uint32_t i = 0; i < m_Indices.size(); i += 3)
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
		for (uint32_t i = 0; i < m_Vertices.size(); i++)
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

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(m_StaticVertices.data(), m_StaticVertices.size() * sizeof(Vertex));
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			//{ ShaderDataType::Float3, "a_Binormal" },
		});

		
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.capacity());
		m_VertexArray->SetIndexBuffer(indexBuffer);

		//Submesh submesh;
		Submesh& submesh = m_Submeshes.emplace_back();

		submesh.BaseVertex = 0;
		submesh.BaseIndex = 0;
		submesh.MaterialIndex = 0;
		submesh.IndexCount = m_Indices.capacity();
		//m_Submeshes.push_back(submesh);
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

		for (uint32_t i = 0; i < m_Indices.size(); i += 3)
		{

			m_TriangleCache[0].emplace_back(
				m_StaticVertices[m_Indices[i + 0]],
				m_StaticVertices[m_Indices[i + 1]],
				m_StaticVertices[m_Indices[i + 2]]
			);
		}


		//Ref<Shader> m_MeshShader = Shader::Find("Assets/Shaders/PBRStatic.glsl");
		//m_Materials.resize(1);
		//m_Materials[0] = CreateRef<Material>(m_MeshShader);;
		

		//Ref<Shader> m_MeshShader = Shader::Find("Assets/Shaders/pbr_static.glsl");


		//m_BaseMaterial = CreateRef<Material>(m_MeshShader);
		/*m_MaterialOverrides.resize(1);
		m_MaterialOverrides[0] = CreateRef<MaterialInstance>(m_BaseMaterial);;*/
	}


	static Ref<Texture2D> LoadTexture(const aiTexture* texture, bool srgb)
	{

		int width, height, components_per_pixel;

		unsigned char* image_data = nullptr;

		if (texture->mHeight == 0)
		{
			image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &components_per_pixel, 0);
		}
		else
		{
			image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &components_per_pixel, 0);
		}

		// components_per_pixel?
		Ref<Texture2D> t = Texture2D::Create(
			srgb ? TextureFormat::RGB : TextureFormat::RGBA, 
			width, height, 
			TextureWrap::Clamp,
			FilterType::Trilinear, 
			true
		);
		t->SetData(image_data);
		return t;
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


		if (!lImportStatus) {
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



	static const size_t u_AlbedoTexture = StringUtils::String2Hash("u_AlbedoTexture");
	static const size_t u_NormalTexture = StringUtils::String2Hash("u_NormalTexture");
		



	Mesh::Mesh(const std::string& filepath, std::vector<Ref<Material>>& m_Materials)
		: m_FilePath(filepath)
	{
		LogStream::Initialize();

		ARES_CORE_INFO("Loading mesh: {0}", filepath.c_str());

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

		m_Scene = scene;


		
		m_IsAnimated = scene->mAnimations != nullptr;

		//Ref<Shader> m_MeshShader = Shader::Find("Assets/Shaders/PBRStatic.glsl");
		Ref<Shader> m_MeshShader = Shader::Find("Assets/Shaders/Standard.glsl");

		/*Ref<Shader> m_MeshShader = nullptr;
		
		if (m_IsAnimated)
		{
			m_MeshShader = Shader::Find("Assets/Shaders/pbr_anim.glsl");
		}
		else
		{
			m_MeshShader = Shader::Find("Assets/Shaders/pbr_static.glsl");
		}*/

		//m_BaseMaterial = CreateRef<Material>(m_MeshShader);

		// m_MaterialInstance = std::make_shared<MaterialInstance>(m_BaseMaterial);

		m_InverseTransform = glm::inverse(Mat4FromAssimpMat4(scene->mRootNode->mTransformation));

		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		m_Submeshes.reserve(scene->mNumMeshes);
		for (size_t m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];


			//Submesh submesh;
			Submesh& submesh = m_Submeshes.emplace_back();
			submesh.BaseVertex = vertexCount;
			submesh.BaseIndex = indexCount;
			submesh.MaterialIndex = mesh->mMaterialIndex;
			submesh.IndexCount = mesh->mNumFaces * 3;
			//m_Submeshes.push_back(submesh);
			submesh.MeshName = mesh->mName.C_Str();

			vertexCount += mesh->mNumVertices;
			indexCount += submesh.IndexCount;



			ARES_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
			ARES_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

			if (m_IsAnimated)
			{
				for (size_t i = 0; i < mesh->mNumVertices; i++)
				{
					AnimatedVertex vertex;
					vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
					vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

					if (mesh->HasTangentsAndBitangents())
					{
						vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
						//vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
					}

					if (mesh->HasTextureCoords(0))
						vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

					m_AnimatedVertices.push_back(vertex);
				}
			}
			else
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
				}
			}

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


				if (!m_IsAnimated)
				{
					m_TriangleCache[m].emplace_back(
						m_StaticVertices[mesh->mFaces[i].mIndices[0] + submesh.BaseVertex], 
						m_StaticVertices[mesh->mFaces[i].mIndices[1] + submesh.BaseVertex],
						m_StaticVertices[mesh->mFaces[i].mIndices[2] + submesh.BaseVertex]
					);

				}
			}
		}

		/*ARES_CORE_LOG("NODES:");
		ARES_CORE_LOG("-----------------------------");*/
		TraverseNodes(scene->mRootNode);
		//ARES_CORE_LOG("-----------------------------");

		// Bones
		if (m_IsAnimated)
		{

			for (size_t m = 0; m < scene->mNumMeshes; m++)
			{
				aiMesh* mesh = scene->mMeshes[m];
				Submesh& submesh = m_Submeshes[m];

				for (size_t i = 0; i < mesh->mNumBones; i++)
				{
					aiBone* bone = mesh->mBones[i];
					std::string boneName(bone->mName.data);
					int boneIndex = 0;

					if (m_BoneMapping.find(boneName) == m_BoneMapping.end())
					{
						// Allocate an index for a new bone
						boneIndex = m_BoneCount;
						m_BoneCount++;
						BoneInfo bi;
						m_BoneInfo.push_back(bi);
						m_BoneInfo[boneIndex].BoneOffset = Mat4FromAssimpMat4(bone->mOffsetMatrix);
						m_BoneMapping[boneName] = boneIndex;
					}
					else
					{
						ARES_CORE_LOG("Found existing bone in map");
						boneIndex = m_BoneMapping[boneName];
					}

					for (size_t j = 0; j < bone->mNumWeights; j++)
					{
						int VertexID = submesh.BaseVertex + bone->mWeights[j].mVertexId;
						float Weight = bone->mWeights[j].mWeight;
						//m_AnimatedVertices[VertexID].AddBoneData(boneIndex, Weight);
						m_AnimatedVertices[VertexID].AddBoneData2((float)boneIndex, Weight);
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

			m_BoneMatrixTexture = Texture2D::Create(TextureFormat::Float16, 4, m_BoneCount, TextureWrap::Clamp, FilterType::Point, false);
			m_BoneMatrixData = new float[16 * m_BoneCount];
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

			//m_Textures.resize(scene->mNumMaterials);
			
			//m_MaterialOverrides.resize(scene->mNumMaterials);
			m_Materials.resize(scene->mNumMaterials);



			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto aiMaterial = scene->mMaterials[i];
				auto aiMaterialName = aiMaterial->GetName();

				/*auto mi = CreateRef<MaterialInstance>(m_BaseMaterial, aiMaterialName.data);
				m_MaterialOverrides[i] = mi;*/

				auto mi = CreateRef<Material>(m_MeshShader, aiMaterialName.data);
				m_Materials[i] = mi;



				ARES_CORE_INFO(" {0}; Index = {1}", aiMaterialName.data, i);
				aiString aiTexPath;
				uint32_t textureCount = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
				ARES_CORE_LOG("  TextureCount = {0}", textureCount);

				aiColor3D aiColor;
				aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
				mi->SetValue("u_AlbedoColor", glm::vec3{ aiColor.r, aiColor.g, aiColor.b });

				float shininess, metalness;
				aiMaterial->Get(AI_MATKEY_SHININESS, shininess);
				aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness);
				metalness = 0;


				// float roughness = 1.0f - shininess * 0.01f;
				// roughness *= roughness;
				float roughness = 1.0f - glm::sqrt(shininess / 100.0f);
				ARES_CORE_LOG("    COLOR = {0}, {1}, {2}", aiColor.r, aiColor.g, aiColor.b);
				ARES_CORE_LOG("    ROUGHNESS = {0}", roughness);


				bool hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
				if (hasAlbedoMap)
				{
					std::string imgPath = aiTexPath.data;
					if (hasEmbeddedTextures)
					{
						// remove aitexpath part before ".fbm/..." then add teh filename
						// this is pretty specific to the tests with mixamo models with embedded textures
						imgPath = FileUtils::ExtractFileNameFromPath(filepath) + imgPath.substr(imgPath.find(".fbm"));
					}

					/*for (int i = 0; i < scene->mNumTextures; i++)
					{
						auto t = scene->mTextures[i];
					}*/


					//aiString texture_file;
					//aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file);
					//auto aiTexture = scene->GetEmbeddedTexture(texture_file.C_Str());
					//auto texture = LoadTexture(aiTexture, true);

					//std::string imgPath = FileUtils::RemoveDirectoryFromPath(aiTexPath.data);
					//std::string imgPath = aiTexPath.data;

					// TODO: Temp - this should be handled by our filesystem
					std::filesystem::path path = filepath;
					auto parentPath = path.parent_path();
					parentPath /= imgPath;
					std::string texturePath = parentPath.string();

					//texturePath = filename.substr(0, filename.size() - 1)  + "m/" + FileUtils::RemoveDirectoryFromPath(scene->mTextures[2]->mFilename.data);

					ARES_CORE_LOG("  Albedo Texture Path = {0}", texturePath);
					auto texture = Texture2D::Create(texturePath, FilterType::Trilinear, true, true);
					if (texture->Loaded())
					{
						//m_Textures[i] = texture;
						//mi->Set("u_AlbedoTexture", m_Textures[i]);
						mi->SetTexture("u_AlbedoTex", texture);
						//mi->Set("u_AlbedoTexToggle", 1.0f);
					}
					else
					{
						ARES_CORE_ERROR("Could not load texture: {0}", texturePath);
						//mi->Set("u_AlbedoTexToggle", 0.0f);
						//mi->SetValue("u_AlbedoColor", glm::vec3{ aiColor.r, aiColor.g, aiColor.b });
					}
				}
				else
				{
					//mi->Set("u_AlbedoTexToggle", 0.0f);
					//mi->SetValue("u_AlbedoColor", glm::vec3{ aiColor.r, aiColor.g, aiColor.b });
					ARES_CORE_LOG("		Mesh has no albedo map");
				}
				//continue;

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
						//mi->Set("u_NormalTexToggle", 1.0f);
					}
					else
					{
						ARES_CORE_ERROR("Could not load texture: {0}", texturePath);
						//mi->Set("u_AlbedoTexToggle", 0.0f);
						// mi->Set("u_AlbedoColor", glm::vec3{ color.r, color.g, color.b });
					}
				}
				else
				{
					ARES_CORE_LOG("Mesh has no normal map");
				}
				
#if 0



				// Roughness map
				if (aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS)
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
					ARES_CORE_LOG("  Roughness map path = {0}", texturePath);

					auto texture = Texture2D::Create(texturePath, FilterType::Trilinear, true);
					if (texture->Loaded())
					{
						mi->Set("u_RoughnessTexture", texture);
						mi->Set("u_RoughnessTexToggle", 1.0f);
					}
					else
					{
						ARES_CORE_ERROR("		Could not load texture: {0}", texturePath);
						//mi->Set("u_RoughnessTexToggle", 1.0f);
						mi->Set("u_Roughness", roughness);
					}
				}
				else
				{
					ARES_CORE_LOG("		no roughness map");
					mi->Set("u_Roughness", roughness);
				}

				// metallic

				//material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &fileBaseColor);
				//material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &fileMetallicRoughness);
				//if (aiMaterial->GetTexture(aiTextureType_UNKNOWN, 0, &aiTexPath) == AI_SUCCESS)
				if (aiMaterial->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &aiTexPath) == AI_SUCCESS)
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
					ARES_CORE_LOG("  Metallic map path = {0}", texturePath);

					auto texture = Texture2D::Create(texturePath, FilterType::Trilinear, true);
					if (texture->Loaded())
					{
						mi->Set("u_MetalnessTexture", texture);
						mi->Set("u_MetalnessTexToggle", 1.0f);
					}
					else
					{
						ARES_CORE_ERROR("		Could not load texture: {0}", texturePath);
						//mi->Set("u_RoughnessTexToggle", 1.0f);
						mi->Set("u_Metalness", roughness);
					}
				}
				else
				{
					ARES_CORE_LOG("		no Metalness map");
					mi->Set("u_Metalness", roughness);
				}
				//aiTextureType_REFLECTION aiTextureType_SPECULAR
#endif
#if 0

				// Metalness map
				if (aiMaterial->Get("$raw.ReflectionFactor|file", aiPTI_String, 0, aiTexPath) == AI_SUCCESS)
				{
					// TODO: Temp - this should be handled by Hazel's filesystem
					std::filesystem::path path = filename;
					auto parentPath = path.parent_path();
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();
					ARES_CORE_LOG("  Metalness map path = {0}", texturePath);

					auto texture = Texture2D::Create(texturePath);
					if (texture->Loaded())
					{
						mi->Set("u_MetalnessTexture", texture);
						mi->Set("u_MetalnessTexToggle", 1.0f);
					}
					else
					{
						ARES_CORE_ERROR("Could not load texture: {0}", texturePath);
						mi->Set("u_Metalness", metalness);
						//mi->Set("u_MetalnessTexToggle", 1.0f);
					}
				}
				else
				{
					ARES_CORE_LOG("Mesh has no metalness map");
					mi->Set("u_Metalness", metalness);
				}
#endif



				/*
				bool metalnessTextureFound = false;

				for (uint32_t i = 0; i < aiMaterial->mNumProperties; i++)
				{
					auto prop = aiMaterial->mProperties[i];
					
					ARES_CORE_LOG("Material Property:");
					ARES_CORE_LOG("  Name = {0}", prop->mKey.data);
					ARES_CORE_LOG("  Type = {0}", prop->mType);
					ARES_CORE_LOG("  Size = {0}", prop->mDataLength);
					float data = *(float*)prop->mData;
					ARES_CORE_LOG("  Value = {0}", data);


					switch (prop->mSemantic)
					{
					case aiTextureType_NONE:			ARES_CORE_LOG("  Semantic = aiTextureType_NONE");			break;
					case aiTextureType_DIFFUSE:			ARES_CORE_LOG("  Semantic = aiTextureType_DIFFUSE");		break;
					case aiTextureType_SPECULAR:		ARES_CORE_LOG("  Semantic = aiTextureType_SPECULAR");		break;
					case aiTextureType_AMBIENT:			ARES_CORE_LOG("  Semantic = aiTextureType_AMBIENT");		break;
					case aiTextureType_EMISSIVE:		ARES_CORE_LOG("  Semantic = aiTextureType_EMISSIVE");		break;
					case aiTextureType_HEIGHT:			ARES_CORE_LOG("  Semantic = aiTextureType_HEIGHT");			break;
					case aiTextureType_NORMALS:			ARES_CORE_LOG("  Semantic = aiTextureType_NORMALS");		break;
					case aiTextureType_SHININESS:		ARES_CORE_LOG("  Semantic = aiTextureType_SHININESS");		break;
					case aiTextureType_OPACITY:			ARES_CORE_LOG("  Semantic = aiTextureType_OPACITY");		break;
					case aiTextureType_DISPLACEMENT:	ARES_CORE_LOG("  Semantic = aiTextureType_DISPLACEMENT");	break;
					case aiTextureType_LIGHTMAP:		ARES_CORE_LOG("  Semantic = aiTextureType_LIGHTMAP");		break;
					case aiTextureType_REFLECTION:		ARES_CORE_LOG("  Semantic = aiTextureType_REFLECTION");		break;
					case aiTextureType_UNKNOWN:			ARES_CORE_LOG("  Semantic = aiTextureType_UNKNOWN");		break;
					}

					if (prop->mType == aiPTI_String)
					{
						uint32_t strLength = *(uint32_t*)prop->mData;
						std::string str(prop->mData + 4, strLength);
						//ARES_CORE_LOG("  Value = {0}", str);

						std::string key = prop->mKey.data;
						if (key == "$raw.ReflectionFactor|file")
						{

							metalnessTextureFound = true;

							std::string imgPath = str;
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
							ARES_CORE_LOG("    Metalness map path = {0}", texturePath);

							auto texture = Texture2D::Create(texturePath, FilterType::Trilinear, true);
							if (texture->Loaded())
							{
								//ARES_CORE_LOG("  Metalness map path = {0}", texturePath);
								mi->Set("u_MetalnessTexture", texture);
								mi->Set("u_MetalnessTexToggle", 1.0f);
							}
							else
							{
								ARES_CORE_ERROR("Could not load texture: {0}", texturePath);
								mi->Set("u_Metalness", metalness);
								//mi->Set("u_MetalnessTexToggle", 1.0f);
							}
							break;
						}
					}
				}
				if (!metalnessTextureFound)
				{
					ARES_CORE_LOG("    No metalness map");

					mi->Set("u_Metalness", metalness);
					mi->Set("u_MetalnessTexToggle", 0.0f);
				}

				*/

			}
			ARES_CORE_LOG("------------------------");
		}

		m_VertexArray = VertexArray::Create();

		Ref<VertexBuffer> vertBuffer = nullptr;
		
		if (m_IsAnimated)
		{
			vertBuffer = VertexBuffer::Create(m_AnimatedVertices.data(), m_AnimatedVertices.size() * sizeof(AnimatedVertex));
			vertBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Tangent" },
				//{ ShaderDataType::Float3, "a_Binormal" },
				//{ ShaderDataType::Int4, "a_BoneIDs" },
				{ ShaderDataType::Float4, "a_BoneIDs" },
				{ ShaderDataType::Float4, "a_BoneWeights" },
			});
		}
		else
		{
			vertBuffer = VertexBuffer::Create(m_StaticVertices.data(), m_StaticVertices.size() * sizeof(Vertex));
			vertBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Tangent" },
				//{ ShaderDataType::Float3, "a_Binormal" },
			});
		}

		m_VertexArray->AddVertexBuffer(vertBuffer);
	
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.capacity());
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
			//m_Submeshes[mesh].Transform = transform;

			auto& submesh = m_Submeshes[mesh];
			submesh.NodeName = node->mName.C_Str();
			submesh.Transform = transform;
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			TraverseNodes(node->mChildren[i], transform, level + 1);
		}
	}


	static uint32_t FindKey(float animTime, const aiVectorKey* keys, const unsigned int& numKeys)
	{
		ARES_CORE_ASSERT(numKeys > 0, "");
		for (uint32_t i = 0; i < numKeys - 1; i++)
			if (animTime < (float)keys[i + 1].mTime)
				return i;
		return 0;
	}

	uint32_t Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		return FindKey(AnimationTime, pNodeAnim->mPositionKeys, pNodeAnim->mNumPositionKeys);
	}
	uint32_t Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		return FindKey(AnimationTime, pNodeAnim->mScalingKeys, pNodeAnim->mNumScalingKeys);
	}
	uint32_t Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		ARES_CORE_ASSERT(pNodeAnim->mNumRotationKeys > 0, "");
		for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		return 0;
	}


	glm::vec3 Mesh::InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumPositionKeys == 1)
		{
			// No interpolation necessary for single value
			auto v = nodeAnim->mPositionKeys[0].mValue;
			return { v.x, v.y, v.z };
		}

		uint32_t PositionIndex = FindPosition(animationTime, nodeAnim);
		uint32_t NextPositionIndex = (PositionIndex + 1);
		ARES_CORE_ASSERT(NextPositionIndex < nodeAnim->mNumPositionKeys, "");
		float DeltaTime = (float)(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (animationTime - (float)nodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		/*if (Factor < 0.0f)
			Factor = 0.0f;*/
		ARES_CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");
		Factor = glm::clamp(Factor, 0.0f, 1.0f);

		const aiVector3D& Start = nodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = nodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		auto aiVec = Start + Factor * Delta;
		return { aiVec.x, aiVec.y, aiVec.z };
	}


	glm::quat Mesh::InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumRotationKeys == 1)
		{
			// No interpolation necessary for single value
			auto v = nodeAnim->mRotationKeys[0].mValue;
			return glm::quat(v.w, v.x, v.y, v.z);
		}

		uint32_t RotationIndex = FindRotation(animationTime, nodeAnim);
		uint32_t NextRotationIndex = (RotationIndex + 1);
		ARES_CORE_ASSERT(NextRotationIndex < nodeAnim->mNumRotationKeys, "");
		float DeltaTime = (float)(nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (animationTime - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		/*if (Factor < 0.0f)
			Factor = 0.0f;*/
		ARES_CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");
		Factor = glm::clamp(Factor, 0.0f, 1.0f);

		const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
		auto q = aiQuaternion();
		aiQuaternion::Interpolate(q, StartRotationQ, EndRotationQ, Factor);
		q = q.Normalize();
		return glm::quat(q.w, q.x, q.y, q.z);
	}


	glm::vec3 Mesh::InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumScalingKeys == 1)
		{
			// No interpolation necessary for single value
			auto v = nodeAnim->mScalingKeys[0].mValue;
			return { v.x, v.y, v.z };
		}

		uint32_t index = FindScaling(animationTime, nodeAnim);
		uint32_t nextIndex = (index + 1);
		ARES_CORE_ASSERT(nextIndex < nodeAnim->mNumScalingKeys, "");
		float deltaTime = (float)(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime);
		float Factor = (animationTime - (float)nodeAnim->mScalingKeys[index].mTime) / deltaTime;
		/*if (factor < 0.0f)
			factor = 0.0f;*/
		ARES_CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");
		Factor = glm::clamp(Factor, 0.0f, 1.0f);

		const auto& start = nodeAnim->mScalingKeys[index].mValue;
		const auto& end = nodeAnim->mScalingKeys[nextIndex].mValue;
		auto delta = end - start;
		auto aiVec = start + Factor * delta;
		return { aiVec.x, aiVec.y, aiVec.z };
	}

	void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& parentTransform)
	{
		std::string name(pNode->mName.data);
		const aiAnimation* animation = m_Scene->mAnimations[0];
		glm::mat4 nodeTransform(Mat4FromAssimpMat4(pNode->mTransformation));
		const aiNodeAnim* nodeAnim = FindNodeAnim(animation, name);

		if (nodeAnim)
		{
			glm::vec3 translation = InterpolateTranslation(AnimationTime, nodeAnim);
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));

			glm::quat rotation = InterpolateRotation(AnimationTime, nodeAnim);
			glm::mat4 rotationMatrix = glm::toMat4(rotation);

			glm::vec3 scale = InterpolateScale(AnimationTime, nodeAnim);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}

		glm::mat4 transform = parentTransform * nodeTransform;

		if (m_BoneMapping.find(name) != m_BoneMapping.end())
		{
			uint32_t BoneIndex = m_BoneMapping[name];
			m_BoneInfo[BoneIndex].FinalTransformation = m_InverseTransform * transform * m_BoneInfo[BoneIndex].BoneOffset;
		}

		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], transform);
	}

	const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName)
	{
		for (uint32_t i = 0; i < animation->mNumChannels; i++)
		{
			const aiNodeAnim* nodeAnim = animation->mChannels[i];
			if (std::string(nodeAnim->mNodeName.data) == nodeName)
				return nodeAnim;
		}
		return nullptr;
	}

	void Mesh::BoneTransform(float time)
	{
		ReadNodeHierarchy(time, m_Scene->mRootNode, glm::mat4(1.0f));
		//m_BoneTransforms.resize(m_BoneCount);
		for (size_t i = 0; i < m_BoneCount; i++)
		{
			//m_BoneTransforms[i] = m_BoneInfo[i].FinalTransformation;
			
			const float* pSource = (const float*)glm::value_ptr(m_BoneInfo[i].FinalTransformation);
			std::copy(pSource, pSource + 16, m_BoneMatrixData + 16 * i);
		}
		m_BoneMatrixTexture->SetData(m_BoneMatrixData);
	}

	void Mesh::OnUpdate()
	{
		if (m_IsAnimated)
		{

			if (m_AnimationPlaying)
			{
				m_WorldTime += Time::GetDeltaTime();

				float ticksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f) * m_TimeMultiplier;
				m_AnimationTime += Time::GetDeltaTime() * ticksPerSecond;
				m_AnimationTime = fmod(m_AnimationTime, (float)m_Scene->mAnimations[0]->mDuration);
			}

			BoneTransform(m_AnimationTime);
		}

	}

	//void Mesh::Render(Ref<Shader> shader, const glm::mat4& transform)
	//{

	//	if (m_IsAnimated)
	//	{

	//		if (m_AnimationPlaying)
	//		{
	//			m_WorldTime += Time::GetDeltaTime();

	//			float ticksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f) * m_TimeMultiplier;
	//			m_AnimationTime += Time::GetDeltaTime() * ticksPerSecond;
	//			m_AnimationTime = fmod(m_AnimationTime, (float)m_Scene->mAnimations[0]->mDuration);
	//		}

	//		BoneTransform(m_AnimationTime);
	//	}

	//	m_VertexArray->Bind();
	//	
	//	Renderer::Submit([this, shader, transform]() {
	//		for (Submesh& submesh : this->m_Submeshes)
	//		{

	//			if (m_IsAnimated)
	//			{
	//				for (size_t i = 0; i < this->m_BoneTransforms.size(); i++)
	//				{
	//					std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
	//					shader->SetMat4FromRenderThread(uniformName, this->m_BoneTransforms[i]);
	//				}
	//			}

	//			/*if (shader)
	//			{
	//				shader->SetMat4FromRenderThread("u_ModelMatrix", transform * submesh.Transform);
	//			}*/

	//			glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
	//		}
	//	});
	//}

	/*void Mesh::OnImGuiRender()
	{
		ImGui::Begin("Mesh Debug");
		if (ImGui::CollapsingHeader(m_FilePath.c_str()))
		{
			if (m_IsAnimated)
			{

				if (ImGui::CollapsingHeader("Animation"))
				{
					if (ImGui::Button(m_AnimationPlaying ? "Pause" : "Play"))
						m_AnimationPlaying = !m_AnimationPlaying;

					ImGui::SliderFloat("##AnimationTime", &m_AnimationTime, 0.0f, (float)m_Scene->mAnimations[0]->mDuration);
					ImGui::DragFloat("Time Scale", &m_TimeMultiplier, 0.05f, 0.0f, 10.0f);
				}
			}
		}

		ImGui::End();
	}*/

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
				//ARES_CORE_LOG("Binormal: {0}, {1}, {2}", vertex.Binormal.x, vertex.Binormal.y, vertex.Binormal.z);
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
				//ARES_CORE_LOG("Binormal: {0}, {1}, {2}", vertex.Binormal.x, vertex.Binormal.y, vertex.Binormal.z);
				ARES_CORE_LOG("Tangent: {0}, {1}, {2}", vertex.Tangent.x, vertex.Tangent.y, vertex.Tangent.z);
				ARES_CORE_LOG("TexCoord: {0}, {1}", vertex.Texcoord.x, vertex.Texcoord.y);
				ARES_CORE_LOG("--");
			}
		}
		ARES_CORE_LOG("------------------------------------------------------");
	}

	

}