project "FMOD"
	kind "StaticLib"
	language "C"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"core/inc/**.h",
		"core/inc/fmod.hpp",
		"fsbank/inc/**.h",
		"studio/inc/**.h",
		"studio/inc/fmod_studio.hpp"
	}

	includedirs
	{
		"core/inc",
		"fsbank/inc",
		"studio/inc",
		"libs"
	}
	
	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"