workspace "Game"

configurations {"Debug", "Release"}
architecture "x86_64"

project "glad"
	kind "StaticLib"
	language "C"

	targetdir "../../build/"
	objdir "../../build/"


	files
	{
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c"
	}

	includedirs {
		"include"
	}


	filter "system:linux"
		systemversion "latest"
		buildoptions {"-g", "-fPIC"}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
        buildoptions "/MTd"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
