workspace "Ares"
	architecture "x86_64"
	startproject "Sandbox"

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
	

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Ares/vendor/GLFW/include"
IncludeDir["Glad"] = "Ares/vendor/Glad/include"
IncludeDir["ImGui"] = "Ares/vendor/imgui"
IncludeDir["glm"] = "Ares/vendor/glm"
IncludeDir["stb_image"] = "Ares/vendor/stb_image"

include "Ares/vendor/GLFW"
include "Ares/vendor/Glad"
include "Ares/vendor/imgui"

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
        "%{prj.name}/vendor/glm/glm/**.inl"
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
        "%{IncludeDir.stb_image}"
    }
    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }
    
	filter "system:windows"
        systemversion "latest"
        defines 
        {
        }

	filter "configurations:Debug"
        defines "ARES_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
        defines "ARES_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
        defines "ARES_DIST"
		runtime "Release"
        optimize "on"
        

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
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
        "%{IncludeDir.glm}"
    }

    links
    {
        "Ares"
    } 

    filter "system:windows"
        systemversion "latest"
        
        
    filter "configurations:Debug"
        defines "ARES_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ARES_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "ARES_DIST"
        runtime "Release"
        optimize "on"