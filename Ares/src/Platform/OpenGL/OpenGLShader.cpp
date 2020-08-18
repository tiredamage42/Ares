
/*
	TODO:

	setup system for shader compilation like so:

	props {...}
	flags {...}

	// shared between all passes all shaders
	shared_all {...}

	// shared between all passes vertex shaders
	shared_vert {...}

	// shared between all passes fragment shaders
	shared_frag {...}

	pass forward
	{
		// shared between vertex and fragment shaders
		shared {...}
		vs/ps code...
	}
	pass deferred
	{
		// shared between vertex and fragment shaders
		shared {...}
		vs/ps code...
	}
	pass shadow
	{
		// shared between vertex and fragment shaders
		shared {...}
		vs/ps code...
	}

	since shadersources map shouldnt be on heap, just have shadersource map for each 
	pass / variation: 6 total so extra code can be inserted that step, not in compilation

	!!! MAKE SURE THAT "_ares_internal_*" UNIFORMS ARENT PICKED UP BY PARSER!!!
*/
	
#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "Ares/Core/FileUtils/FileUtils.h"
#include <glm/gtc/type_ptr.hpp>

#include "Ares/Renderer/Renderer.h"
#include "Ares/Core/StringUtils.h"
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

		m_UniformLocationMaps.resize(MAX_VARIANTS);
		m_RendererIDs.resize(MAX_VARIANTS);
		
		for (uint32_t i = 0; i < MAX_VARIANTS; i++)
		{
			m_RendererIDs[i] = 0;
		}

		Reload();
	}

	void OpenGLShader::Reload()
	{
		bool success;
		std::string source = FileUtils::GetFileContents(m_AssetPath, success);
		if (!success)
			ARES_CORE_ASSERT(false, "");
	
		// pre process [ get flags, properties, attributes, add standard variables ]
		std::unordered_map<std::string, UniformAttributes> uniformAttributes;
		bool createObject2World;
		m_ShaderSource = PreProcess(source, uniformAttributes, createObject2World);

		// clear the uniform location maps
		for (size_t i = 0; i < MAX_VARIANTS; i++)
			m_UniformLocationMaps[i].clear();
		
		if (!m_IsCompute)
			Parse(uniformAttributes);
		
		Renderer::Submit([=]() {

			for (size_t i = 0; i < MAX_VARIANTS; i++)
			{
				if (m_RendererIDs[i])
				{
					glDeleteProgram(m_RendererIDs[i]);
					m_RendererIDs[i] = 0;
				}
			}
			
			CompileAndUploadShader(createObject2World);

			if (!m_IsCompute)
			{
				for (uint32_t i = 0; i < MAX_VARIANTS; i++)
				{
					if (HasVariation(i))
					{
						ResolveUniforms(i);
					}
				}
			}

			if (HasVariation(ShaderVariations::DefaultSkinned))
				UploadUniformInt("_ares_internal_BoneSampler", Renderer::BONE_SAMPLER_TEX_SLOT, ShaderVariations::DefaultSkinned);
			if (HasVariation(ShaderVariations::DeferredSkinned))
				UploadUniformInt("_ares_internal_BoneSampler", Renderer::BONE_SAMPLER_TEX_SLOT, ShaderVariations::DeferredSkinned);


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


	OpenGLShader::~OpenGLShader()
	{
		std::vector<uint32_t> rendererIDs = m_RendererIDs;
		Renderer::Submit([rendererIDs]() {
			for (size_t i = 0; i < rendererIDs.size(); i++)
				if (rendererIDs[i])
					glDeleteProgram(rendererIDs[i]);
			
		}, "Delete Shader");
	}

	void OpenGLShader::Bind(ShaderVariations variation)
	{
		Renderer::Submit([=]() { glUseProgram(m_RendererIDs[GetVariationIndex(variation)]); }, "Bind Shader");
	}

	void OpenGLShader::Unbind() const
	{
		Renderer::Submit([]() { glUseProgram(0); }, "Unbidn Shader");
	}

	uint32_t OpenGLShader::GetRendererID(ShaderVariations variant) const
	{
		return m_RendererIDs[GetVariationIndex(variant)];
	}

	const size_t OpenGLShader::GetVariationIndex(ShaderVariations variation) const
	{
		if (!HasVariation(variation))
		{
			ARES_CORE_WARN("Shader '{0}' Doesnt Have Variation: '{1}', using default variation instead!", m_Name, variation);
			return 0;
		}
		return ShaderVariation2Int(variation);
	}



	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(std::string source, std::unordered_map<std::string, UniformAttributes>& uniformAttributes, bool& createObject2World)
	{
		createObject2World = false;

		size_t propertiesStartIDX = source.find("#properties");
		if (propertiesStartIDX != std::string::npos)
		{
			std::string propsString = StringUtils::ExtractStringFromBetweenBrackets(source, propertiesStartIDX, true);
			StringUtils::RemoveWhiteSpaceFrom(propsString);

			std::istringstream f(propsString);
			std::string s;
			while (getline(f, s, ';')) {

				if (s.find("//") == 0)
					continue;

				size_t nameEndIDX = s.find_first_of("|", 0);
				std::string name = s.substr(0, nameEndIDX);
				size_t attributes_start = s.find_first_of("[", nameEndIDX) + 1;

				if (attributes_start != std::string::npos)
				{
					size_t attributes_end = s.find_first_of("]", attributes_start);
					std::string attributes_string = s.substr(attributes_start, attributes_end - attributes_start);

					UniformAttributes attributes;
					attributes.Attributes |= (uint32_t)UniformAttribute::Public;

					std::istringstream as(attributes_string);
					std::string st;
					while (getline(as, st, ':'))
					{
						if (st == "TOGGLE")
						{
							attributes.Attributes |= (uint32_t)UniformAttribute::Toggle;
						}
						else if (st == "COLOR")
						{
							attributes.Attributes |= (uint32_t)UniformAttribute::Color;
						}
						else if (st == "BUMP")
						{
							attributes.Attributes |= (uint32_t)UniformAttribute::BumpMap;
						}
						else if (st.rfind("RANGE", 0) == 0) {

							attributes.Attributes |= (uint32_t)UniformAttribute::Range;

							std::string rangeStr = StringUtils::ExtractStringFromBetweenBrackets(st, 0, false, "()");
							size_t commaPos = rangeStr.find_first_of(",", 0);
							std::string minS = rangeStr.substr(0, commaPos);
							std::string maxS = rangeStr.substr(commaPos + 1, rangeStr.size() - (commaPos + 1));
							attributes.Range = { std::stof(minS), std::stof(maxS) };
						}
						else if (st.rfind("DEF", 0) == 0)
						{
							attributes.Attributes |= (uint32_t)UniformAttribute::DefaultValue;

							std::istringstream f2(StringUtils::ExtractStringFromBetweenBrackets(st, 0, false, "()"));

							std::vector<float> vals;
							std::string s2;
							while (getline(f2, s2, ','))
							{
								vals.push_back(std::stof(s2));
							}

							byte* value;
							size_t size;
							if (vals.size() == 1)
							{
								auto v = vals[0];
								value = (byte*)&v;
								size = sizeof(float);
							}
							else if (vals.size() == 2)
							{
								auto v = glm::vec2(vals[0], vals[1]);
								value = (byte*)&v;
								size = sizeof(glm::vec2);
							}
							else if (vals.size() == 3)
							{
								auto v = glm::vec3(vals[0], vals[1], vals[2]);
								value = (byte*)&v;
								size = sizeof(glm::vec3);
							}
							else if (vals.size() == 4)
							{
								auto v = glm::vec4(vals[0], vals[1], vals[2], vals[3]);
								value = (byte*)&v;
								size = sizeof(glm::vec4);
							}
							else
							{
								ARES_CORE_ASSERT(false, "Invalid number of default value components");
							}
							attributes.DefaultValue = new uint8_t[size];
							memcpy(attributes.DefaultValue, value, size);
						}
					}
					uniformAttributes[name] = attributes;
				}
				else
				{
					uniformAttributes[name] = {};
				}

			}
		}

		// find flags
		AddVariation(ShaderVariations::Default);

		size_t flagsStartIDX = source.find("#flags");

		if (flagsStartIDX != std::string::npos)
		{
			std::string flagsString = StringUtils::ExtractStringFromBetweenBrackets(source, flagsStartIDX, true);
			StringUtils::RemoveWhiteSpaceFrom(flagsString);

			std::istringstream f(flagsString);
			std::string s;
			while (getline(f, s, ','))
			{
				if (s == SKINNED_FLAG)
				{
					AddVariation(ShaderVariations::DefaultSkinned);
				}
				else if (s == STANDARD_VARS_FLAG)
				{
					createObject2World = true;
				}
			}
		}


		bool hasSharedBlock = false;
		std::string sharedBlock;

		size_t sharedStartIDX = source.find("#shared");
		if (sharedStartIDX != std::string::npos)
		{
			hasSharedBlock = true;
			sharedBlock = StringUtils::ExtractStringFromBetweenBrackets(source, sharedStartIDX, true);
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
			std::string shaderString = source.substr(shadrSourceStartIndex, (startIndexOfTypeLine == std::string::npos ? source.size() : startIndexOfTypeLine) - shadrSourceStartIndex);

			if (hasSharedBlock)
			{
				if (shaderType == GL_VERTEX_SHADER)
				{
					shaderString.insert(shaderString.find_first_of("\r\n") + 1, StringUtils::ReplaceInString(sharedBlock, "varying", "out"));
				}
				else if (shaderType == GL_FRAGMENT_SHADER)
				{
					shaderString.insert(shaderString.find_first_of("\r\n") + 1, StringUtils::ReplaceInString(sharedBlock, "varying", "in"));
				}
				else
				{
					shaderString.insert(shaderString.find_first_of("\r\n") + 1, sharedBlock);
				}
			}

			if (createObject2World)
			{
				if (shaderType == GL_VERTEX_SHADER)
				{
					/*
						add standard attributes to shader if specified
					*/
					shaderString.insert(
						shaderString.find_first_of("\r\n") + 1,
						R"(
							layout(location = 0) in vec3 ares_ObjectPos;
							layout(location = 1) in vec2 ares_ObjectUVs;
							layout(location = 2) in vec3 ares_ObjectNormals;
							layout(location = 3) in vec3 ares_ObjectTangents;

							uniform mat4 _ares_internal_Transform;
							uniform mat4 ares_VPMatrix;
							uniform mat4 ares_VMatrix;
						)"
					);

					// "ares_Object2World" will be defined in the compile step
					// since it depends on being the skinned variation or not
					shaderString.insert(
						shaderString.find_first_of("{", shaderString.find("void main")) + 1,
						"\nmat4 ares_MVPMatrix = ares_VPMatrix * ares_Object2World;\n"
					);
				}
				else if (shaderType == GL_FRAGMENT_SHADER)
				{
					shaderString.insert(
						shaderString.find_first_of("\r\n") + 1,
						"\nlayout(location = 0) out vec4 out_Color;\n"
					);
				}
			}

			shaderSources[shaderType] = shaderString;

			// Compute shaders cannot contain other types
			if (shaderType == GL_COMPUTE_SHADER)
			{
				m_IsCompute = true;
				break;
			}
		}
		return shaderSources;
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

	void OpenGLShader::Parse(const std::unordered_map<std::string, UniformAttributes>& uniformAttributes)
	{
		const char* token;

		m_Resources.clear();
		m_ResourceArrays.clear();
		m_PSMaterialUniformBuffer.reset();

		auto& vertexSource = m_ShaderSource[GL_VERTEX_SHADER];
		auto& fragmentSource = m_ShaderSource[GL_FRAGMENT_SHADER];

		// vertex shader
		const char* vstr = vertexSource.c_str();
		while (token = FindToken(vstr, "uniform"))
		{
			ParseUniform(GetStatement(token, &vstr), uniformAttributes);
		}

		// Fragment Shader
		const char* fstr = fragmentSource.c_str();
		while (token = FindToken(fstr, "uniform"))
		{
			ParseUniform(GetStatement(token, &fstr), uniformAttributes);
		}
	}

	static bool IsTypeStringResource(const std::string& type)
	{
		if (type == "sampler2D")		return true;
		if (type == "sampler2DMS")		return true;
		if (type == "samplerCube")		return true;
		if (type == "sampler2DShadow")	return true;
		return false;
	}

	void OpenGLShader::ParseUniform(const std::string& statement, const std::unordered_map<std::string, UniformAttributes>& uniformAttributes)
	{
		std::vector<std::string> tokens = Tokenize(statement);
		uint32_t index = 0;

		index++; // "uniform"

		std::string typeString = tokens[index++];

		std::string name = tokens[index++];

		if (StartsWith(name, "ares_"))
			return;

		// Strip ; from name if present
		if (const char* s = strstr(name.c_str(), ";"))
			name = std::string(name.c_str(), s - name.c_str());

		std::string n(name);
		int32_t count = 1;
		const char* namestr = n.c_str();

		bool isArray = false;
		if (const char* s = strstr(namestr, "["))
		{
			isArray = true;
			name = std::string(namestr, s - namestr);

			const char* end = strstr(namestr, "]");
			std::string c(s + 1, end - s);
			count = atoi(c.c_str());
		}

		if (IsTypeStringResource(typeString))
		{
			if (isArray)
			{
				bool isRepeat = false;
				for (auto u : m_ResourceArrays)
				{
					if (name == u->GetName())
					{
						isRepeat = true;
						break;
					}
				}
				if (!isRepeat)
				{
					ShaderResourceArrayDeclaration* declaration = new OpenGLShaderResourceArrayDeclaration(OpenGLShaderResourceDeclaration::StringToType(typeString), name, count);
					m_ResourceArrays.push_back(declaration);
					if (uniformAttributes.find(name) != uniformAttributes.end())
					{
						declaration->m_Attributes = uniformAttributes.at(name);
					}
				}
			}
			else
			{
				bool isRepeat = false;
				for (auto u : m_Resources)
				{
					if (name == u->GetName())
					{
						isRepeat = true;
						break;
					}
				}
				if (!isRepeat)
				{
					ShaderResourceDeclaration* declaration = new OpenGLShaderResourceDeclaration(OpenGLShaderResourceDeclaration::StringToType(typeString), name);// , count);
					m_Resources.push_back(declaration);
					if (uniformAttributes.find(name) != uniformAttributes.end())
					{
						declaration->m_Attributes = uniformAttributes.at(name);
					}
				}
			}
		}
		else
		{
			bool isRepeat = false;
			if (m_PSMaterialUniformBuffer)
			{
				for (auto u : m_PSMaterialUniformBuffer->GetUniformDeclarations())
				{
					if (name == u->GetName())
					{
						isRepeat = true;
						break;
					}
				}
			}
			if (!isRepeat)
			{

				OpenGLShaderUniformDeclaration::Type t = OpenGLShaderUniformDeclaration::StringToType(typeString);
				OpenGLShaderUniformDeclaration* declaration = nullptr;

				if (t != OpenGLShaderUniformDeclaration::Type::NONE)
				{
					declaration = new OpenGLShaderUniformDeclaration(t, name, count);
					if (uniformAttributes.find(name) != uniformAttributes.end())
					{
						declaration->m_Attributes = uniformAttributes.at(name);
					}
					if (!m_PSMaterialUniformBuffer)
						m_PSMaterialUniformBuffer.reset(new OpenGLShaderUniformBufferDeclaration(""));
					m_PSMaterialUniformBuffer->PushUniform(declaration);
				}
			}
		}
	}

	
	void OpenGLShader::CompileAndUploadShader(bool createObj2World)
	{
		
		for (uint32_t i = 0; i < MAX_VARIANTS; i++)
		{
			if (!HasVariation(i))
				continue;
			
			bool variationSkinned = i % 2 == 1;

			std::vector<GLuint> shaderRendererIDs;

			// Get a program object.
			GLuint program = glCreateProgram();

			// Read our shaders into the appropriate buffers
			for (auto& kv : m_ShaderSource)
			{
				GLenum type = kv.first;
				
				std::string source = kv.second;

				// add necessary code that is variation dependent here
				if (type == GL_VERTEX_SHADER)
				{
					if (variationSkinned)
					{
						// insert the necessary bone attributes and uniforms
						source.insert(source.find("void main") - 1, R"(
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
						)");

						// insert some code at teh beginning of the main function in order to 
						// create "ares_Object2World" as a matrix that takes the bone transform into account
						source.insert(source.find_first_of("{", source.find("void main")) + 1, R"(
							mat4 _ares_internal_bone_transform = 
								_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.x) * _ares_internal_BoneWeights.x +
								_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.y) * _ares_internal_BoneWeights.y +
								_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.z) * _ares_internal_BoneWeights.z +
								_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.w) * _ares_internal_BoneWeights.w;

							mat4 ares_Object2World = _ares_internal_Transform * _ares_internal_bone_transform;
						)");
					}
					else
					{
						// create the "ares_Object2World" matrix at the beginning of the main function
						if (createObj2World)
						{
							source.insert(
								source.find_first_of("{", source.find("void main")) + 1, 
								"\nmat4 ares_Object2World = _ares_internal_Transform;\n"
							);
						}
					}

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
		}
	}



	void OpenGLShader::ResolveUniforms(uint32_t variationIDX)
	{
	
		glUseProgram(m_RendererIDs[variationIDX]);

		const auto& decl = m_PSMaterialUniformBuffer;
		if (decl)
		{
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
				uniform->m_Locations[variationIDX] = GetUniformLocation(uniform->m_Name, variationIDX);
			}
		}

		for (size_t i = 0; i < m_Resources.size(); i++)
		{
			OpenGLShaderResourceDeclaration* resource = (OpenGLShaderResourceDeclaration*)m_Resources[i];
			int32_t location = GetUniformLocation(resource->m_Name, variationIDX);
			resource->m_TexSlot = i;
			UploadUniformInt(location, i);
		}
		for (size_t i = 0; i < m_ResourceArrays.size(); i++)
		{
			OpenGLShaderResourceArrayDeclaration* resource = (OpenGLShaderResourceArrayDeclaration*)m_ResourceArrays[i];
			int32_t location = GetUniformLocation(resource->m_Name, variationIDX);
			uint32_t count = resource->GetCount();
			int* samplers = new int[count];
			for (uint32_t s = 0; s < count; s++)
				samplers[s] = s;
			UploadUniformIntArray(location, samplers, count);
			delete[] samplers;
		}
	}

	

	// TODO: check shader should be bound before this is called
	void OpenGLShader::SetPSMaterialUniformBuffer(Buffer buffer, ShaderVariations variant)
	{
		Renderer::Submit([=]() { ResolveAndSetUniforms(m_PSMaterialUniformBuffer, buffer, variant); }, "SetPSMaterialUniformBuffer");
	}

	void OpenGLShader::ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer, ShaderVariations variation)
	{
		uint32_t variationIDX = ShaderVariation2Int(variation);

		const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
		for (size_t i = 0; i < uniforms.size(); i++)
		{
			OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[i];
			if (uniform->IsArray())
			{
				ResolveAndSetUniformArray(uniform, buffer, variationIDX);
			}
			else
			{
				ResolveAndSetUniform(uniform, buffer, variationIDX);
			}
		}
	}

	void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer, uint32_t variationIDX)
	{

		GLint location = uniform->GetLocation(variationIDX);
		uint32_t offset = uniform->GetOffset();

		switch (uniform->GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(location, *(float*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(location, *(int32_t*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(location, *(glm::vec2*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(location, *(glm::vec3*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(location, *(glm::vec4*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(location, *(glm::mat3*) & buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4(location, *(glm::mat4*) & buffer.Data[offset]);
			break;
		default:
			ARES_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}

	/*
		TODO:
			check if material setting arrays works...
	*/
	void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer, uint32_t variationIDX)
	{
		GLint location = uniform->GetLocation(variationIDX);
		uint32_t offset = uniform->GetOffset();

		switch (uniform->GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloatArray(location, (float*)&buffer.Data[offset], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformIntArray(location, (int32_t*)&buffer.Data[offset], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2Array(location, *(glm::vec2*)&buffer.Data[offset], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3Array(location, *(glm::vec3*)&buffer.Data[offset], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4Array(location, *(glm::vec4*)&buffer.Data[offset], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3Array(location, *(glm::mat3*)&buffer.Data[offset], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4Array(location, *(glm::mat4*) & buffer.Data[offset], uniform->GetCount());
			break;
		default:
			ARES_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}


#pragma region SET_UNIFORMS_WITH_NAME
#pragma region SINGLE
	void OpenGLShader::SetInt(const std::string& name, int value, ShaderVariations variant)
	{
		Renderer::Submit([=]() { UploadUniformInt(name, value, variant); }, "set uniform");
	}
	void OpenGLShader::SetFloat(const std::string& name, float value, ShaderVariations variant)
	{
		Renderer::Submit([=]() { UploadUniformFloat(name, value, variant); }, "set uniform");
	}
	void OpenGLShader::SetFloat2(const std::string& name, glm::vec2 value, ShaderVariations variant)
	{
		Renderer::Submit([=]() { UploadUniformFloat2(name, value, variant); }, "set uniform");
	}
	void OpenGLShader::SetFloat3(const std::string& name, glm::vec3 value, ShaderVariations variant)
	{
		Renderer::Submit([=]() { UploadUniformFloat3(name, value, variant); }, "set uniform");
	}
	void OpenGLShader::SetFloat4(const std::string& name, glm::vec4 value, ShaderVariations variant)
	{
		Renderer::Submit([=]() { UploadUniformFloat4(name, value, variant); }, "set uniform");
	}
	void OpenGLShader::SetMat3(const std::string& name, glm::mat3 value, ShaderVariations variant)
	{
		Renderer::Submit([=]() { UploadUniformMat3(name, value, variant); }, "set uniform");
	}
	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value, ShaderVariations variant)
	{
		Renderer::Submit([=]() { UploadUniformMat4(name, value, variant); }, "set uniform");
	}
#pragma endregion
#pragma region ARRAY
	void OpenGLShader::SetIntArray(const std::string& name, int32_t* values, const uint32_t count, ShaderVariations variation)
	{
		Renderer::Submit([=]() { UploadUniformIntArray(name, values, count, variation); }, "set uniform");
	}
	void OpenGLShader::SetFloatArray(const std::string& name, float* values, uint32_t count, ShaderVariations variation)
	{
		Renderer::Submit([=]() { UploadUniformFloatArray(name, values, count, variation); }, "set uniform");
	}
	void OpenGLShader::SetFloat2Array(const std::string& name, const glm::vec2& values, uint32_t count, ShaderVariations variation)
	{
		Renderer::Submit([=]() { UploadUniformFloat2Array(name, values, count, variation); }, "set uniform");
	}
	void OpenGLShader::SetFloat3Array(const std::string& name, const glm::vec3& values, uint32_t count, ShaderVariations variation)
	{
		Renderer::Submit([=]() { UploadUniformFloat3Array(name, values, count, variation); }, "set uniform");
	}
	void OpenGLShader::SetFloat4Array(const std::string& name, const glm::vec4& values, uint32_t count, ShaderVariations variation)
	{
		Renderer::Submit([=]() { UploadUniformFloat4Array(name, values, count, variation); }, "set uniform");
	}
	void OpenGLShader::SetMat3Array(const std::string& name, const glm::mat3& values, uint32_t count, ShaderVariations variation)
	{
		Renderer::Submit([=]() { UploadUniformMat3Array(name, values, count, variation); }, "set uniform");
	}
	void OpenGLShader::SetMat4Array(const std::string& name, const glm::mat4& values, uint32_t count, ShaderVariations variation)
	{
		Renderer::Submit([=]() { UploadUniformMat4Array(name, values, count, variation); }, "set uniform");
	}
#pragma endregion
#pragma endregion
#pragma region UPLOAD_UNIFORMS_WITH_NAME
#pragma region SINGLE
	void OpenGLShader::UploadUniformInt(const std::string& name, const int& value, ShaderVariations variant)
	{
		UploadUniformInt(GetUniformLocation(name, ShaderVariation2Int(variant)), value);
	}
	void OpenGLShader::UploadUniformFloat(const std::string& name, const float& value, ShaderVariations variant)
	{
		UploadUniformFloat(GetUniformLocation(name, ShaderVariation2Int(variant)), value);
	}
	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value, ShaderVariations variant)
	{
		UploadUniformFloat2(GetUniformLocation(name, ShaderVariation2Int(variant)), value);
	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value, ShaderVariations variant)
	{
		UploadUniformFloat3(GetUniformLocation(name, ShaderVariation2Int(variant)), value);
	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value, ShaderVariations variant)
	{
		UploadUniformFloat4(GetUniformLocation(name, ShaderVariation2Int(variant)), value);
	}
	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& value, ShaderVariations variant)
	{
		UploadUniformMat3(GetUniformLocation(name, ShaderVariation2Int(variant)), value);
	}
	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& value, ShaderVariations variant)
	{
		UploadUniformMat4(GetUniformLocation(name, ShaderVariation2Int(variant)), value);
	}
#pragma endregion
#pragma region ARRAY
	void OpenGLShader::UploadUniformIntArray(const std::string& name, const int32_t* values, uint32_t count, ShaderVariations variant)
	{
		UploadUniformIntArray(GetUniformLocation(name, ShaderVariation2Int(variant)), values, count);
	}
	void OpenGLShader::UploadUniformFloatArray(const std::string& name, const float* values, uint32_t count, ShaderVariations variant)
	{
		UploadUniformFloatArray(GetUniformLocation(name, ShaderVariation2Int(variant)), values, count);
	}
	void OpenGLShader::UploadUniformFloat2Array(const std::string& name, const glm::vec2& values, uint32_t count, ShaderVariations variant)
	{
		UploadUniformFloat2Array(GetUniformLocation(name, ShaderVariation2Int(variant)), values, count);
	}
	void OpenGLShader::UploadUniformFloat3Array(const std::string& name, const glm::vec3& values, uint32_t count, ShaderVariations variant)
	{
		UploadUniformFloat3Array(GetUniformLocation(name, ShaderVariation2Int(variant)), values, count);
	}
	void OpenGLShader::UploadUniformFloat4Array(const std::string& name, const glm::vec4& values, uint32_t count, ShaderVariations variant)
	{
		UploadUniformFloat4Array(GetUniformLocation(name, ShaderVariation2Int(variant)), values, count);
	}
	void OpenGLShader::UploadUniformMat3Array(const std::string& name, const glm::mat3& values, uint32_t count, ShaderVariations variant)
	{
		UploadUniformMat3Array(GetUniformLocation(name, ShaderVariation2Int(variant)), values, count);
	}
	void OpenGLShader::UploadUniformMat4Array(const std::string& name, const glm::mat4& values, uint32_t count, ShaderVariations variant)
	{
		UploadUniformMat4Array(GetUniformLocation(name, ShaderVariation2Int(variant)), values, count);
	}
#pragma endregion
#pragma endregion
#pragma region UPLOAD_UNIFORMS_WITH_LOCATION
#pragma region SINGLE
	void OpenGLShader::UploadUniformInt(uint32_t location, const int& value)
	{
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformFloat(uint32_t location, const float& value)
	{
		glUniform1f(location, value);
	}
	void OpenGLShader::UploadUniformFloat2(uint32_t location, const glm::vec2& value)
	{
		glUniform2f(location, value.x, value.y);
	}
	void OpenGLShader::UploadUniformFloat3(uint32_t location, const glm::vec3& value)
	{
		glUniform3f(location, value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformFloat4(uint32_t location, const glm::vec4& value)
	{
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::UploadUniformMat3(uint32_t location, const glm::mat3& value)
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLShader::UploadUniformMat4(uint32_t location, const glm::mat4& value)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
#pragma endregion
#pragma region ARRAY
	void OpenGLShader::UploadUniformIntArray(uint32_t location, const int32_t* values, uint32_t count)
	{
		glUniform1iv(location, count, values);
	}
	void OpenGLShader::UploadUniformFloatArray(uint32_t location, const float* values, uint32_t count)
	{
		glUniform1fv(location, count, values);
	}
	void OpenGLShader::UploadUniformFloat2Array(uint32_t location, const glm::vec2& values, uint32_t count)
	{
		glUniform2fv(location, count, glm::value_ptr(values));
	}
	void OpenGLShader::UploadUniformFloat3Array(uint32_t location, const glm::vec3& values, uint32_t count)
	{
		glUniform3fv(location, count, glm::value_ptr(values));
	}
	void OpenGLShader::UploadUniformFloat4Array(uint32_t location, const glm::vec4& values, uint32_t count)
	{
		glUniform4fv(location, count, glm::value_ptr(values));
	}
	void OpenGLShader::UploadUniformMat3Array(uint32_t location, const glm::mat3& values, uint32_t count)
	{
		glUniformMatrix3fv(location, count, GL_FALSE, glm::value_ptr(values));
	}
	void OpenGLShader::UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count)
	{
		glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(values));
	}
#pragma endregion
#pragma endregion

	GLint OpenGLShader::GetUniformLocation(const std::string& name, uint32_t variationIDX)
	{
		std::unordered_map<std::string, GLint>& map = m_UniformLocationMaps[variationIDX];

		if (map.find(name) != map.end())
		{
			return map.at(name);
		}

		GLint location = glGetUniformLocation(m_RendererIDs[variationIDX], name.c_str());

		if (location == -1)
		{
			ARES_CORE_WARN("Uniform '{0}' not found in shader '{1}'!", name, m_Name);
			return location;
		}

		map[name] = location;
		return location;
	}
}