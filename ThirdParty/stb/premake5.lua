project "stb"
    kind "StaticLib"
	language "C"

	staticruntime "on"

	flags { "NoPCH" }

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/ThirdParty/%{prj.name}")
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/ThirdParty/%{prj.name}")

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"Source/**.h",
		"Source/**.c",
	}

    includedirs
	{
		"Source/Public"
	}