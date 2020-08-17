workspace "Ares"
	architecture "x86_64"
	startproject "Phobos"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
    }
    flags
	{
		"MultiProcessorCompile"
	}
	

FBX_SDK_INSTALL_DIR = "C:/Program Files/Autodesk/FBX/FBX SDK/2019.5/";

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Ares/vendor/GLFW/include"
IncludeDir["Glad"] = "Ares/vendor/Glad/include"
IncludeDir["ImGui"] = "Ares/vendor/imgui"
IncludeDir["glm"] = "Ares/vendor/glm"
IncludeDir["stb_image"] = "Ares/vendor/stb_image"
IncludeDir["entt"] = "Ares/vendor/entt"
IncludeDir["FastNoise"] = "Ares/vendor/FastNoise"
IncludeDir["json"] = "Ares/vendor/nlohmann-json"
IncludeDir["assimp"] = "Ares/vendor/assimp/include"

IncludeDir["FBXSDK"] = FBX_SDK_INSTALL_DIR .. "include"


group "Dependencies"
    include "Ares/vendor/GLFW"
    include "Ares/vendor/Glad"
    include "Ares/vendor/imgui"
group ""

project "Ares"
	location "Ares"
	kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    pchheader "AresPCH.h"
    pchsource "Ares/src/AresPCH.cpp"

	files
	{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp",
        
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",

        "%{prj.name}/vendor/FastNoise/**.cpp",

        "%{prj.name}/vendor/yaml-cpp/src/**.cpp",
		"%{prj.name}/vendor/yaml-cpp/src/**.h",
		"%{prj.name}/vendor/yaml-cpp/include/**.h"

    }
    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }
    
	includedirs
	{
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.json}",
        "%{IncludeDir.FastNoise}",
        "%{IncludeDir.FBXSDK}",
        "%{IncludeDir.assimp}",
        "%{prj.name}/vendor/yaml-cpp/include"
    }
        
    links
    {
        -- fbx sdk
        "zlib-mt.lib",
        "libxml2-mt.lib",
        "libfbxsdk-mt.lib",

        -- glfw
        "GLFW",

        -- glad
        "Glad",

        -- imgui
        "ImGui",
        
        -- opengl
        "opengl32.lib"
    }
    filter "files:Ares/vendor/FastNoise/**.cpp or files:Ares/vendor/yaml-cpp/src/**.cpp"
       	flags { "NoPCH" }

    
	filter "system:windows"
        systemversion "latest"
        
	filter "configurations:Debug"
        defines "ARES_DEBUG"
		runtime "Debug"
        symbols "on"
        libdirs
        {
            FBX_SDK_INSTALL_DIR .. "lib\\vs2017\\x64\\debug"
        }
	filter "configurations:Release"
        defines "ARES_RELEASE"
		runtime "Release"
        optimize "on"
        libdirs
        {
            FBX_SDK_INSTALL_DIR .. "lib\\vs2017\\x64\\release"
        }
	filter "configurations:Dist"
        defines "ARES_DIST"
		runtime "Release"
        optimize "on"
        libdirs
        {
            FBX_SDK_INSTALL_DIR .. "lib\\vs2017\\x64\\release"
        }
        

project "Phobos"
    location "Phobos"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    debugdir ("bin/" .. outputdir .. "/%{prj.name}")

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Ares/vendor/spdlog/include",
        "Ares/src",
        "Ares/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.json}"
    }

    links
    {
        "Ares"
    } 

    postbuildcommands 
	{
        '{COPY} "../Phobos/Assets" "%{cfg.targetdir}/Assets"',
        '{COPY} "../Phobos/EditorResources" "%{cfg.targetdir}/EditorResources"',
        '{COPY} "../Phobos/imgui.ini" "%{cfg.targetdir}"'
	}
	
    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
        defines "ARES_DEBUG"
        runtime "Debug"
        symbols "on"
        links
		{
			"Ares/vendor/assimp/lib/Debug/assimp-vc141-mtd.lib"
		}
		postbuildcommands 
		{
			'{COPY} "../Ares/vendor/assimp/lib/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"',
		}

    filter "configurations:Release"
        defines "ARES_RELEASE"
        runtime "Release"
        optimize "on"
        links
		{
			"Ares/vendor/assimp/lib/Release/assimp-vc141-mt.lib"
		}
		postbuildcommands 
		{
			'{COPY} "../Ares/vendor/assimp/lib/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
		}

    filter "configurations:Dist"
        defines "ARES_DIST"
        runtime "Release"
        optimize "on"
        links
		{
			"Ares/vendor/assimp/lib/Release/assimp-vc141-mt.lib"
		}
		postbuildcommands 
		{
			'{COPY} "../Ares/vendor/assimp/lib/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
		}