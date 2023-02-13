#pragma once

#include "MCI/Common.h"
#include "MCI/Network/Packet.h"
#include <unordered_map>
#include <unordered_set>

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

	class Socket;

	class SocketHandler
	{
	public:

		/* Shared events */

		virtual void OnSocketClosed(Socket& socket) {}

		/* Client events */

		virtual void OnConnectedToServer(Socket& socket) {}
		virtual void OnConnectionUnsuccessfull(Socket& socket) {}
		virtual void OnDisconnectedFromServer(Socket& socket, ClientDisconnectReason reason) {}
		virtual void OnReceivedPacket(Socket& socket, Packet& packet) {}

		/* Server events */

		virtual void OnServerStarted(Socket& socket) {}
		virtual void OnClientConnected(Socket& socket, ClientId id) {}
		virtual void OnClientDisconnected(Socket& socket, ClientId id, ClientDisconnectReason reason) {}
		virtual void OnReceivedPacketFromClient(Socket& socket, ClientId id, Packet& packet) {}

	};

	class Socket final
	{
	public:

		static void InitSubsystem();
		static void ShutdownSubsystem();

		Socket(SocketHandler& handler);
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

		SocketHandler& m_handler;

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