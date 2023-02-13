include "Dependencies.lua"

workspace "MCI"
    architecture "x86_64"
    startproject "mci"

    flags { "NoPCH" }

    configurations
    {
        "Development",
        "Shipping"
    }

    files
    {
        ".editorconfig"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    defines
    {
        "PROJECT_PATH=\"%{wks.location}\"",
        "PROJECT_NAME=\"%{prj.name}\""
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    staticruntime "on"

    filter "system:windows"
		systemversion "latest"

		links
		{
			"%{Library.WinSock}",
			"%{Library.WinMM}",
            "%{Library.WinVersion}",
            "%{Library.BCrypt}"
		}

	filter "configurations:Development"
		defines
        {
            "MCI_DEVELOPMENT",
            "_DEBUG"
        }

		runtime "Debug"
		optimize "off"
		symbols "on"

	filter "configurations:Shipping"
		defines
        {
            "MCI_SHIPPING",
            "NDEBUG"
        }

		runtime "Release"
		optimize "Speed"
		symbols "on"

group "Dependencies"
    include "ThirdParty/Premake"
group ""

group "ThirdParty"
    include "ThirdParty/glad"
    include "ThirdParty/GLFW"
    include "ThirdParty/spdlog"
    include "ThirdParty/stb"
    include "ThirdParty/zlib"
    include "ThirdParty/enet"
    include "ThirdParty/lua"
group ""

group "Core"
    include "mci"
    include "mci-server"
    include "mci-shared"
group ""