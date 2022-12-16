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
IncludeDir["entt"] = "Engine/vendor/entt/include"
IncludeDir["FMOD"] = "Engine/vendor/FMOD/include"

include "Engine/vendor/GLFW"
include "Engine/vendor/Glad"
include "Engine/vendor/imgui"
include "Engine/vendor/FMOD"

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
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}"
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
		"../Engine/vendor/FMOD/libs/**.lib",
		"opengl32.lib",
		"opengl32"
	}

	filter
		"system:windows"
		systemversion "latest"

		defines
		{
			"E_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "E_DEBUG"
		runtime "Debug"
		symbols "on"
		links 
		{
		"fmodstudioL_vc.lib"
		}


	filter "configurations:Release"
		defines "E_RELEASE"
		runtime "Release"
		optimize "on"
		links {
		"fmodstudio_vc.lib"
		}


	filter "configurations:Dist"
		defines "E_DIST"
		runtime "Release"
		optimize "on"
		links {
		"fmodstudio_vc.lib"
		}

		
		-- FMOD
	filter { "system:windows"}
		postbuildcommands
		{
			("{COPYFILE} %{wks.location}%{fmodlibdir.FMOD}/fmod.dll $(TargetDir)"),
			("{COPYFILE} %{wks.location}%{fmodlibdir.FMOD}/fmodstudio.dll $(TargetDir)")
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
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"Engine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.FMOD}"
	}

	links
	{
		"Engine"
--		"fmod_vc.lib"
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