workspace "Game"
configurations {"Debug", "Release"}
architecture "x86_64"
toolset "clang"

project "glad"
	kind "StaticLib"
	language "C"

	targetdir "../../bin/"
	objdir "../../bin/"


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
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"