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

project "Ares"
	location "Ares"
	kind "SharedLib"
    language "C++"
    
    staticruntime "off"
    
	targetdir ("_bin/" .. outputdir .. "/%{prj.name}")
	objdir ("_bin-int/" .. outputdir .. "/%{prj.name}")
    
	files
	{
        "%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
    
	includedirs
	{
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
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
            ("{COPY} %{cfg.buildtarget.relpath} \"../_bin/" .. outputdir .. "/Sandbox\"")
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
    targetdir ("_bin/" .. outputdir .. "/%{prj.name}")
    objdir ("_bin-int/" .. outputdir .. "/%{prj.name}")

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