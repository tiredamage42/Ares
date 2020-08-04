#include "AresPCH.h" 

#include "Mesh.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <glad/glad.h>

#include "Ares/Renderer/Renderer.h"

namespace Ares {
	static const uint32_t s_MeshImportFlags =
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_Triangulate |             // Make sure we're triangles
		aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_GenNormals |              // Make sure we have legit normals
		aiProcess_GenUVCoords |             // Convert UVs if required 
		aiProcess_OptimizeMeshes |          // Batch draws where possible
		aiProcess_ValidateDataStructure |	// Validation

		aiProcess_RemoveRedundantMaterials |
		aiProcess_JoinIdenticalVertices
	;    
	

	struct LogStream : public Assimp::LogStream
	{
		static void Initialize()
		{
			if (Assimp::DefaultLogger::isNullLogger())
			{
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
			}
		}

		virtual void write(const char* message) override
		{
			ARES_CORE_ERROR("Assimp error: {0}", message);
		}
	};



	Mesh::Mesh()
	{
		const uint32_t numVertices = 36;

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

		glm::vec3 up =		{  0,  1,  0 };
		glm::vec3 down =	{  0, -1,  0 };
		glm::vec3 forward = {  0,  0, -1 };
		glm::vec3 back =	{  0,  0,  1 };
		glm::vec3 left =	{ -1,  0,  0 };
		glm::vec3 right =	{  1,  0,  0 };

		glm::vec3 normals[] = {
			down, down, down, down,             // Bottom
			left, left, left, left,             // Left
			forward, forward, forward, forward,	// Front
			back, back, back, back,             // Back
			right, right, right, right,         // Right
			up, up, up, up	                    // Top
		};

		glm::vec2 uv00 = { 0, 0 };
		glm::vec2 uv10 = { 1, 0 };
		glm::vec2 uv01 = { 0, 1 };
		glm::vec2 uv11 = { 1, 1 };

		glm::vec2 uvs[] = {
			uv01, uv11, uv10, uv00, // Bottom
			uv01, uv11, uv10, uv00, // Left
			uv01, uv11, uv10, uv00, // Front
			uv01, uv11, uv10, uv00, // Back	        
			uv01, uv11, uv10, uv00, // Right 
			uv01, uv11, uv10, uv00  // Top
		};

		uint32_t tris[] = {
			3,  1,  0,		3,  2,  1,      // Bottom	
			7,  5,  4,		7,  6,  5,      // Left
			11, 9,  8,		11, 10, 9,      // Front
			15, 13, 12,		15, 14, 13,     // Back
			19, 17, 16,		19, 18, 17,	    // Right
			23, 21, 20,		23, 22, 21,	    // Top
		};

		m_Vertices.reserve(numVertices);

		for (size_t i = 0; i < m_Vertices.capacity(); i++)
		{
			Vertex vertex;
			vertex.Position = verts[i];
			vertex.Normal = normals[i];
			vertex.Texcoord = uvs[i];
			m_Vertices.push_back(vertex);
		}

		m_VertexArray = VertexArray::Create();

		// VERTEX BUFFER =============================================================================
		Ref<VertexBuffer> vertBuffer = VertexBuffer::Create(m_Vertices.size() * sizeof(Vertex));
		vertBuffer->SetData(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

		vertBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Binormal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
		});

		m_VertexArray->AddVertexBuffer(vertBuffer);

		// Extract indices from model
		m_Indices.reserve(numVertices);
		for (size_t i = 0; i < m_Indices.capacity(); i++)
		{
			m_Indices.push_back(tris[i]);
		}

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.capacity());
		m_VertexArray->SetIndexBuffer(indexBuffer);
	}


	Mesh::Mesh(const std::string& filename)
		: m_FilePath(filename)
	{
		LogStream::Initialize();

		ARES_CORE_INFO("Loading mesh: {0}", filename.c_str());

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filename, s_MeshImportFlags);
		if (!scene || !scene->HasMeshes())
			ARES_CORE_ERROR("Failed to load mesh file: {0}", filename);

		aiMesh* mesh = scene->mMeshes[0];

		ARES_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
		ARES_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

		m_Vertices.reserve(mesh->mNumVertices);

		// Extract vertices from model
		for (size_t i = 0; i < m_Vertices.capacity(); i++)
		{
			Vertex vertex;
			vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}
			if (mesh->HasTextureCoords(0))
			{
				vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			}
			
			m_Vertices.push_back(vertex);
		}

		m_VertexArray = VertexArray::Create();

		// VERTEX BUFFER =============================================================================
		Ref<VertexBuffer> vertBuffer = VertexBuffer::Create(m_Vertices.size() * sizeof(Vertex));
		vertBuffer->SetData(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

		vertBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Binormal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
		});

		m_VertexArray->AddVertexBuffer(vertBuffer);

		// Extract indices from model
		m_Indices.reserve(mesh->mNumFaces * 3);

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			bool rightIndicies = mesh->mFaces[i].mNumIndices == 3;
			if (!rightIndicies)
			{
				ARES_CORE_ERROR("Need 3 Indicies per face, found: {0}", mesh->mFaces[i].mNumIndices);
				ARES_CORE_ASSERT(false, "Must have 3 indices.");
			}

			m_Indices.push_back(mesh->mFaces[i].mIndices[0] );
			m_Indices.push_back(mesh->mFaces[i].mIndices[1] );
			m_Indices.push_back(mesh->mFaces[i].mIndices[2] );
		}

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.capacity());
		m_VertexArray->SetIndexBuffer(indexBuffer);

	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Render()
	{

		m_VertexArray->Bind();
		
		Renderer::DrawIndexed(m_VertexArray->GetIndexBuffer()->GetCount());
	}

}