project "spdlog"
    kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	staticruntime "on"

	flags { "NoPCH" }

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/ThirdParty/%{prj.name}")
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/ThirdParty/%{prj.name}")

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"SPDLOG_COMPILED_LIB"
	}

	files
	{
		"Source/**.h",
		"Source/**.cpp",
	}

    includedirs
	{
		"Source/Public"
	}

		