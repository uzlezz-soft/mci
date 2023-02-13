#include "MCI/Log.h"
#include "MCI/Network/Packets.h"
#include "MCI/Network/Socket.h"
#include "MCI/Block.h"
#include <sstream>

using namespace MCI;

int main()
{
	Log::Init("MCI-Server");
	Socket::InitSubsystem();

	MCI_LOG_INFO("Hello World!");

	BlockInfo info;
	info.EmitsLight = false;
	info.Name = "Dirt";
	for (uint8 i = 0; i < (uint8)BlockInfo::TextureSide::Max; i++) info.Textures[i] = 1;
	info.Transparent = false;

	BlocksDatabase::AddBlock(info);

	Socket server;

	struct ClientDesc
	{
		std::string Name;
		uint64 MessagesCount;

		bool Connected = false;
	};

	std::unordered_map<ClientId, ClientDesc> clients;

	server.OnClientConnected = [&clients](Socket& server, ClientId id)
	{
		clients[id] = ClientDesc{ "Anonymous", 0, false };
	};

	server.OnClientDisconnected = [&clients](Socket& server, ClientId id, ClientDisconnectReason reason)
	{
		clients.erase(id);

		Packet packet;
		packet << SP_ChatMessage;
		packet << fmt::format("Client with id {} disconnected", id).data();

		server.SendPacketToAll(packet);
	};

	server.OnReceivedPacketFromClient = [&clients](Socket& socket, ClientId id, Packet& packet)
	{
		uint8 packetId;
		packet >> packetId;

		auto& client = clients[id];

		switch (packetId)
		{
		case CP_Connect:
		{
			if (client.Connected)
			{
				/* Illegal or duplicate packet */
				socket.KickClient(id, "Illegal packet");
				break;
			}

			client.Connected = true;
			packet >> client.Name;

			Packet blocksInfo;
			blocksInfo << SP_BlocksInfo;
			blocksInfo << BlocksDatabase::GetNumberOfBlocks();

			for (uint8 i = 1; i <= BlocksDatabase::GetNumberOfBlocks(); i++)
			{
				auto info = BlocksDatabase::GetBlockById(i);

				blocksInfo << info->Name;
				blocksInfo << info->Transparent << info->EmitsLight;
				for (uint8 i = 0; i < info->Emission.size(); i++) blocksInfo << info->Emission[i];
				blocksInfo << (uint8)info->Mesh;

				if (info->Mesh == MeshType::Custom)
				{
					blocksInfo << info->Vertices.size();

					for (uint64 i = 0; i < info->Vertices.size(); i++)
					{
						blocksInfo << info->Vertices[i];
					}
				}
			}

			socket.SendPacket(id, blocksInfo, PSM_Reliable);

			break;
		}

		case CP_ChatMessage:
		{
			if (!client.Connected) break;

			std::string message;
			packet >> message;

			Packet p;
			p << SP_ChatMessage;
			p << fmt::format("{}: {}", client.Name, message).data();

			//socket.SendPacketToAllExcept({ id }, p);
			socket.SendPacketToAll(p);

			break;
		}
		}
	};

	server.StartListening(6969);

	while (true) { server.Tick(); }

	Socket::ShutdownSubsystem();

	return 0;
}