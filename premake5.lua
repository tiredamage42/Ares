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

include "Ares/vendor/GLFW"

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
	}
    
	includedirs
	{
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}"
    }
    links
    {
        "GLFW",
        "opengl32.lib"
    }
    
	filter "system:windows"
        cppdialect "C++17"
		systemversion "latest"
        defines 
        {
            "ARES_PLATFORM_WINDOWS",
            "ARES_BUILD_DLL"
        }
        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox\"")
        }

	filter "configurations:Debug"
        defines "ARES_DEBUG"
        buildoptions "/MDd"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
        defines "ARES_RELEASE"
        buildoptions "/MD"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
        defines "ARES_DIST"
        buildoptions "/MD"
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
        "Ares/src"
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
        
    filter "configurations:Debug"
        defines "ARES_DEBUG"
        buildoptions "/MDd"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "ARES_RELEASE"
        buildoptions "/MD"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "ARES_DIST"
        buildoptions "/MD"
        runtime "Release"
        optimize "On"