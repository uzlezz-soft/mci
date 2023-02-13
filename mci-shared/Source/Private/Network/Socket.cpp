#include "MCI/Network/Socket.h"
#include "MCI/Assert.h"
#include <enet/enet.h>

namespace MCI
{

	void Socket::InitSubsystem()
	{
		MCI_ASSERT(enet_initialize() == 0);

		MCI_LOG_TRACE("Network subsystem initialized.");
	}

	void Socket::ShutdownSubsystem()
	{
		enet_deinitialize();

		MCI_LOG_TRACE("Network subsystem shutdowned.");
	}

	Socket::Socket(SocketHandler& handler) : m_handler(handler)
	{
	}

	Socket::~Socket()
	{
		Close();
	}

	void Socket::StartListening(uint16 port, uint64 maxClients)
	{
		m_server = true;
		m_maxClients = maxClients;

		ENetAddress address{};
		address.host = ENET_HOST_ANY;
		address.port = port;

		m_host = enet_host_create(&address, maxClients, 2, 0, 0);

		MCI_ASSERT(m_host, "Couldn't start server!");

		m_handler.OnServerStarted(*this);
		MCI_LOG_DEBUG("Started listening on port {}", port);
	}

	void Socket::Connect(std::string_view ip, uint16 port)
	{
		m_server = false;

		m_host = enet_host_create(nullptr, 1, 1, 0, 0);

		ENetAddress address{};
		
		enet_address_set_host(&address, ip.data());
		address.port = port;

		m_clientPeer = enet_host_connect(m_host, &address, 1, 0);
		enet_host_flush(m_host);

		MCI_ASSERT(m_clientPeer, "Can't connect to server!");

		ENetEvent event;
		MCI_ASSERT(enet_host_service(m_host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT, "Can't connect to server");

		enet_peer_timeout(m_clientPeer, ENET_PEER_TIMEOUT_LIMIT, 3000, 5000);

		m_connected = true;
		m_handler.OnConnectedToServer(*this);
		MCI_LOG_INFO("Connected to {}:{}", ip, port);
	}

	void Socket::SendPacket(const Packet& packet, PacketSendMode sendMode)
	{
		ENetPacket* p = enet_packet_create(packet.GetData(), packet.GetDataSize(), ENET_PACKET_FLAG_NO_ALLOCATE | (sendMode == PSM_Reliable ? ENET_PACKET_FLAG_RELIABLE : 0));
		enet_peer_send(m_clientPeer, 0, p);
		enet_host_flush(m_host);
	}

	void Socket::SendPacket(ClientId clientId, const Packet& packet, PacketSendMode sendMode)
	{
		ENetPacket* p = enet_packet_create(packet.GetData(), packet.GetDataSize(), ENET_PACKET_FLAG_NO_ALLOCATE | (sendMode == PSM_Reliable ? ENET_PACKET_FLAG_RELIABLE : 0));
		enet_peer_send(m_clients[clientId], 0, p);
		enet_host_flush(m_host);
	}

	void Socket::SendPacketToAll(const Packet& packet, PacketSendMode sendMode)
	{
		ENetPacket* p = enet_packet_create(packet.GetData(), packet.GetDataSize(), ENET_PACKET_FLAG_NO_ALLOCATE | (sendMode == PSM_Reliable ? ENET_PACKET_FLAG_RELIABLE : 0));
		enet_host_broadcast(m_host, 0, p);
		enet_host_flush(m_host);
	}

	void Socket::SendPacketToAllExcept(const std::unordered_set<ClientId>& exceptedClients, const Packet& packet, PacketSendMode sendMode)
	{
		enet_uint32 packetFlags = ENET_PACKET_FLAG_NO_ALLOCATE | (sendMode == PSM_Reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

		for (auto& [id, peer] : m_clients)
		{
			if (exceptedClients.find(id) == exceptedClients.end())
			{
				ENetPacket* p = enet_packet_create(packet.GetData(), packet.GetDataSize(), packetFlags);
				enet_peer_send(peer, 0, p);
			}
		}

		enet_host_flush(m_host);
	}

	void Socket::KickClient(ClientId clientId, std::string_view message)
	{
		auto peer = m_clients[clientId];

		enet_peer_disconnect_now(peer, 69);

		if (message.empty()) MCI_LOG_INFO("Client with id {} was kicked off the server.", clientId);
		else MCI_LOG_INFO("Client with id {} was kicked off the server. Reason: {}.", clientId, message);
	}

	void Socket::Tick()
	{
		ENetEvent event;

		if (m_server)
		{
			while (PollEvent(event))
			{
				switch (event.type)
				{
					case ENET_EVENT_TYPE_CONNECT:
					{
						ClientId id = 0;

						for (ClientId i = 1; i < m_maxClients; i++)
						{
							if (m_clients.find(i) == m_clients.end())
							{
								id = i;
								break;
							}
						}

						m_clients[id] = event.peer;
						event.peer->data = (void*)id;

						enet_peer_timeout(event.peer, ENET_PEER_TIMEOUT_LIMIT, 3000, 5000);

						MCI_LOG_INFO("New client connected with id {}.", id);

						m_handler.OnClientConnected(*this, id);

						break;
					}

					case ENET_EVENT_TYPE_DISCONNECT:
					{
						ClientId id = (ClientId)event.peer->data;
						m_clients.erase(id);
						MCI_LOG_INFO("Client with id {} disconnected.", id);
						m_handler.OnClientDisconnected(*this, id, CDR_Disconnect);

						break;
					}

					case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
					{
						ClientId id = (ClientId)event.peer->data;
						m_clients.erase(id);
						MCI_LOG_INFO("Client with id {} disconnected (timeout).", id);
						m_handler.OnClientDisconnected(*this, id, CDR_TimeOut);

						break;
					}

					case ENET_EVENT_TYPE_RECEIVE:
					{
						ClientId id = (ClientId)event.peer->data;

						Packet packet;
						packet.Append(event.packet->data, event.packet->dataLength);
						enet_packet_destroy(event.packet);

						m_handler.OnReceivedPacketFromClient(*this, id, packet);

						break;
					}
				}
			}
		}
		else
		{
			while (PollEvent(event))
			{
				switch (event.type)
				{
					case ENET_EVENT_TYPE_DISCONNECT:
					{
						m_connected = false;

						if (event.data == 69)
						{
							MCI_LOG_INFO("Disconnected from server! (Kick)");
							m_handler.OnDisconnectedFromServer(*this, CDR_Kick);
						}
						else
						{
							MCI_LOG_INFO("Disconnected from server!");
							m_handler.OnDisconnectedFromServer(*this, CDR_Disconnect);
						}

						break;
					}

					case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
					{
						MCI_LOG_INFO("Disconnected from server! (timeout)");
						m_connected = false;
						m_handler.OnDisconnectedFromServer(*this, CDR_TimeOut);

						break;
					}

					case ENET_EVENT_TYPE_RECEIVE:
					{
						Packet packet;
						packet.Append(event.packet->data, event.packet->dataLength);
						enet_packet_destroy(event.packet);

						m_handler.OnReceivedPacket(*this, packet);
						break;
					}
				}
			}
		}
	}

	void Socket::Close()
	{
		if (!m_host) return;

		if (!m_server && m_connected)
		{
			enet_peer_disconnect(m_clientPeer, 0);

			bool disconnected = false;

			ENetEvent event;
			while (!disconnected)
			{
				while (PollEvent(event))
				{
					switch (event.type)
					{
					case ENET_EVENT_TYPE_DISCONNECT:
					{
						disconnected = true;
						break;
					}

					case ENET_EVENT_TYPE_RECEIVE:
					{
						enet_packet_destroy(event.packet);
						break;
					}

					default:
						break;
					}
				}
			}

			m_connected = false;
		}
		else
		{
			for (auto& [id, peer] : m_clients)
			{
				enet_peer_disconnect(peer, 0);
				m_handler.OnClientDisconnected(*this, id, CDR_Disconnect);
			}

			m_clients.clear();
		}

		enet_host_destroy(m_host);
		m_host = nullptr;
		m_handler.OnSocketClosed(*this);
	}

	std::vector<ClientId> Socket::GetConnectedClients() const
	{
		std::vector<ClientId> clients;
		clients.resize(m_clients.size());

		ClientId i = 0;
		for (auto& [id, peer] : m_clients)
		{
			clients[i++] = id;
		}

		return clients;
	}

	ClientId Socket::FindClientIdByPeer(_ENetPeer* peer) const
	{
		for (auto& [id, p] : m_clients)
		{
			if (p == peer) return id;
		}

		return 0;
	}

	bool Socket::PollEvent(ENetEvent& event)
	{
		return enet_host_service(m_host, &event, 0) > 0;
	}

}