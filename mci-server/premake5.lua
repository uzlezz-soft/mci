project "mci-server"
	language "C++"
	cppdialect "C++17"
    kind "ConsoleApp"

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Source/**.h",
		"Source/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"Source/Public",
        "%{Include.mci_shared}",
		"%{Include.spdlog}",
		"%{Include.glm}"
	}

	links
	{
        "mci-shared",
		"enet",
		"spdlog"
	}

	disablewarnings { "4251", "4005", "4267" }

	filter "toolset:gcc* or clang*"
		links { "stdc++fs" }