/*

	TODO:
		in-editor shader text editor
		(try and get syntax highlighting / keyboard shortcuts)

	TODO:
		make compilation and parsing errors more robust
		keep track of them to show in editor...

	TODO: 
		add error state
		
		if in error state, we just forward all calls to an error shader
		or material just gives error shader as it's "get shader"

	TODO:
		if pass doesnt exist, if is skinned, go down one
		if still doesnt exist use error shader
*/
	
#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "Ares/Core/FileUtils/FileUtils.h"
#include <glm/gtc/type_ptr.hpp>

#include "Ares/Renderer/Renderer.h"
#include "Ares/Core/StringUtils.h"
namespace Ares 
{
	/*
		keep track of which shader is currently bound with which variation
		so we dont have to keep supplying the variation as a parameter
		since we should only be calling the set uniform functions 
		when already bound
	*/
	static OpenGLShader* s_CurrentBoundShader = nullptr;
	static ShaderVariations s_CurrentBoundVariation = ShaderVariations::None;

	const bool OpenGLShader::IsCurrentlyBound() const
	{
		return this == s_CurrentBoundShader;
	}


	static const std::string PROPERTIES_TAG = "#PROPERTIES";
	static const std::string SHARED_ALL_TAG = "#SHARED_ALL";
	static const std::string SHARED_VERT_TAG = "SHARED_VERT";
	static const std::string SHARED_FRAG_TAG = "#SHARED_FRAG";
	static const std::string SHARED_TAG = "#SHARED";
	static const std::string PASS_TAG = "#PASS";
	static const std::string END_PASS_TAG = "#END_PASS";
	static const std::string FLAGS_TAG = "#FLAGS";
	static const std::string SKINNED_FLAG = "SKINNED";
	static const std::string STANDARD_VARS_FLAG = "STANDARD_VARS";
	static const std::string LIGHTING_ON_FLAG = "LIGHTING_ON";

	
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
		std::vector<std::unordered_map<GLenum, std::string>> variant2sourceMap;

		PreProcess(source, variant2sourceMap, uniformAttributes);

		// clear the uniform location maps
		for (size_t i = 0; i < MAX_VARIANTS; i++)
			m_UniformLocationMaps[i].clear();
		
		if (!m_IsCompute)
		{
			std::unordered_map<GLenum, std::string> sourceMap;
			for (uint32_t i = 0; i < MAX_VARIANTS; i++)
			{
				if (HasVariation(i))
				{
					sourceMap = variant2sourceMap[i];
					break;
				}
			}
			Parse(uniformAttributes, sourceMap);
		}
		
