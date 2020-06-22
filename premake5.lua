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
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
	files
	{
        "%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
    
	includedirs
	{
        "%{prj.name}/vendor/spdlog/include",
	}
    
	filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
		systemversion "latest"
        defines 
        {
            "A_PLATFORM_WINDOWS",
            "A_BUILD_DLL"
        }
        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }

	filter "configurations:Debug"
		defines "A_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "A_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "A_DIST"
		runtime "Release"
        optimize "On"
        

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    
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
        staticruntime "On"
        systemversion "latest"
        defines 
        {
            "A_PLATFORM_WINDOWS"
        }
        
    filter "configurations:Debug"
        defines "A_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "A_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "A_DIST"
        runtime "Release"
        optimize "On"