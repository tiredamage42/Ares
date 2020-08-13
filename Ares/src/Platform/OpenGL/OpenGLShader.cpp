

#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "Ares/Core/FileUtils/FileUtils.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include "Ares/Renderer/Renderer.h"

namespace Ares {


	const std::string SKINNED_FLAG = "SKINNED";
	const std::string STANDARD_VARS_FLAG = "STANDARD_VARS";

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")	return GL_VERTEX_SHADER;
		if (type == "fragment")	return GL_FRAGMENT_SHADER;
		if (type == "compute")	return GL_COMPUTE_SHADER;

		ARES_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_AssetPath(filepath)
	{

		m_Name = FileUtils::ExtractFileNameFromPath(filepath);
		//// extract name from filepath `assets/shaders/shader.glsl`
		//// find last of forward slash or back slash
		//size_t lastSlashI = filepath.find_last_of("/\\");
		//lastSlashI = lastSlashI == std::string::npos ? 0 : lastSlashI + 1;
		//size_t lastDotI = filepath.rfind('.');
		//auto count = lastDotI == std::string::npos ? filepath.size() - lastSlashI : lastDotI - lastSlashI;
		//m_Name = filepath.substr(lastSlashI, count);

		Reload();

		/*ReadShaderFromFile(filepath);

		Renderer::Submit([this]() {
			CompileAndUploadShader();
		});*/
		
		/*std::string source = ReadFile(filepath);
		std::unordered_map<GLenum, std::string> sources = PreProcess(source);

		Renderer::Submit([=]() {
			Compile(sources);
		});*/
	}

	void OpenGLShader::Reload()
	{

		// TODO: CHECK MEMORY IMPLICATIONS OF JUST CLEARING VARIANT BUFFERS ARRAY


		bool success;
		std::string source = FileUtils::GetFileContents(m_AssetPath, success);// ReadShaderFromFile(m_AssetPath);
		if (!success)
			ARES_CORE_ASSERT(false, "");
		
		m_ShaderSource = PreProcess(source);

		m_RendererIDs.clear();
		uint32_t variants = m_ShaderFlags.count(SKINNED_FLAG) ? 2 : 1;
		for (size_t i = 0; i < variants; i++)
		{
			m_RendererIDs.push_back(0);
		}
		
		if (!m_IsCompute)
		{
			for (size_t i = 0; i < variants; i++)
			{
				ShaderVariantBuffers& buffers = m_VariantBuffers.emplace_back();
				
				Parse(buffers, (ShaderVariant)i);

			}
		}
		

		
		Renderer::Submit([this]() {

			//if (m_RendererIDs.size())
			//{
				for (size_t i = 0; i < m_RendererIDs.size(); i++)
				{
					if (m_RendererIDs[i])
					{
						glDeleteProgram(m_RendererIDs[i]);
						m_RendererIDs[i] = 0;
					}
					/*glDeleteProgram(m_VariantBuffers[i].m_RendererID);*/
				}
				//m_RendererIDs.clear();
			//}
			/*if (this->m_RendererID)
			{
				glDeleteProgram(this->m_RendererID);
			}*/

			
			CompileAndUploadShader();
			if (!m_IsCompute)
			{

				uint32_t variants = m_ShaderFlags.count(SKINNED_FLAG) ? 2 : 1;
				for (uint32_t i = 0; i < variants; i++)
				{
					ShaderVariantBuffers& buffers = m_VariantBuffers[i];
					ResolveUniforms(buffers, (ShaderVariant)i);
				}

				//ResolveUniforms();
				//ValidateUniforms();
			}


			if (m_Loaded)
			{
				for (auto& callback : m_ShaderReloadedCallbacks)
					callback();
			}

			m_Loaded = true;

		}, "Reload Shader");
	}

	void OpenGLShader::AddShaderReloadedCallback(const ShaderReloadedCallback& callback)
	{
		m_ShaderReloadedCallbacks.push_back(callback);
	}


	/*OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		Renderer::Submit([=]() {
			Compile(sources);
		});
	}*/

	OpenGLShader::~OpenGLShader()
	{
		/*GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {

			glDeleteProgram(rendererID);

		}, "Delete Shader");*/


		std::vector<uint32_t> rendererIDs = m_RendererIDs;
		/*for (size_t i = 0; i < m_VariantBuffers.size(); i++)
		{
			rendererIDs.push_back(m_VariantBuffers[i].m_RendererID);
		}*/
		Renderer::Submit([rendererIDs]() {

			for (size_t i = 0; i < rendererIDs.size(); i++)
			{
				glDeleteProgram(rendererIDs[i]);
			}

		}, "Delete Shader");
	}

	void OpenGLShader::Bind(ShaderVariant variant)
	{
		Renderer::Submit([=]() {

			glUseProgram(GetRendererID(variant));
			//glUseProgram(this->m_RendererID);

		}, "Bind Shader");
	}

	void OpenGLShader::Unbind() const
	{
		Renderer::Submit([]() {
			glUseProgram(0);
		}, "Unbidn Shader");
	}
	const size_t OpenGLShader::GetVariantIndex(ShaderVariant variant) const
	{
		return std::min(m_RendererIDs.size() - 1, (size_t)variant);
	}
	uint32_t OpenGLShader::GetRendererID(ShaderVariant variant) const
	{
		return m_RendererIDs[GetVariantIndex(variant)];
		//return m_VariantBuffers[GetVariantIndex(variant)].m_RendererID;
	}