		Renderer::Submit([=]() {

			for (size_t i = 0; i < MAX_VARIANTS; i++)
			{
				if (m_RendererIDs[i])
				{
					glDeleteProgram(m_RendererIDs[i]);
					m_RendererIDs[i] = 0;
				}
			}
			
			CompileAndUploadShader(variant2sourceMap);
			
			if (!m_IsCompute)
			{
				for (uint32_t i = 0; i < MAX_VARIANTS; i++)
				{
					if (HasVariation(i))
					{
						ResolveUniforms(i);

						// set Ares specific default values for textures
						// but dont add these tot the uniform map
						
						// odd variations are the skinned variations

						GLint location = -1;
						if (i % 2 == 1)
						{
							location = glGetUniformLocation(m_RendererIDs[i], "_ares_internal_BoneSampler");
							if (location != -1)	UploadUniformInt(location, Renderer::BONE_SAMPLER_TEX_SLOT);
						}

						location = glGetUniformLocation(m_RendererIDs[i], "ares_BRDFLUT");
						if (location != -1)	UploadUniformInt(location, Renderer::BRDF_LUT_TEX_SLOT);
						
						location = glGetUniformLocation(m_RendererIDs[i], "ares_EnvironmentCube");
						if (location != -1)	UploadUniformInt(location, Renderer::ENVIRONMENT_CUBE_TEX_SLOT);

						location = glGetUniformLocation(m_RendererIDs[i], "ares_EnvironmentIrradianceCube");
						if (location != -1)	UploadUniformInt(location, Renderer::ENVIRONMENT_IRRADIANCE_TEX_SLOT);
					}
				}
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

	OpenGLShader::~OpenGLShader()
	{
		if (IsCurrentlyBound())
		{
			s_CurrentBoundShader = nullptr;
		}
		std::vector<uint32_t> rendererIDs = m_RendererIDs;
		Renderer::Submit([rendererIDs]() {
			for (size_t i = 0; i < rendererIDs.size(); i++)
				if (rendererIDs[i])
					glDeleteProgram(rendererIDs[i]);
			
		}, "Delete Shader");
	}

	void OpenGLShader::Bind(ShaderVariations variation)
	{
		s_CurrentBoundShader = this;
		s_CurrentBoundVariation = variation;
		Renderer::Submit([=]() { glUseProgram(m_RendererIDs[GetVariationIndex(variation)]); }, "Bind Shader");
	}

	void OpenGLShader::Unbind() const
	{
		s_CurrentBoundShader = nullptr;
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

	static void ParseProperties(std::string& source, std::unordered_map<std::string, UniformAttributes>& uniformAttributes)
	{
		size_t propertiesStartIDX = source.find(PROPERTIES_TAG);
		if (propertiesStartIDX != std::string::npos)
		{
			std::string propsString = StringUtils::ExtractStringFromBetweenBrackets(source, propertiesStartIDX, true);
			StringUtils::RemoveWhiteSpaceFrom(propsString);

			std::istringstream f(propsString);
			std::string s;
			while (getline(f, s, ';')) {

				size_t nameEndIDX = s.find_first_of("[", 0);
				
				std::string name;
				if (nameEndIDX == std::string::npos)
				{
					name = s;
					uniformAttributes[name] = {};
				}
				else
				{
					name = s.substr(0, nameEndIDX);
					size_t attributes_start = nameEndIDX + 1;
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
							std::string s2;
							glm::vec4 v{ 0 };
							size_t i = 0;
							while (getline(f2, s2, ','))
							{
								if (i >= 4)
								{
									i++;
									break;
								}
								v[i++] = std::stof(s2);
							}
							
							if (i == 0 || i > 5)
							{
								ARES_CORE_ASSERT(false, "Invalid number of default value components in attribute for uniform '{0}'", name);
							}

							size_t size = sizeof(float) * i;
							attributes.DefaultValue = new byte[size];
							memcpy(attributes.DefaultValue, (byte*)&v, size);
						}
					}
					uniformAttributes[name] = attributes;
				}
			}
		}
	}

	static int32_t String2PassIndex(const std::string& pass)
	{
		if (pass == "DEFAULT")
			return 0;
		else if (pass == "FWD_BASE")
			return 2;
		else if (pass == "FWD_ADD")
			return 4;
		else if (pass == "DEFERRED")
			return 6;
		else if (pass == "SHADOW")
			return 8;
		// error, unknown pass name
		return -1;
	}
	

	static std::string ExtractSharedBlock(std::string& source, const std::string& blockTag)
	{
		size_t startPos = source.find(blockTag);
		if (startPos == std::string::npos)
			return "";

		size_t blockStart = source.find_first_of("\n\r", startPos);
		size_t blockEnd = source.find(blockTag, blockStart);

		std::string block = source.substr(blockStart, blockEnd - blockStart);

		source.erase(startPos, (blockEnd + blockTag.size()) - startPos);

		return block;
	}

	void OpenGLShader::PreProcess(std::string fileSource, std::vector<std::unordered_map<GLenum, std::string>>& variant2Source, std::unordered_map<std::string, UniformAttributes>& uniformAttributes)//, bool& createObject2World)
	{
		fileSource = StringUtils::RemoveCommentsFrom(fileSource);
		
		ParseProperties(fileSource, uniformAttributes);

		size_t flagsStartIDX = fileSource.find(FLAGS_TAG);
		std::unordered_set<std::string> flags;
		if (flagsStartIDX != std::string::npos)
		{
			std::string flagsString = StringUtils::ExtractStringFromBetweenBrackets(fileSource, flagsStartIDX, true);
			StringUtils::RemoveWhiteSpaceFrom(flagsString);

			std::istringstream f(flagsString);
			std::string s;
			while (getline(f, s, ','))
			{
				flags.insert(s);
			}
		}

		m_IsLit = flags.count(LIGHTING_ON_FLAG);

		// code shared between all passes in both shaders
		std::string passSharedAll = ExtractSharedBlock(fileSource, SHARED_ALL_TAG);
		// code shared betweel all passes in vertex shader
		std::string passSharedVert = ExtractSharedBlock(fileSource, SHARED_VERT_TAG);
		// code shared betweel all passes in fragment shader
		std::string passSharedFrag = ExtractSharedBlock(fileSource, SHARED_FRAG_TAG);
		
		// Passes:
		
		std::string passBlocks[MAX_VARIANTS];

		// cehck for passes
		size_t passStartIDX = fileSource.find(PASS_TAG);

		// no passes specified, it's an unlit shader
		if (passStartIDX == std::string::npos)
		{
			// assert that there is no shared_all, shared_vert, and shared_frag blocks
			// warn that they should just use normal shared
			AddVariation(0);
			passBlocks[0] = fileSource;
			if (flags.count(SKINNED_FLAG))
			{
				AddVariation(1);
				passBlocks[1] = fileSource;
			}
			
		}
		else
		{
			while (passStartIDX != std::string::npos)
			{
				size_t lastIndexOfPassLine = fileSource.find_first_of("\r\n", passStartIDX);
				size_t startIndexOfPassString = passStartIDX + 5 + 1;

				std::string passString = fileSource.substr(startIndexOfPassString, lastIndexOfPassLine - startIndexOfPassString);
				
				size_t passBlockEnd = fileSource.find(END_PASS_TAG, lastIndexOfPassLine);
				
				int pass = String2PassIndex(passString);
				if (pass != -1)
				{
					std::string passBlock = fileSource.substr(lastIndexOfPassLine, passBlockEnd - lastIndexOfPassLine);
					AddVariation(pass);
					passBlocks[pass] = passBlock;
					if (flags.count(SKINNED_FLAG))
					{
						AddVariation(pass + 1);
						passBlocks[pass + 1] = passBlock;
					}
				}
				else
				{
					// error unknown pass warning

				}
				fileSource.erase(passStartIDX, (passBlockEnd + END_PASS_TAG.size()) - passStartIDX);

				passStartIDX = fileSource.find(PASS_TAG, lastIndexOfPassLine);
			}
		}

		/*
		if (flags.count(LIGHTING_ON_FLAG))
		{
			// if no deferred give warning
			// if no forward base give warning
			// if no forward add give warning
		}
		*/


		variant2Source.resize(MAX_VARIANTS);

		bool addStandardVariables = flags.count(STANDARD_VARS_FLAG) || flags.count(SKINNED_FLAG);

		for (uint32_t i = 0; i < MAX_VARIANTS; i++)
		{
			if (!HasVariation(i))
				continue;

			std::string passSource = passBlocks[i];
			std::string shared = ExtractSharedBlock(passSource, SHARED_TAG);

			std::unordered_map<GLenum, std::string> sources;

			// token to split on
			const char* typeToken = "#type";
			size_t typeTokenLength = strlen(typeToken);

			// find the first one
			size_t startIndexOfTypeLine = passSource.find(typeToken, 0);

			while (startIndexOfTypeLine != std::string::npos)
			{
				size_t lastIndexOfTypeLine = passSource.find_first_of("\r\n", startIndexOfTypeLine);
				
				size_t startIndexOfTypeString = startIndexOfTypeLine + typeTokenLength + 1;

				std::string typeString = passSource.substr(startIndexOfTypeString, lastIndexOfTypeLine - startIndexOfTypeString);
				auto shaderType = ShaderTypeFromString(typeString);
				if (!shaderType)
				{
					// "Invalid Shader Type Specified"
				}
				
				// get next line
				size_t shadrSourceStartIndex = passSource.find_first_not_of("\r\n", lastIndexOfTypeLine);
				ARES_CORE_ASSERT(shadrSourceStartIndex != std::string::npos, "Syntax error:: no shader source after #type line");

				// find the next #type line
				startIndexOfTypeLine = passSource.find(typeToken, shadrSourceStartIndex);

				// source shader code is from the next line, until either
				// the end of the file, or the next line we found #type
				std::string shaderString = passSource.substr(shadrSourceStartIndex, (startIndexOfTypeLine == std::string::npos ? passSource.size() : startIndexOfTypeLine) - shadrSourceStartIndex);

				// add local shared block
				if (shaderType == GL_VERTEX_SHADER)
				{
					shaderString.insert(shaderString.find_first_of("\r\n") + 1, StringUtils::ReplaceInString(shared, "varying", "out"));
					shaderString.insert(shaderString.find_first_of("\r\n") + 1, StringUtils::ReplaceInString(passSharedVert, "varying", "out"));
					shaderString.insert(shaderString.find_first_of("\r\n") + 1, StringUtils::ReplaceInString(passSharedAll, "varying", "out"));
				}
				else if (shaderType == GL_FRAGMENT_SHADER)
				{
					shaderString.insert(shaderString.find_first_of("\r\n") + 1, StringUtils::ReplaceInString(shared, "varying", "in"));
					shaderString.insert(shaderString.find_first_of("\r\n") + 1, StringUtils::ReplaceInString(passSharedFrag, "varying", "in"));
					shaderString.insert(shaderString.find_first_of("\r\n") + 1, StringUtils::ReplaceInString(passSharedAll, "varying", "in"));
				}

				if (addStandardVariables)
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

						// "ares_Object2World" will be defined in the next step
						// since it depends on being the skinned variation or not
						shaderString.insert(
							shaderString.find_first_of("{", shaderString.find("void main")) + 1,
							"\nmat4 ares_MVPMatrix = ares_VPMatrix * ares_Object2World;\n"
						);

						// skinned
						if (i % 2 == 1)
						{
							// insert the necessary bone attributes and uniforms
							shaderString.insert(
								shaderString.find("void main") - 1, 
								R"(
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
								)"
							);
							// insert some code at teh beginning of the main function in order to 
							// create "ares_Object2World" as a matrix that takes the bone transform into account
							shaderString.insert(
								shaderString.find_first_of("{", shaderString.find("void main")) + 1, 
								R"(
									mat4 _ares_internal_bone_transform = 
										_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.x) * _ares_internal_BoneWeights.x +
										_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.y) * _ares_internal_BoneWeights.y +
										_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.z) * _ares_internal_BoneWeights.z +
										_ares_internal_GetBoneMatrix(_ares_internal_BoneIndices.w) * _ares_internal_BoneWeights.w;

									mat4 ares_Object2World = _ares_internal_Transform * _ares_internal_bone_transform;
								)"
							);
						}
						else
						{
							shaderString.insert(
								shaderString.find_first_of("{", shaderString.find("void main")) + 1,
								"\nmat4 ares_Object2World = _ares_internal_Transform;\n"
							);
						}

					}
					else if (shaderType == GL_FRAGMENT_SHADER)
					{
						/*
							TODO:
							ONLY IF NOT DEFERRED
						*/
						shaderString.insert(
							shaderString.find_first_of("\r\n") + 1,
							"\nlayout(location = 0) out vec4 out_Color;\n"
						);
					}
				}

				sources[shaderType] = shaderString;

				// Compute shaders cannot contain other types
				if (shaderType == GL_COMPUTE_SHADER)
				{
					m_IsCompute = true;
					break;
				}
			}

			variant2Source[i] = sources;
		}
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

	void OpenGLShader::Parse(const std::unordered_map<std::string, UniformAttributes>& uniformAttributes, std::unordered_map<GLenum, std::string>& sourceMap)
	{
		const char* token;

		m_Resources.clear();
		m_ResourceArrays.clear();
		m_PSMaterialUniformBuffer.reset();

		auto& vertexSource = sourceMap[GL_VERTEX_SHADER];
		auto& fragmentSource = sourceMap[GL_FRAGMENT_SHADER];

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
		if (StartsWith(name, "_ares_internal"))
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

	void OpenGLShader::CompileAndUploadShader(std::vector<std::unordered_map<GLenum, std::string>> variant2sourceMap)//bool createObj2World)
	{
		
		for (uint32_t i = 0; i < MAX_VARIANTS; i++)
		{
			if (!HasVariation(i))
				continue;
			
			std::vector<GLuint> shaderRendererIDs;

			// Get a program object.
			GLuint program = glCreateProgram();

			// Read our shaders into the appropriate buffers
			for (auto& kv : variant2sourceMap[i])
			{
				GLenum type = kv.first;
				
				std::string source = kv.second;

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
				uniform->m_Locations[variationIDX] = glGetUniformLocation(m_RendererIDs[variationIDX], uniform->m_Name.c_str());
			}
		}
		for (size_t i = 0; i < m_Resources.size(); i++)
		{
			OpenGLShaderResourceDeclaration* resource = (OpenGLShaderResourceDeclaration*)m_Resources[i];
			resource->m_TexSlot = i;
			UploadUniformInt(glGetUniformLocation(m_RendererIDs[variationIDX], resource->m_Name.c_str()), i);
		}
		for (size_t i = 0; i < m_ResourceArrays.size(); i++)
		{
			OpenGLShaderResourceArrayDeclaration* resource = (OpenGLShaderResourceArrayDeclaration*)m_ResourceArrays[i];
			uint32_t count = resource->GetCount();
			int* samplers = new int[count];
			for (uint32_t s = 0; s < count; s++)
				samplers[s] = s;
			UploadUniformIntArray(glGetUniformLocation(m_RendererIDs[variationIDX], resource->m_Name.c_str()), samplers, count);
			delete[] samplers;
		}
	}

	const uint32_t OpenGLShader::CheckBoundAndGetVariationIDX() const
	{
		if (!IsCurrentlyBound())
		{
			ARES_CORE_ERROR("Trying to set uniform on Shader: '{0}' when it's not bound!", m_Name);
			return -1;
		}
		return ShaderVariation2Int(s_CurrentBoundVariation);
	}
#define CHECK_BOUND_AND_GET_VARIATION_IDX() \
	uint32_t variationIDX = CheckBoundAndGetVariationIDX(); \
	if (variationIDX == -1)	return;


	void OpenGLShader::SetPSMaterialUniformBuffer(Buffer buffer)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { ResolveAndSetUniforms(buffer, variationIDX); }, "SetPSMaterialUniformBuffer");
	}

	void OpenGLShader::ResolveAndSetUniforms(Buffer buffer, uint32_t variationIDX)
	{
		const ShaderUniformList& uniforms = m_PSMaterialUniformBuffer->GetUniformDeclarations();
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
		switch (uniform->GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(uniform->GetLocation(variationIDX), *(float*)&buffer.Data[uniform->GetOffset()]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(uniform->GetLocation(variationIDX), *(int32_t*)&buffer.Data[uniform->GetOffset()]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(uniform->GetLocation(variationIDX), *(glm::vec2*) & buffer.Data[uniform->GetOffset()]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(uniform->GetLocation(variationIDX), *(glm::vec3*) & buffer.Data[uniform->GetOffset()]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(uniform->GetLocation(variationIDX), *(glm::vec4*) & buffer.Data[uniform->GetOffset()]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(uniform->GetLocation(variationIDX), *(glm::mat3*) & buffer.Data[uniform->GetOffset()]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4(uniform->GetLocation(variationIDX), *(glm::mat4*) & buffer.Data[uniform->GetOffset()]);
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
		switch (uniform->GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloatArray(uniform->GetLocation(variationIDX), (float*)&buffer.Data[uniform->GetOffset()], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformIntArray(uniform->GetLocation(variationIDX), (int32_t*)&buffer.Data[uniform->GetOffset()], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2Array(uniform->GetLocation(variationIDX), *(glm::vec2*)&buffer.Data[uniform->GetOffset()], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3Array(uniform->GetLocation(variationIDX), *(glm::vec3*)&buffer.Data[uniform->GetOffset()], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4Array(uniform->GetLocation(variationIDX), *(glm::vec4*)&buffer.Data[uniform->GetOffset()], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3Array(uniform->GetLocation(variationIDX), *(glm::mat3*)&buffer.Data[uniform->GetOffset()], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4Array(uniform->GetLocation(variationIDX), *(glm::mat4*)&buffer.Data[uniform->GetOffset()], uniform->GetCount());
			break;
		default:
			ARES_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}


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
#pragma region SET_UNIFORMS_WITH_NAME
#pragma region SINGLE
	void OpenGLShader::SetInt(const std::string& name, const int& value)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformInt(name, value, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetFloat(const std::string& name, const float& value)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformFloat(name, value, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformFloat2(name, value, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformFloat3(name, value, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformFloat4(name, value, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& value)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformMat3(name, value, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformMat4(name, value, variationIDX); }, "set uniform");
	}
#pragma endregion
#pragma region ARRAY
	void OpenGLShader::SetIntArray(const std::string& name, const int32_t* values, const uint32_t count)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformIntArray(name, values, count, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetFloatArray(const std::string& name, const float* values, uint32_t count)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformFloatArray(name, values, count, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetFloat2Array(const std::string& name, const glm::vec2& values, uint32_t count)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformFloat2Array(name, values, count, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetFloat3Array(const std::string& name, const glm::vec3& values, uint32_t count)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformFloat3Array(name, values, count, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetFloat4Array(const std::string& name, const glm::vec4& values, uint32_t count)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformFloat4Array(name, values, count, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetMat3Array(const std::string& name, const glm::mat3& values, uint32_t count)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformMat3Array(name, values, count, variationIDX); }, "set uniform");
	}
	void OpenGLShader::SetMat4Array(const std::string& name, const glm::mat4& values, uint32_t count)
	{
		CHECK_BOUND_AND_GET_VARIATION_IDX();
		Renderer::Submit([=]() { UploadUniformMat4Array(name, values, count, variationIDX); }, "set uniform");
	}
#pragma endregion
#pragma endregion
#pragma region UPLOAD_UNIFORMS_WITH_NAME
#pragma region SINGLE
	void OpenGLShader::UploadUniformInt(const std::string& name, const int& value, uint32_t variationIDX)
	{
		UploadUniformInt(GetUniformLocation(name, variationIDX), value);
	}
	void OpenGLShader::UploadUniformFloat(const std::string& name, const float& value, uint32_t variationIDX)
	{
		UploadUniformFloat(GetUniformLocation(name, variationIDX), value);
	}
	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value, uint32_t variationIDX)
	{
		UploadUniformFloat2(GetUniformLocation(name, variationIDX), value);
	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value, uint32_t variationIDX)
	{
		UploadUniformFloat3(GetUniformLocation(name, variationIDX), value);
	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value, uint32_t variationIDX)
	{
		UploadUniformFloat4(GetUniformLocation(name, variationIDX), value);
	}
	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& value, uint32_t variationIDX)
	{
		UploadUniformMat3(GetUniformLocation(name, variationIDX), value);
	}
	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& value, uint32_t variationIDX)
	{
		UploadUniformMat4(GetUniformLocation(name, variationIDX), value);
	}
#pragma endregion
#pragma region ARRAY
	void OpenGLShader::UploadUniformIntArray(const std::string& name, const int32_t* values, uint32_t count, uint32_t variationIDX)
	{
		UploadUniformIntArray(GetUniformLocation(name, variationIDX), values, count);
	}
	void OpenGLShader::UploadUniformFloatArray(const std::string& name, const float* values, uint32_t count, uint32_t variationIDX)
	{
		UploadUniformFloatArray(GetUniformLocation(name, variationIDX), values, count);
	}
	void OpenGLShader::UploadUniformFloat2Array(const std::string& name, const glm::vec2& values, uint32_t count, uint32_t variationIDX)
	{
		UploadUniformFloat2Array(GetUniformLocation(name, variationIDX), values, count);
	}
	void OpenGLShader::UploadUniformFloat3Array(const std::string& name, const glm::vec3& values, uint32_t count, uint32_t variationIDX)
	{
		UploadUniformFloat3Array(GetUniformLocation(name, variationIDX), values, count);
	}
	void OpenGLShader::UploadUniformFloat4Array(const std::string& name, const glm::vec4& values, uint32_t count, uint32_t variationIDX)
	{
		UploadUniformFloat4Array(GetUniformLocation(name, variationIDX), values, count);
	}
	void OpenGLShader::UploadUniformMat3Array(const std::string& name, const glm::mat3& values, uint32_t count, uint32_t variationIDX)
	{
		UploadUniformMat3Array(GetUniformLocation(name, variationIDX), values, count);
	}
	void OpenGLShader::UploadUniformMat4Array(const std::string& name, const glm::mat4& values, uint32_t count, uint32_t variationIDX)
	{
		UploadUniformMat4Array(GetUniformLocation(name, variationIDX), values, count);
	}
#pragma endregion
#pragma endregion
#pragma region UPLOAD_UNIFORMS_WITH_LOCATION
#pragma region SINGLE
	void OpenGLShader::UploadUniformInt(uint32_t location, const int& value)
	{
		if (location == -1) return;
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformFloat(uint32_t location, const float& value)
	{
		if (location == -1) return;
		glUniform1f(location, value);
	}
	void OpenGLShader::UploadUniformFloat2(uint32_t location, const glm::vec2& value)
	{
		if (location == -1) return;
		glUniform2f(location, value.x, value.y);
	}
	void OpenGLShader::UploadUniformFloat3(uint32_t location, const glm::vec3& value)
	{
		if (location == -1) return;
		glUniform3f(location, value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformFloat4(uint32_t location, const glm::vec4& value)
	{
		if (location == -1) return;
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::UploadUniformMat3(uint32_t location, const glm::mat3& value)
	{
		if (location == -1) return;
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLShader::UploadUniformMat4(uint32_t location, const glm::mat4& value)
	{
		if (location == -1) return;
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
#pragma endregion
#pragma region ARRAY
	void OpenGLShader::UploadUniformIntArray(uint32_t location, const int32_t* values, uint32_t count)
	{
		if (location == -1) return;
		glUniform1iv(location, count, values);
	}
	void OpenGLShader::UploadUniformFloatArray(uint32_t location, const float* values, uint32_t count)
	{
		if (location == -1) return;
		glUniform1fv(location, count, values);
	}
	void OpenGLShader::UploadUniformFloat2Array(uint32_t location, const glm::vec2& values, uint32_t count)
	{
		if (location == -1) return;
		glUniform2fv(location, count, glm::value_ptr(values));
	}
	void OpenGLShader::UploadUniformFloat3Array(uint32_t location, const glm::vec3& values, uint32_t count)
	{
		if (location == -1) return;
		glUniform3fv(location, count, glm::value_ptr(values));
	}
	void OpenGLShader::UploadUniformFloat4Array(uint32_t location, const glm::vec4& values, uint32_t count)
	{
		if (location == -1) return;
		glUniform4fv(location, count, glm::value_ptr(values));
	}
	void OpenGLShader::UploadUniformMat3Array(uint32_t location, const glm::mat3& values, uint32_t count)
	{
		if (location == -1) return;
		glUniformMatrix3fv(location, count, GL_FALSE, glm::value_ptr(values));
	}
	void OpenGLShader::UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count)
	{
		if (location == -1) return;
		glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(values));
	}
#pragma endregion
#pragma endregion

	
}