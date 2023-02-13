Include = {}

Include["glad"] = "%{wks.location}/ThirdParty/glad/Source/Public"
Include["spdlog"] = "%{wks.location}/ThirdParty/spdlog/Source/Public"
Include["GLFW"] = "%{wks.location}/ThirdParty/GLFW/include"
Include["stb"] = "%{wks.location}/ThirdParty/stb/Source/Public"
Include["glm"] = "%{wks.location}/ThirdParty/glm/Source/Public"
Include["zlib"] = "%{wks.location}/ThirdParty/zlib"
Include["enet"] = "%{wks.location}/ThirdParty/enet/Source/Public"
Include["lua"] = "%{wks.location}/ThirdParty/lua/Source/Public"
Include["luabridge"] = "%{wks.location}/ThirdParty/luabridge/Source/Public"

Include["mci_shared"] = "%{wks.location}/mci-shared/Source/Public"

Library = {}

Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"