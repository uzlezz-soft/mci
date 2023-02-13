#include "MCI/Log.h"
#include "MCI/Assert.h"
#include "MCI/Rendering/Window.h"
#include "MCI/Network/Packet.h"
#include "MCI/Network/Packets.h"
#include "MCI/Network/Socket.h"
#include "MCI/Block.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace MCI;

class ClientSocketHandler : public SocketHandler
{
public:

	void OnConnectedToServer(Socket& socket) override
	{

	}

	void OnReceivedPacket(Socket& socket, Packet& packet) override
	{
		uint8 packetId;
		packet >> packetId;

		switch (packetId)
		{
			case SP_Chunk:
				break;

			case SP_BlocksInfo:
			{
				uint8 numberOfBlocks = 0;
				packet >> numberOfBlocks;

				for (uint8 i = 0; i < numberOfBlocks; i++)
				{
					BlockInfo info{};
					packet >> info.Name;
					packet >> info.Transparent >> info.EmitsLight;
					for (uint8 i = 0; i < info.Emission.size(); i++) packet >> info.Emission[i];
					packet >> (uint8&)info.Mesh;

					if (info.Mesh == MeshType::Custom)
					{
						uint64 verticesAmount = 0;
						packet >> verticesAmount;
						info.Vertices.reserve(verticesAmount);

						for (uint64 i = 0; i < verticesAmount; i++)
						{
							packet >> info.Vertices[i];
						}
					}

					BlocksDatabase::AddBlock(info);
				}

				break;
			}

			case SP_ChatMessage:
			{
				std::string message;
				packet >> message;

				MCI_LOG_INFO("{}", message);
				break;
			}

			case SP_UpdateBlock:
				break;
		}
	}

};

int main()
{
	Log::Init("MCI-Client");
	Socket::InitSubsystem();

	MCI_LOG_INFO("Hello World!");

	ClientSocketHandler socketHandler;

	Socket client(socketHandler);

	client.Connect("127.0.0.1", 6969);

	Packet packet;
	packet << CP_Connect;
	packet << "Uzlezz";

	client.SendPacket(packet, PSM_Reliable);

	Window::InitSubsystem();

	Window::Context context;
	Scope<Window> window = MakeScope<Window>(context);

	window->Open(1280, 720, "MCI");

	MCI_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	while (window->IsOpen())
	{
		window->PollEvents();

		if (window->IsKeyDown(KeyCode::M))
		{
			Packet packet;
			packet << CP_ChatMessage;
			packet << "Lesha bot";

			client.SendPacket(packet);
		}

		client.Tick();

		window->SwapBuffers();
	}

	client.Close();

	Socket::ShutdownSubsystem();
	Window::ShutdownSubsystem();

	return 0;
}