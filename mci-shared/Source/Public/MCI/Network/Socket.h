#pragma once

#include "MCI/Common.h"
#include "MCI/Network/Packet.h"
#include <unordered_map>
#include <unordered_set>
#include <functional>

struct _ENetHost;
struct _ENetAddress;
struct _ENetEvent;
struct _ENetPeer;

namespace MCI
{

	using ClientId = uint64;

	enum PacketSendMode : uint8
	{
		PSM_Reliable,
		PSM_Unreliable
	};

	enum ClientDisconnectReason : uint8
	{
		CDR_Disconnect,
		CDR_TimeOut,
		CDR_Kick
	};

	class Socket final
	{
	public:

		static void InitSubsystem();
		static void ShutdownSubsystem();

		std::function<void(Socket&)> OnClosed;

		std::function<void(Socket&)> OnConnectedToServer;
		std::function<void(Socket&)> OnConnectUnsuccessfull;
		std::function<void(Socket&, ClientDisconnectReason)> OnDisconnectedFromServer;
		std::function<void(Socket&, Packet&)> OnReceivedPacket;

		std::function<void(Socket&)> OnServerStarted;
		std::function<void(Socket&, ClientId)> OnClientConnected;
		std::function<void(Socket&, ClientId, ClientDisconnectReason)> OnClientDisconnected;
		std::function<void(Socket&, ClientId, Packet&)> OnReceivedPacketFromClient;

		Socket();
		~Socket();

		void StartListening(uint16 port, ClientId maxClients = 64);
		void Connect(std::string_view ip, uint16 port);

		// Send packet as client
		void SendPacket(const Packet& packet, PacketSendMode sendMode = PSM_Unreliable);

		void SendPacket(ClientId clientId, const Packet& packet, PacketSendMode sendMode = PSM_Unreliable);
		void SendPacketToAll(const Packet& packet, PacketSendMode sendMode = PSM_Unreliable);
		void SendPacketToAllExcept(const std::unordered_set<ClientId>& exceptedClients, const Packet& packet, PacketSendMode sendMode = PSM_Unreliable);

		void KickClient(ClientId clientId, std::string_view message = "");

		void Tick();
		void Close();

		bool IsServer() const { return m_server; }

		std::vector<ClientId> GetConnectedClients() const;

	private:

		bool m_server{ false };
		_ENetHost* m_host{ nullptr };
		ClientId m_maxClients{ 0 };

		_ENetPeer* m_clientPeer{ nullptr };
		bool m_connected{ false };

		std::unordered_map<ClientId, _ENetPeer*> m_clients;

		ClientId FindClientIdByPeer(_ENetPeer* peer) const;

		bool PollEvent(_ENetEvent& event);

	};

}