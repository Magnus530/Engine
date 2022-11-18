workspace "Engine"
	architecture "x64"
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

--Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Engine/vendor/GLFW/include"
IncludeDir["Glad"] = "Engine/vendor/Glad/include"
IncludeDir["ImGui"] = "Engine/vendor/imgui"
IncludeDir["glm"] = "Engine/vendor/glm"
IncludeDir["stb_image"] = "Engine/vendor/stb_image"
IncludeDir["FMOD"] = "Engine/vendor/FMOD/include"

include "Engine/vendor/GLFW"
include "Engine/vendor/Glad"
include "Engine/vendor/imgui"
--include "Engine/vendor/FMOD"

fmodlibdir = {}
fmodlibdir["FMOD"] = "Engine/vendor/FMOD/libs"


project "Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "epch.h"
	pchsource "Engine/src/epch.cpp"

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
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.FMOD}",
		"%{IncludeDir.stb_image}"
	}

	libdirs
	{
	"%{fmodlibdir.FMOD}"
	}
	
	links
	{
		"GLFW",
		"Glad",
		"ImGui",
--		"FMOD",
		"fmod_vc.lib",
		"fmodstudio_vc.lib",
		"opengl32.lib"
		
	}

	filter
		"system:windows"
		systemversion "latest"

		defines
		{
			"E_PLATFORM_WINDOWS",
			"E_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "E_DEBUG"
		runtime "Debug"
		symbols "on"
		links{
		"FMOD/libs/fmodL_vc.lib",
		"FMOD/libs/fmodstudioL_vc.lib"
		}

	filter "configurations:Release"
		defines "E_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "E_DIST"
		runtime "Release"
		optimize "on"
		
		-- FMOD
	filter { "system:windows"}
		postbuildcommands
		{
			("{COPYFILE} %{wks.location}%{fmodlibdir} bin/" .. outputdir .. "/Engine/")
		}

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
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"Engine/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Engine"
	}

	filter
		"system:windows"
		systemversion "latest"

		defines
		{
			"E_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "E_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "E_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "E_DIST"
		runtime "Release"
		optimize "on"