	//std::string OpenGLShader::ReadShaderFromFile(const std::string& filePath) const
	//{
	//	std::string result;
	//	std::ifstream in(filePath, std::ios::in | std::ios::binary);
	//	if (in)
	//	{
	//		// go to the end
	//		in.seekg(0, std::ios::end);
	//		size_t size = in.tellg();
	//		if (size != -1)
	//		{
	//			// resize reult to size of file
	//			result.resize(size);
	//			// go to the beginning of the files
	//			in.seekg(0, std::ios::beg);
	//			in.read(&result[0], size);
	//			//in.close();
	//		}
	//		else
	//		{
	//			ARES_CORE_ERROR("Could not read from file '{0}'", filePath);
	//			ARES_CORE_ASSERT(false, "");
	//		}
	//	}
	//	else
	//	{
	//		ARES_CORE_ERROR("Could not open file: {0}", filePath);
	//		ARES_CORE_ASSERT(false, "");

	//	}
	//	in.close();
	//	return result;
	//}

	/*OpenGLShader::ShaderType OpenGLShader::ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return ShaderType::Vertex;
		if (type == "fragment" || type == "pixel")
			return ShaderType::Fragment;

		return ShaderType::None;
	}*/

	//void OpenGLShader::ReadShaderFromFile(const std::string& filePath)
	//{
	//	std::ifstream in(filePath, std::ios::in | std::ios::binary);
	//	if (in)
	//	{
	//		// go to the end
	//		in.seekg(0, std::ios::end);
	//		size_t size = in.tellg();
	//		if (size != -1)
	//		{
	//			// resize reult to size of file
	//			m_ShaderSource.resize(size);
	//			// go to the beginning of the files
	//			in.seekg(0, std::ios::beg);
	//			in.read(&m_ShaderSource[0], size);
	//			in.close();
	//		}
	//		else
	//		{
	//			ARES_CORE_ERROR("Could not read from file '{0}'", filePath);
	//		}
	//	}
	//	else
	//	{
	//		ARES_CORE_ERROR("Could not open file: {0}", filePath);
	//	}
	//}

	
	void OpenGLShader::CompileAndUploadShader()
	{
		/*std::istringstream ss(m_ShaderSource);
		std::string token;
		while (ss >> token)
		{
			if (token == "#type")
			{
				std::string type;
				ss >> type;
				ARES_CORE_LOG("Type={0}", type);
			}

			printf("%s \n", token.c_str());
		}*/

		//std::unordered_map<GLenum, std::string> shaderSources;

		//// token to split on
		//const char* typeToken = "#type";
		//size_t typeTokenLength = strlen(typeToken);
		//// find the first one
		//size_t pos = m_ShaderSource.find(typeToken, 0);
		//while (pos != std::string::npos)
		//{
		//	size_t eol = m_ShaderSource.find_first_of("\r\n", pos);
		//	// if there were no lines after the #type
		//	ARES_CORE_ASSERT(eol != std::string::npos, "Syntax Error! nothing after #type declaration");
		//	// beginning of the type string (after '#type')
		//	size_t begin = pos + typeTokenLength + 1;
		//	// the actual type as string
		//	std::string type = m_ShaderSource.substr(begin, eol - begin);
		//	//ARES_CORE_ASSERT(ShaderTypeFromString(type), "Invalid Shader Type Specified");
		//	ARES_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invalid shader type specified");
		//	// get next line
		//	size_t nextLinePos = m_ShaderSource.find_first_not_of("\r\n", eol);
		//	ARES_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
		//	// find the next #type line
		//	pos = m_ShaderSource.find(typeToken, nextLinePos);
		//	// source shader code is from the next line, until either
		//	// the end of the file, or the next line we found #type
		//	shaderSources[ShaderTypeFromString(type)] = m_ShaderSource.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? m_ShaderSource.size() - 1 : nextLinePos));
		//}


		/*std::array<size_t, 4> typePositions;
		typePositions.fill(std::string::npos);
		unsigned int typePositionIndex = 0;

		const char* typeToken = "#type";
		size_t pos = m_ShaderSource.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = m_ShaderSource.find('\n', pos);
			ARES_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			std::string type = m_ShaderSource.substr(pos, eol);

			typePositions[typePositionIndex++] = pos;
			pos = m_ShaderSource.find(typeToken, pos + 1);
		}

		for (int i = 0; i < 4; i++)
		{
			if (typePositions[i] == std::string::npos)
				break;



		}*/


		
		uint32_t variants = m_ShaderFlags.count(SKINNED_FLAG) ? 2 : 1;
		for (uint32_t i = 0; i < variants; i++)
		{

			std::vector<GLuint> shaderRendererIDs;

			// Get a program object.
			GLuint program = glCreateProgram();

			// Read our shaders into the appropriate buffers
			for (auto& kv : m_ShaderSource)
			{
				GLenum type = kv.first;
				
				//const std::string& source = kv.second;
				std::string source = kv.second;



				if (type == GL_VERTEX_SHADER)
				{
					// TODO: this needs to be more robust
					size_t startMain = source.find("void main");

					size_t insertVars = startMain - 1;

					size_t startInMain = source.find_first_of("{", startMain) + 1;


					// add necessary bits here
					if (i == 1) // teh skinneed mesh variant
					{

						source.insert(startInMain, R"(
							mat4 _ares_internal_bone_transform = 
								_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.x) * _ares_internal_BoneWeights.x +
								_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.y) * _ares_internal_BoneWeights.y +
								_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.z) * _ares_internal_BoneWeights.z +
								_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.w) * _ares_internal_BoneWeights.w;

							mat4 ares_ModelMatrix = _ares_internal_Transform * _ares_internal_bone_transform;
							mat4 ares_MVPMatrix = ares_VPMatrix * ares_ModelMatrix;
						)");
						source.insert(insertVars, R"(
							layout(location = 4) in vec4 _ares_internal_BoneIndices;
							layout(location = 5) in vec4 _ares_internal_BoneWeights;

							uniform sampler2D _ares_internal_BoneSampler;

							mat4 _ares_internal_GetBoneMatrix(float bIdx) {
								return mat4(
									texture2D(_ares_internal_BoneSampler, vec2(0.125, bIdx)),
									texture2D(_ares_internal_BoneSampler, vec2(0.375, bIdx)),
									texture2D(_ares_internal_BoneSampler, vec2(0.625, bIdx)),
									texture2D(_ares_internal_BoneSampler, vec2(0.875, bIdx))
								);
							}

							uniform mat4 _ares_internal_Transform;
							uniform mat4 ares_VPMatrix;

						)");
					}
					else
					{
						// add necessary bits here

						if (m_ShaderFlags.count(STANDARD_VARS_FLAG))
						{

							source.insert(startInMain, R"(
								mat4 ares_ModelMatrix = _ares_internal_Transform;
								mat4 ares_MVPMatrix = ares_VPMatrix * ares_ModelMatrix;
							)");
							source.insert(insertVars, R"(
								uniform mat4 _ares_internal_Transform;
								uniform mat4 ares_VPMatrix;
							)");
						}
					}



