project "mci-shared"
	language "C++"
	cppdialect "C++17"
    kind "StaticLib"

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Source/**.h",
		"Source/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"Source/Public",
		"%{Include.spdlog}",
		"%{Include.enet}",
		"%{Include.glm}"
	}

	links
	{
		"spdlog",
		"enet"
	}

	disablewarnings { "4251", "4005", "4267" }