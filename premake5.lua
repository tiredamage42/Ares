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
	

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Ares/vendor/GLFW/include"
IncludeDir["Glad"] = "Ares/vendor/Glad/include"
IncludeDir["ImGui"] = "Ares/vendor/imgui"
IncludeDir["glm"] = "Ares/vendor/glm"
IncludeDir["stb_image"] = "Ares/vendor/stb_image"
IncludeDir["entt"] = "Ares/vendor/entt/include"
IncludeDir['FastNoise'] = "Ares/vendor/FastNoise"
-- IncludeDir["assimp"] = "Ares/vendor/assimp/include"


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
        "%{IncludeDir.FastNoise}",
        "%{prj.name}/vendor/assimp/include",
        "%{prj.name}/vendor/yaml-cpp/include"
        -- "%{IncludeDir.assimp}"
        
    }
    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }
    filter "files:Ares/vendor/FastNoise/**.cpp or files:Ares/vendor/yaml-cpp/src/**.cpp"
       	flags { "NoPCH" }

    
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
        "Ares",
        -- "Ares/vendor/assimp/win64/assimp.lib"
    } 
    -- postbuildcommands 
    -- {
    --     '{COPY} "../Ares/vendor/assimp/win64/assimp.lib" "%{cfg.targetdir}"',
    -- }

    filter "system:windows"
        systemversion "latest"
        
        
    filter "configurations:Debug"
        defines "ARES_DEBUG"
        runtime "Debug"
        symbols "on"
        links
		{
			"Ares/vendor/assimp/bin/Debug/assimp-vc141-mtd.lib"
		}

		postbuildcommands 
		{
			'{COPY} "../Ares/vendor/assimp/bin/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"',
		}

    filter "configurations:Release"
        defines "ARES_RELEASE"
        runtime "Release"
        optimize "on"
        links
		{
			"Hazel/vendor/assimp/bin/Release/assimp-vc141-mt.lib"
		}
		postbuildcommands 
		{
			'{COPY} "../Hazel/vendor/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
		}

    filter "configurations:Dist"
        defines "ARES_DIST"
        runtime "Release"
        optimize "on"
        links
		{
			"Hazel/vendor/assimp/bin/Release/assimp-vc141-mt.lib"
		}
		postbuildcommands 
		{
			'{COPY} "../Hazel/vendor/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
		}


project "Phobos"
    location "Phobos"
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
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}"
    }

    links
    {
        "Ares",
        -- "Ares/vendor/assimp/win64/assimp.lib"

    } 
    -- postbuildcommands 
    -- {
    --     '{COPY} "../Ares/vendor/assimp/win64/assimp.dll" "%{cfg.targetdir}"',
    -- }
    postbuildcommands 
	{
		'{COPY} "../Phobos/Assets" "%{cfg.targetdir}/Assets"'
	}
	


    filter "system:windows"
        systemversion "latest"
        
        
    filter "configurations:Debug"
        defines "ARES_DEBUG"
        runtime "Debug"
        symbols "on"
        links
		{
			"Ares/vendor/assimp/bin/Debug/assimp-vc141-mtd.lib"
		}

		postbuildcommands 
		{
			'{COPY} "../Ares/vendor/assimp/bin/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"',
		}

    filter "configurations:Release"
        defines "ARES_RELEASE"
        runtime "Release"
        optimize "on"
        links
		{
			"Hazel/vendor/assimp/bin/Release/assimp-vc141-mt.lib"
		}
		postbuildcommands 
		{
			'{COPY} "../Hazel/vendor/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
		}

    filter "configurations:Dist"
        defines "ARES_DIST"
        runtime "Release"
        optimize "on"
        links
		{
			"Hazel/vendor/assimp/bin/Release/assimp-vc141-mt.lib"
		}
		postbuildcommands 
		{
			'{COPY} "../Hazel/vendor/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
		}