					/*
					
					#flags STANDARD_VARS
					const std::string SKINNED_FLAG = "SKINNED";
					const std::string STANDARD_VARS_FLAG = "STANDARD_VARS";

					ares_VPMatrix
					ares_MVPMatrix
					ares_ModelMatrix

					
					
					*/
				}

				// Create an empty shader handle
				GLuint shader = glCreateShader(type);

				// Send the shader source code to GL
				// Note that std::string's .c_str is NULL character terminated.
				const GLchar* sourceCStr = (const GLchar*)source.c_str();
				glShaderSource(shader, 1, &sourceCStr, 0);

				// Compile the shader
				glCompileShader(shader);

				GLint isCompiled = 0;
				glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
				if (isCompiled == GL_FALSE)
				{
					// get error logs
					GLint maxLength = 0;
					glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

					// The maxLength includes the NULL character
					std::vector<GLchar> infoLog(maxLength);
					glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

					// We don't need the shader anymore.
					glDeleteShader(shader);


					ARES_CORE_ERROR("Shader compilation failed:\n{0}\n{1}", m_Name, infoLog.data());
					ARES_CORE_ASSERT(false, "Shader Compilation Failure!");
					break;
				}
			 
				// Attach our shaders to our program
				glAttachShader(program, shader);
				shaderRendererIDs.push_back(shader);
			}


			// Vertex and fragment shaders are successfully compiled.
			// Now time to link them together into a program.

			// Link our program
			glLinkProgram(program);

			// Note the different functions here: glGetProgram* instead of glGetShader*.
			GLint isLinked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
			if (isLinked == GL_FALSE)
			{
				// get error logs
				GLint maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

				// We don't need the program anymore.
				glDeleteProgram(program);

				// Don't leak shaders either.
				for (auto id : shaderRendererIDs)
					glDeleteShader(id);


				// Use the infoLog as you see fit.

				ARES_CORE_ERROR("Shader compilation failed:\n{0}", infoLog.data());
				ARES_CORE_ASSERT(false, "Shader Link Failure!");
				return;
			}

			// Always detach shaders after a successful link.
			for (auto id : shaderRendererIDs)
			{
				glDetachShader(program, id);
				glDeleteShader(id);
			}

			m_RendererIDs[i] = program;
			//m_VariantBuffers[i].m_RendererID = program;
			//m_RendererIDs.push_back(program);

		}

		//m_RendererID = program;
	}





	// Parsing helper functions
	const char* FindToken(const char* str, const std::string& token)
	{
		const char* t = str;
		while (t = strstr(t, token.c_str()))
		{
			bool left = str == t || isspace(t[-1]);
			bool right = !t[token.size()] || isspace(t[token.size()]);
			if (left && right)
				return t;

			t += token.size();
		}
		return nullptr;
	}

	const char* FindToken(const std::string& string, const std::string& token)
	{
		return FindToken(string.c_str(), token);
	}

	std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
	{
		size_t start = 0;
		size_t end = string.find_first_of(delimiters);

		std::vector<std::string> result;

		while (end <= std::string::npos)
		{
			std::string token = string.substr(start, end - start);
			if (!token.empty())
				result.push_back(token);

			if (end == std::string::npos)
				break;

			start = end + 1;
			end = string.find_first_of(delimiters, start);
		}

		return result;
	}

	std::vector<std::string> SplitString(const std::string& string, const char delimiter)
	{
		return SplitString(string, std::string(1, delimiter));
	}

	std::vector<std::string> Tokenize(const std::string& string)
	{
		return SplitString(string, " \t\n");
	}

	std::vector<std::string> GetLines(const std::string& string)
	{
		return SplitString(string, "\n");
	}

	std::string GetBlock(const char* str, const char** outPosition)
	{
		const char* end = strstr(str, "}");
		if (!end)
			return str;

		if (outPosition)
			*outPosition = end;
		uint32_t length = end - str + 1;
		return std::string(str, length);
	}

	std::string GetStatement(const char* str, const char** outPosition)
	{
		const char* end = strstr(str, ";");
		if (!end)
			return str;

		if (outPosition)
			*outPosition = end;
		uint32_t length = end - str + 1;
		return std::string(str, length);
	}

	bool StartsWith(const std::string& string, const std::string& start)
	{
		return string.find(start) == 0;
	}

	void OpenGLShader::Parse(ShaderVariantBuffers& variantBuffers, ShaderVariant variant)
	{
		const char* token;
		const char* vstr;
		const char* fstr;

		variantBuffers.m_Resources.clear();
		variantBuffers.m_Structs.clear();
		variantBuffers.m_VSMaterialUniformBuffer.reset();
		variantBuffers.m_PSMaterialUniformBuffer.reset();

		auto& vertexSource = m_ShaderSource[GL_VERTEX_SHADER];
		auto& fragmentSource = m_ShaderSource[GL_FRAGMENT_SHADER];

		// Vertex Shader
		vstr = vertexSource.c_str();
		while (token = FindToken(vstr, "struct"))
			ParseUniformStruct(GetBlock(token, &vstr), ShaderDomain::Vertex, variantBuffers);

		vstr = vertexSource.c_str();
		while (token = FindToken(vstr, "uniform"))
			ParseUniform(GetStatement(token, &vstr), ShaderDomain::Vertex, variantBuffers, variant);

		// Fragment Shader
		fstr = fragmentSource.c_str();
		while (token = FindToken(fstr, "struct"))
			ParseUniformStruct(GetBlock(token, &fstr), ShaderDomain::Pixel, variantBuffers);

		fstr = fragmentSource.c_str();
		while (token = FindToken(fstr, "uniform"))
			ParseUniform(GetStatement(token, &fstr), ShaderDomain::Pixel, variantBuffers, variant);
	}

	static bool IsTypeStringResource(const std::string& type)
	{
		if (type == "sampler2D")		return true;
		if (type == "sampler2DMS")		return true;
		if (type == "samplerCube")		return true;
		if (type == "sampler2DShadow")	return true;
		return false;
	}

	void OpenGLShader::ParseUniform(const std::string& statement, ShaderDomain domain, ShaderVariantBuffers& variantBuffers, ShaderVariant variant)
	{
		std::vector<std::string> tokens = Tokenize(statement);

		ARES_CORE_ASSERT(tokens.size() == 3, "Token must be size 3");
		uint32_t index = 0;

		index++; // "uniform"
		std::string typeString = tokens[index++];
		std::string name = tokens[index++];

		// Strip ; from name if present
		if (const char* s = strstr(name.c_str(), ";"))
			name = std::string(name.c_str(), s - name.c_str());

		std::string n(name);
		int32_t count = 1;
		const char* namestr = n.c_str();
		if (const char* s = strstr(namestr, "["))
		{
			name = std::string(namestr, s - namestr);

			const char* end = strstr(namestr, "]");
			std::string c(s + 1, end - s);
			count = atoi(c.c_str());
		}

		if (IsTypeStringResource(typeString))
		{
			ShaderResourceDeclaration* declaration = new OpenGLShaderResourceDeclaration(OpenGLShaderResourceDeclaration::StringToType(typeString), name, count);
			variantBuffers.m_Resources.push_back(declaration);
		}
		else
		{
			OpenGLShaderUniformDeclaration::Type t = OpenGLShaderUniformDeclaration::StringToType(typeString);
			OpenGLShaderUniformDeclaration* declaration = nullptr;

			if (t == OpenGLShaderUniformDeclaration::Type::NONE)
			{
				// Find struct
				ShaderStruct* s = FindStruct(typeString, variantBuffers);// variant);
				ARES_CORE_ASSERT(s, "");
				declaration = new OpenGLShaderUniformDeclaration(domain, s, name, count);
			}
			else
			{
				declaration = new OpenGLShaderUniformDeclaration(domain, t, name, count);
			}

			/*if (StartsWith(name, "r_"))
			{
				if (domain == ShaderDomain::Vertex)
					((OpenGLShaderUniformBufferDeclaration*)m_VSRendererUniformBuffers.front())->PushUniform(declaration);
				else if (domain == ShaderDomain::Pixel)
					((OpenGLShaderUniformBufferDeclaration*)m_PSRendererUniformBuffers.front())->PushUniform(declaration);
			}
			else
			{*/
				if (domain == ShaderDomain::Vertex)
				{
					if (!variantBuffers.m_VSMaterialUniformBuffer)
						variantBuffers.m_VSMaterialUniformBuffer.reset(new OpenGLShaderUniformBufferDeclaration("", domain));

					variantBuffers.m_VSMaterialUniformBuffer->PushUniform(declaration);
				}
				else if (domain == ShaderDomain::Pixel)
				{
					if (!variantBuffers.m_PSMaterialUniformBuffer)
						variantBuffers.m_PSMaterialUniformBuffer.reset(new OpenGLShaderUniformBufferDeclaration("", domain));

					variantBuffers.m_PSMaterialUniformBuffer->PushUniform(declaration);
				}
			//}
		}
	}

	void OpenGLShader::ParseUniformStruct(const std::string& block, ShaderDomain domain, ShaderVariantBuffers& variantBuffers)
	{
		std::vector<std::string> tokens = Tokenize(block);

		uint32_t index = 0;
		index++; // struct
		std::string name = tokens[index++];
		ShaderStruct* uniformStruct = new ShaderStruct(name);
		index++; // {
		while (index < tokens.size())
		{
			if (tokens[index] == "}")
				break;

			std::string type = tokens[index++];
			std::string name = tokens[index++];

			// Strip ; from name if present
			if (const char* s = strstr(name.c_str(), ";"))
				name = std::string(name.c_str(), s - name.c_str());

			uint32_t count = 1;
			const char* namestr = name.c_str();
			if (const char* s = strstr(namestr, "["))
			{
				name = std::string(namestr, s - namestr);

				const char* end = strstr(namestr, "]");
				std::string c(s + 1, end - s);
				count = atoi(c.c_str());
			}
			ShaderUniformDeclaration* field = new OpenGLShaderUniformDeclaration(domain, OpenGLShaderUniformDeclaration::StringToType(type), name, count);
			uniformStruct->AddField(field);
		}
		variantBuffers.m_Structs.push_back(uniformStruct);
	}

	ShaderStruct* OpenGLShader::FindStruct(const std::string& name, ShaderVariantBuffers& variantBuffers)
	{


		//size_t variantIndex = GetVariantIndex(variant);
		//ShaderVariantBuffers& variantBuffers = m_VariantBuffers[variantIndex];

		//for (ShaderStruct* s : m_Structs)
		for (ShaderStruct* s : variantBuffers.m_Structs)
		{
			if (s->GetName() == name)
				return s;
		}
		return nullptr;
	}

	void OpenGLShader::ResolveUniforms(const ShaderVariantBuffers& buffers, ShaderVariant variant)
	{
		glUseProgram(GetRendererID(variant));

		/*for (size_t i = 0; i < m_VSRendererUniformBuffers.size(); i++)
		{
			OpenGLShaderUniformBufferDeclaration* decl = (OpenGLShaderUniformBufferDeclaration*)m_VSRendererUniformBuffers[i];
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
				if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& s = uniform->GetShaderUniformStruct();
					const auto& fields = s.GetFields();
					for (size_t k = 0; k < fields.size(); k++)
					{
						OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
						field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
					}
				}
				else
				{
					uniform->m_Location = GetUniformLocation(uniform->m_Name);
				}
			}
		}

		for (size_t i = 0; i < m_PSRendererUniformBuffers.size(); i++)
		{
			OpenGLShaderUniformBufferDeclaration* decl = (OpenGLShaderUniformBufferDeclaration*)m_PSRendererUniformBuffers[i];
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
				if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& s = uniform->GetShaderUniformStruct();
					const auto& fields = s.GetFields();
					for (size_t k = 0; k < fields.size(); k++)
					{
						OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
						field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
					}
				}
				else
				{
					uniform->m_Location = GetUniformLocation(uniform->m_Name);
				}
			}
		}*/

		{
			const auto& decl = buffers.m_VSMaterialUniformBuffer;
			if (decl)
			{
				const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
				for (size_t j = 0; j < uniforms.size(); j++)
				{
					OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
					if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
					{
						const ShaderStruct& s = uniform->GetShaderUniformStruct();
						const auto& fields = s.GetFields();
						for (size_t k = 0; k < fields.size(); k++)
						{
							OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
							field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name, variant);
						}
					}
					else
					{
						uniform->m_Location = GetUniformLocation(uniform->m_Name, variant);
					}
				}
			}
		}

		{
			const auto& decl = buffers.m_PSMaterialUniformBuffer;
			if (decl)
			{
				const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
				for (size_t j = 0; j < uniforms.size(); j++)
				{
					OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
					if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
					{
						const ShaderStruct& s = uniform->GetShaderUniformStruct();
						const auto& fields = s.GetFields();
						for (size_t k = 0; k < fields.size(); k++)
						{
							OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
							field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name, variant);
						}
					}
					else
					{
						uniform->m_Location = GetUniformLocation(uniform->m_Name, variant);
					}
				}
			}
		}

		uint32_t sampler = 0;
		for (size_t i = 0; i < buffers.m_Resources.size(); i++)
		{
			OpenGLShaderResourceDeclaration* resource = (OpenGLShaderResourceDeclaration*)buffers.m_Resources[i];
			
			auto resourceName = resource->m_Name;
			int32_t location = GetUniformLocation(resource->m_Name, variant);

			if (resource->GetCount() == 1)
			{
				//resource->m_TexSlot = sampler;
				if (location != -1)
					UploadUniformInt(location, sampler);

				sampler++;
			}
			else if (resource->GetCount() > 1)
			{
				//resource->m_TexSlot = 0;
				
				uint32_t count = resource->GetCount();
				
				int* samplers = new int[count];
				for (uint32_t s = 0; s < count; s++)
					samplers[s] = s;
				
				//UploadUniformIntArray(resource->GetName(), samplers, count);
				UploadUniformIntArray(location, samplers, count);

				delete[] samplers;
			}
		}
	}

	/*void OpenGLShader::ValidateUniforms()
	{
	}*/

	void OpenGLShader::ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer)
	{
		const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
		for (size_t i = 0; i < uniforms.size(); i++)
		{
			OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[i];
			if (uniform->IsArray())
			{
				//ARES_CORE_ASSERT(false, "arrays not implemented yet...");
				ResolveAndSetUniformArray(uniform, buffer);
			}
			else
			{

				ResolveAndSetUniform(uniform, buffer);
			}
		}
	}

	void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
	{
		if (uniform->GetLocation() == -1)
			return;

		//ARES_CORE_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

		uint32_t offset = uniform->GetOffset();
		switch (uniform->GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(uniform->GetLocation(), *(float*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(uniform->GetLocation(), *(int32_t*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(uniform->GetLocation(), *(glm::vec2*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(uniform->GetLocation(), *(glm::vec3*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(uniform->GetLocation(), *(glm::vec4*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(uniform->GetLocation(), *(glm::mat3*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4(uniform->GetLocation(), *(glm::mat4*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::STRUCT:
			UploadUniformStruct(uniform, buffer.Data, offset);
			break;
		default:
			ARES_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}

	void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
	{
		uint32_t offset = uniform->GetOffset();
		switch (uniform->GetType())
		{
		/*case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(uniform->GetLocation(), *(float*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(uniform->GetLocation(), *(int32_t*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(uniform->GetLocation(), *(glm::vec2*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(uniform->GetLocation(), *(glm::vec3*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(uniform->GetLocation(), *(glm::vec4*) & buffer.Data[offset]);
			break;*/
		/*case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(uniform->GetLocation(), *(glm::mat3*) & buffer.Data[offset]);
			break;*/
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4Array(uniform->GetLocation(), *(glm::mat4*) & buffer.Data[offset], uniform->GetCount());
			break;
		/*case OpenGLShaderUniformDeclaration::Type::STRUCT:
			UploadUniformStruct(uniform, buffer.Data, offset);
			break;*/
		default:
			ARES_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}

	void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, byte* data, int32_t offset)
	{
		switch (field.GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(field.GetLocation(), *(float*)&data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(field.GetLocation(), *(int32_t*)&data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(field.GetLocation(), *(glm::vec2*) & data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(field.GetLocation(), *(glm::vec3*) & data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(field.GetLocation(), *(glm::vec4*) & data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(field.GetLocation(), *(glm::mat3*) & data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4(field.GetLocation(), *(glm::mat4*) & data[offset]);
			break;
		default:
			ARES_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{

		// find flags

		size_t flagsStartIDX = source.find("#flags");

		if (flagsStartIDX != std::string::npos)
		{
			size_t flagsEndIDX = source.find_first_of("\r\n", flagsStartIDX) + 1;
			size_t flagsListStartIDX = flagsStartIDX + 7;
			std::string flagsString = source.substr(flagsListStartIDX, flagsEndIDX - flagsListStartIDX);

			size_t pos = 0;
			while (((pos = flagsString.find_first_of(",\r\n")) != std::string::npos)) {
				auto flag = flagsString.substr(0, pos);

				// remove whitespace
				flag.erase(std::remove(flag.begin(), flag.end(), ' '), flag.end());

				m_ShaderFlags.insert(flag);
				flagsString.erase(0, pos + 1);
			}

		}

		std::unordered_map<GLenum, std::string> shaderSources;

		// token to split on
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);

		// find the first one
		size_t startIndexOfTypeLine = source.find(typeToken, 0);

		while (startIndexOfTypeLine != std::string::npos)
		{

			size_t lastIndexOfTypeLine = source.find_first_of("\r\n", startIndexOfTypeLine);
			ARES_CORE_ASSERT(lastIndexOfTypeLine != std::string::npos, "Syntax Error:: no characters after the #type line");

			size_t startIndexOfTypeString = startIndexOfTypeLine + typeTokenLength + 1;

			std::string typeString = source.substr(startIndexOfTypeString, lastIndexOfTypeLine - startIndexOfTypeString);
			auto shaderType = ShaderTypeFromString(typeString);
			ARES_CORE_ASSERT(shaderType, "Invalid Shader Type Specified");
			
			// get next line
			size_t shadrSourceStartIndex = source.find_first_not_of("\r\n", lastIndexOfTypeLine);
			ARES_CORE_ASSERT(shadrSourceStartIndex != std::string::npos, "Syntax error:: no shader source after #type line");

			// find the next #type line
			startIndexOfTypeLine = source.find(typeToken, shadrSourceStartIndex);
			
			// source shader code is from the next line, until either
			// the end of the file, or the next line we found #type
			//shaderSources[shaderType] = source.substr(shadrSourceStartIndex, startIndexOfTypeLine - (shadrSourceStartIndex == std::string::npos ? source.size() - 1 : shadrSourceStartIndex));

			shaderSources[shaderType] = source.substr(shadrSourceStartIndex, (startIndexOfTypeLine == std::string::npos ? source.size() : startIndexOfTypeLine) - shadrSourceStartIndex);
			//shaderSources[shaderType] = source.substr(shadrSourceStartIndex, startIndexOfTypeLine - shadrSourceStartIndex);


			// Compute shaders cannot contain other types
			if (shaderType == GL_COMPUTE_SHADER)
			{
				m_IsCompute = true;
				break;
			}
		}

		return shaderSources;
	}

	//void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	//{
	//	// Get a program object.
	//	GLuint program = glCreateProgram();

	//	ARES_CORE_ASSERT(shaderSources.size() <= 2, "Max 2 Shaders In Source");
	//	std::array<GLenum, 2> glShaderIDs;

	//	int i = 0;
	//	// Read our shaders into the appropriate buffers
	//	for (auto& kv : shaderSources)
	//	{
	//		GLenum type = kv.first;
	//		const std::string& source = kv.second;

	//		// Create an empty shader handle
	//		GLuint shader = glCreateShader(type);

	//		// Send the shader source code to GL
	//		// Note that std::string's .c_str is NULL character terminated.
	//		const GLchar* sourceCStr = source.c_str();
	//		glShaderSource(shader, 1, &sourceCStr, 0);

	//		// Compile the shader
	//		glCompileShader(shader);

	//		GLint isCompiled = 0;
	//		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	//		if (isCompiled == GL_FALSE)
	//		{
	//			// get error logs
	//			GLint maxLength = 0;
	//			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	//			// The maxLength includes the NULL character
	//			std::vector<GLchar> infoLog(maxLength);
	//			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

	//			// We don't need the shader anymore.
	//			glDeleteShader(shader);

	//			ARES_CORE_ERROR("{0}", infoLog.data());
	//			ARES_CORE_ASSERT(false, "Shader Compilation Failure!");
	//			break;
	//		}

	//		// Attach our shaders to our program
	//		glAttachShader(program, shader);

	//		glShaderIDs[i++] = shader;
	//	}


	//	// Vertex and fragment shaders are successfully compiled.
	//	// Now time to link them together into a program.

	//	// Link our program
	//	glLinkProgram(program);

	//	// Note the different functions here: glGetProgram* instead of glGetShader*.
	//	GLint isLinked = 0;
	//	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	//	if (isLinked == GL_FALSE)
	//	{
	//		// get error logs
	//		GLint maxLength = 0;
	//		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

	//		// The maxLength includes the NULL character
	//		std::vector<GLchar> infoLog(maxLength);
	//		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

	//		// We don't need the program anymore.
	//		glDeleteProgram(program);

	//		// Don't leak shaders either.
	//		for (auto id : glShaderIDs)
	//			glDeleteShader(id);


	//		// Use the infoLog as you see fit.

	//		ARES_CORE_ERROR("{0}", infoLog.data());
	//		ARES_CORE_ASSERT(false, "Shader Link Failure!");
	//		return;
	//	}

	//	// Always detach shaders after a successful link.
	//	for (auto id : glShaderIDs)
	//	{

	//		glDetachShader(program, id);
	//		glDeleteShader(id);
	//	}

	//	m_RendererID = program;

	//}

	void OpenGLShader::SetInt(const std::string& name, int value, ShaderVariant variant)
	{
		Renderer::Submit([=]() {
			UploadUniformInt(name, value, variant);
		}, "set uniform");
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, const uint32_t count, ShaderVariant variant, bool deleteFromMem)
	{
		Renderer::Submit([=]() mutable {	
			UploadUniformIntArray(name, values, count, variant);
			if (deleteFromMem)
				delete[] values;
		}, "set uniform");
	}
	void OpenGLShader::SetFloat(const std::string& name, float value, ShaderVariant variant)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat(name, value, variant);
		}, "set uniform");
	}
	void OpenGLShader::SetFloat2(const std::string& name, glm::vec2 value, ShaderVariant variant)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat2(name, value, variant);
		}, "set uniform");
	}
	void OpenGLShader::SetFloat3(const std::string& name, glm::vec3 value, ShaderVariant variant)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat3(name, value, variant);
		}, "set uniform");
	}
	void OpenGLShader::SetFloat4(const std::string& name, glm::vec4 value, ShaderVariant variant)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat4(name, value, variant);
		}, "set uniform");
	}
	void OpenGLShader::SetMat3(const std::string& name, glm::mat3 value, ShaderVariant variant)
	{
		Renderer::Submit([=]() {
			UploadUniformMat3(name, value, variant);
		}, "set uniform");
	}
	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value, ShaderVariant variant)
	{
		Renderer::Submit([=]() {
			UploadUniformMat4(name, value, variant);
		}, "set uniform");
	}

	void OpenGLShader::SetIntFromRenderThread(const std::string& name, int value, ShaderVariant variant)
	{
		UploadUniformInt(name, value, variant);
	}
	void OpenGLShader::SetIntArrayFromRenderThread(const std::string& name, int* values, uint32_t count, ShaderVariant variant)
	{
		UploadUniformIntArray(name, values, count, variant);
	}
	void OpenGLShader::SetFloatFromRenderThread(const std::string& name, float value, ShaderVariant variant)
	{
		UploadUniformFloat(name, value, variant);
	}
	void OpenGLShader::SetFloat2FromRenderThread(const std::string& name, glm::vec2 value, ShaderVariant variant)
	{
		UploadUniformFloat2(name, value, variant);
	}
	void OpenGLShader::SetFloat3FromRenderThread(const std::string& name, glm::vec3 value, ShaderVariant variant)
	{
		UploadUniformFloat3(name, value, variant);
	}
	void OpenGLShader::SetFloat4FromRenderThread(const std::string& name, glm::vec4 value, ShaderVariant variant)
	{
		UploadUniformFloat4(name, value, variant);
	}
	void OpenGLShader::SetMat3FromRenderThread(const std::string& name, glm::mat3 value, ShaderVariant variant)
	{
		UploadUniformMat3(name, value, variant);
	}
	void OpenGLShader::SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, ShaderVariant variant)
	{
		UploadUniformMat4(name, value, variant);
	}







	void OpenGLShader::SetVSMaterialUniformBuffer(Buffer buffer, ShaderVariant variant)
	{
		Renderer::Submit([this, buffer, variant]() {
			/*glUseProgram(m_RendererID);
			ResolveAndSetUniforms(m_VSMaterialUniformBuffer, buffer);*/

			glUseProgram(GetRendererID(variant));
			ResolveAndSetUniforms(m_VariantBuffers[GetVariantIndex(variant)].m_VSMaterialUniformBuffer, buffer);

		}, "SetVSMaterialUniformBuffer");
	}

	void OpenGLShader::SetPSMaterialUniformBuffer(Buffer buffer, ShaderVariant variant)
	{
		Renderer::Submit([this, buffer, variant]() {
			/*glUseProgram(m_RendererID);
			ResolveAndSetUniforms(m_PSMaterialUniformBuffer, buffer);*/
			
			glUseProgram(GetRendererID(variant));
			ResolveAndSetUniforms(m_VariantBuffers[GetVariantIndex(variant)].m_PSMaterialUniformBuffer, buffer);
		}, "SetPSMaterialUniformBuffer");
	}

	
	/*void OpenGLShader::UploadUniformBuffer(const UniformBufferBase& uniformBuffer)
	{
		for (unsigned int i = 0; i < uniformBuffer.GetUniformCount(); i++)
		{
			const UniformDecl& decl = uniformBuffer.GetUniforms()[i];
			switch (decl.Type)
			{

			

			case UniformType::Int32:
			{
				const std::string& name = decl.Name;
				int value = *(int*)(uniformBuffer.GetBuffer() + decl.Offset);
				SetInt(name, value);
				break;
			}
			case UniformType::Float:
			{
				const std::string& name = decl.Name;
				float value = *(float*)(uniformBuffer.GetBuffer() + decl.Offset);
				SetFloat(name, value);
				break;
			}
			case UniformType::Float2:
			{
				const std::string& name = decl.Name;
				glm::vec2& values = *(glm::vec2*)(uniformBuffer.GetBuffer() + decl.Offset);
				SetFloat2(name, values);
				break;
			}
			case UniformType::Float3:
			{
				const std::string& name = decl.Name;
				glm::vec3& values = *(glm::vec3*)(uniformBuffer.GetBuffer() + decl.Offset);
				SetFloat3(name, values);
				break;
			}
			case UniformType::Float4:
			{
				const std::string& name = decl.Name;
				glm::vec4& values = *(glm::vec4*)(uniformBuffer.GetBuffer() + decl.Offset);
				SetFloat4(name, values);
				break;
			}


			case UniformType::Matrix3x3:
			{
				const std::string& name = decl.Name;
				glm::mat3& values = *(glm::mat3*)(uniformBuffer.GetBuffer() + decl.Offset);
				SetMat3(name, values);
				break;
			}
			case UniformType::Matrix4x4:
			{
				const std::string& name = decl.Name;
				glm::mat4& values = *(glm::mat4*)(uniformBuffer.GetBuffer() + decl.Offset);
				SetMat4(name, values);
				break;
			}

			}
		}
	}*/

	
	void OpenGLShader::UploadUniformInt(const std::string& name, int value, ShaderVariant variant)
	{
		GLint location = GetUniformLocation(name, variant);
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count, ShaderVariant variant)
	{
		GLint location = GetUniformLocation(name, variant);
		glUniform1iv(location, count, values);
	}
	void OpenGLShader::UploadUniformFloat(const std::string& name, float value, ShaderVariant variant)
	{
		GLint location = GetUniformLocation(name, variant);
		glUniform1f(location, value);
	}
	void OpenGLShader::UploadUniformFloat2(const std::string& name, glm::vec2 value, ShaderVariant variant)
	{
		GLint location = GetUniformLocation(name, variant);
		glUniform2f(location, value.x, value.y);
	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, glm::vec3 value, ShaderVariant variant)
	{
		GLint location = GetUniformLocation(name, variant);
		glUniform3f(location, value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, glm::vec4 value, ShaderVariant variant)
	{
		GLint location = GetUniformLocation(name, variant);
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::UploadUniformMat3(const std::string& name, glm::mat3 value, ShaderVariant variant)
	{
		GLint location = GetUniformLocation(name, variant);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& value, ShaderVariant variant)
	{
		GLint location = GetUniformLocation(name, variant);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}


	void OpenGLShader::UploadUniformInt(uint32_t location, int value)
	{
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformIntArray(uint32_t location, int* values, uint32_t count)
	{
		glUniform1iv(location, count, values);
	}
	void OpenGLShader::UploadUniformFloat(uint32_t location, float value)
	{
		glUniform1f(location, value);
	}
	void OpenGLShader::UploadUniformFloat2(uint32_t location, glm::vec2 value)
	{
		glUniform2f(location, value.x, value.y);
	}
	void OpenGLShader::UploadUniformFloat3(uint32_t location, glm::vec3 value)
	{
		glUniform3f(location, value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformFloat4(uint32_t location, glm::vec4 value)
	{
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::UploadUniformMat3(uint32_t location, glm::mat3 value)
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLShader::UploadUniformMat4(uint32_t location, const glm::mat4& value)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLShader::UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count)
	{
		glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(values));
	}

	void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, byte* buffer, uint32_t offset)
	{
		const ShaderStruct& s = uniform->GetShaderUniformStruct();
		const auto& fields = s.GetFields();
		for (size_t k = 0; k < fields.size(); k++)
		{
			OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
			ResolveAndSetUniformField(*field, buffer, offset);
			offset += field->m_Size;
		}
	}

	GLint OpenGLShader::GetUniformLocation(const std::string& name, ShaderVariant variant) const
	{

		/*if (m_UniformLocationMap.find(name) != m_UniformLocationMap.end())
			return m_UniformLocationMap[name];*/
		

		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		GLint location = glGetUniformLocation(GetRendererID(variant), name.c_str());

		
		if (location == -1)
		{
			ARES_CORE_WARN("Uniform '{0}' not found in shader '{1}'!", name, m_Name);
			return location;
		}
		//m_UniformLocationMap[name] = location;
		return location;
	}
}