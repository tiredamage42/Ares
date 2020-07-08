workspace "Ares"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Ares/vendor/GLFW/include"
IncludeDir["Glad"] = "Ares/vendor/Glad/include"
IncludeDir["ImGui"] = "Ares/vendor/imgui"
IncludeDir["glm"] = "Ares/vendor/glm"

include "Ares/vendor/GLFW"
include "Ares/vendor/Glad"
include "Ares/vendor/imgui"

project "Ares"
	location "Ares"
	kind "SharedLib"
    language "C++"
    
    staticruntime "off"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    pchheader "AresPCH.h"
    pchsource "Ares/src/AresPCH.cpp"

	files
	{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
	}
    
	includedirs
	{
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}"
    }
    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }
    
	filter "system:windows"
        cppdialect "C++17"
		systemversion "latest"
        defines 
        {
            "ARES_PLATFORM_WINDOWS",
            "ARES_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox\"")
        }

	filter "configurations:Debug"
        defines "ARES_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
        defines "ARES_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
        defines "ARES_DIST"
		runtime "Release"
        optimize "On"
        

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    
    staticruntime "off"
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
        "%{IncludeDir.glm}"
    }

    links
    {
        "Ares"
    }

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"
        defines 
        {
            "ARES_PLATFORM_WINDOWS"
        }
        -- postbuildcommands
        -- {
        --     ("IF NOT EXIST ../bin/" .. outputdir .. "/%{prj.name} mkdir ../bin/" .. outputdir .. "/%{prj.name}"),
        --     ("{COPY} ../bin/" .. outputdir .. "/Ares/Ares.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}")
        -- }
        
    filter "configurations:Debug"
        defines "ARES_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "ARES_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "ARES_DIST"
        runtime "Release"
        optimize "On"