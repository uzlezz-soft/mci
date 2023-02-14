#include "MCI/Network/Packet.h"
#include <enet/enet.h>
#include <cstring>

namespace MCI
{

	void Packet::Append(const void* data, uint64 sizeInBytes)
	{
		if (data && (sizeInBytes > 0))
		{
			uint64 start = m_data.size();
			m_data.resize(start + sizeInBytes);
			std::memcpy(&m_data[start], data, sizeInBytes);
		}
	}

	uint64 Packet::GetReadPosition() const
	{
		return m_readPosition;
	}

	void Packet::Clear()
	{
		m_data.clear();
		m_readPosition = 0;
		m_isValid = true;
	}

	const void* Packet::GetData() const
	{
		return !m_data.empty() ? m_data.data() : nullptr;
	}

	uint64 Packet::GetDataSize() const
	{
		return m_data.size();
	}

	bool Packet::EndOfPacket() const
	{
		return m_readPosition >= m_data.size();
	}

	bool Packet::IsValid() const
	{
		return m_isValid;
	}

	Packet::operator bool() const
	{
		return IsValid();
	}

	Packet& Packet::operator>>(bool& data)
	{
		uint8 value;
		if (*this >> value) data = (value != 0);

		return *this;
	}

	Packet& Packet::operator>>(int8& data)
	{
		if (CheckSize(sizeof(data)))
		{
			std::memcpy(&data, &m_data[m_readPosition], sizeof(data));
			m_readPosition += sizeof(data);
		}

		return *this;
	}

	Packet& Packet::operator>>(uint8& data)
	{
		if (CheckSize(sizeof(data)))
		{
			std::memcpy(&data, &m_data[m_readPosition], sizeof(data));
			m_readPosition += sizeof(data);
		}

		return *this;
	}

	Packet& Packet::operator>>(int16& data)
	{
		if (CheckSize(sizeof(data)))
		{
			std::memcpy(&data, &m_data[m_readPosition], sizeof(data));
			data = static_cast<int16>(ntohs(static_cast<uint16>(data)));
			m_readPosition += sizeof(data);
		}

		return *this;
	}

	Packet& Packet::operator>>(uint16& data)
	{
		if (CheckSize(sizeof(data)))
		{
			std::memcpy(&data, &m_data[m_readPosition], sizeof(data));
			data = ntohs(data);
			m_readPosition += sizeof(data);
		}

		return *this;
	}

	Packet& Packet::operator>>(int32& data)
	{
		if (CheckSize(sizeof(data)))
		{
			std::memcpy(&data, &m_data[m_readPosition], sizeof(data));
			data = static_cast<int32>(ntohs(static_cast<uint32>(data)));
			m_readPosition += sizeof(data);
		}

		return *this;
	}

	Packet& Packet::operator>>(uint32& data)
	{
		if (CheckSize(sizeof(data)))
		{
			std::memcpy(&data, &m_data[m_readPosition], sizeof(data));
			data = ntohl(data);
			m_readPosition += sizeof(data);
		}

		return *this;
	}

	Packet& Packet::operator>>(int64& data)
	{
		if (CheckSize(sizeof(data)))
		{
			// Since ntohll is not available everywhere, we have to convert
			// to network byte order (big endian) manually
			uint8 bytes[sizeof(data)];
			std::memcpy(bytes, &m_data[m_readPosition], sizeof(data));

			data = (static_cast<int64>(bytes[0]) << 56) | (static_cast<int64>(bytes[1]) << 48) |
				(static_cast<int64>(bytes[2]) << 40) | (static_cast<int64>(bytes[3]) << 32) |
				(static_cast<int64>(bytes[4]) << 24) | (static_cast<int64>(bytes[5]) << 16) |
				(static_cast<int64>(bytes[6]) << 8) | (static_cast<int64>(bytes[7]));

			m_readPosition += sizeof(data);
		}

		return *this;
	}

	Packet& Packet::operator>>(uint64& data)
	{
		if (CheckSize(sizeof(data)))
		{
			// Since ntohll is not available everywhere, we have to convert
			// to network byte order (big endian) manually
			uint8 bytes[sizeof(data)];
			std::memcpy(bytes, &m_data[m_readPosition], sizeof(data));

			data = (static_cast<uint64>(bytes[0]) << 56) | (static_cast<uint64>(bytes[1]) << 48) |
				(static_cast<uint64>(bytes[2]) << 40) | (static_cast<uint64>(bytes[3]) << 32) |
				(static_cast<uint64>(bytes[4]) << 24) | (static_cast<uint64>(bytes[5]) << 16) |
				(static_cast<uint64>(bytes[6]) << 8) | (static_cast<uint64>(bytes[7]));

			m_readPosition += sizeof(data);
		}

		return *this;
	}

	Packet& Packet::operator>>(float& data)
	{
		if (CheckSize(sizeof(data)))
		{
			std::memcpy(&data, &m_data[m_readPosition], sizeof(data));
			m_readPosition += sizeof(data);
		}

		return *this;
	}

	Packet& Packet::operator>>(double& data)
	{
		if (CheckSize(sizeof(data)))
		{
			std::memcpy(&data, &m_data[m_readPosition], sizeof(data));
			m_readPosition += sizeof(data);
		}

		return *this;
	}

	Packet& Packet::operator>>(char* data)
	{
		uint32 length = 0;
		*this >> length;

		if ((length > 0) && CheckSize(length))
		{
			// Then extract characters
			std::memcpy(data, &m_data[m_readPosition], length);
			data[length] = '\0';

			// Update reading position
			m_readPosition += length;
		}

		return *this;
	}

	Packet& Packet::operator>>(std::string& data)
	{
		uint32 length = 0;
		*this >> length;

		data.clear();
		if ((length > 0) && CheckSize(length))
		{
			data.assign(&m_data[m_readPosition], length);

			// Update reading position
			m_readPosition += length;
		}

		return *this;
	}

	Packet& Packet::operator>>(wchar_t* data)
	{
		uint32 length = 0;
		*this >> length;

		if ((length > 0) && CheckSize(length * sizeof(uint32)))
		{
			// Then extract characters
			for (uint32 i = 0; i < length; ++i)
			{
				uint32 character = 0;
				*this >> character;
				data[i] = static_cast<wchar_t>(character);
			}
			data[length] = L'\0';
		}

		return *this;
	}

	Packet& Packet::operator>>(std::wstring& data)
	{
		std::uint32_t length = 0;
		*this >> length;

		data.clear();
		if ((length > 0) && CheckSize(length * sizeof(uint32)))
		{
			// Then extract characters
			for (uint32 i = 0; i < length; ++i)
			{
				uint32 character = 0;
				*this >> character;
				data += static_cast<wchar_t>(character);
			}
		}

		return *this;
	}

	Packet& Packet::operator<<(bool data)
	{
		*this << static_cast<uint8>(data);
		return *this;
	}

	Packet& Packet::operator<<(int8 data)
	{
		Append(&data, sizeof(data));
		return *this;
	}

	Packet& Packet::operator<<(uint8 data)
	{
		Append(&data, sizeof(data));
		return *this;
	}

	Packet& Packet::operator<<(int16 data)
	{
		auto toWrite = static_cast<int16>(htons(static_cast<uint16>(data)));
		Append(&toWrite, sizeof(toWrite));
		return *this;
	}

	Packet& Packet::operator<<(uint16 data)
	{
		uint16 toWrite = htons(data);
		Append(&toWrite, sizeof(toWrite));
		return *this;
	}

	Packet& Packet::operator<<(int32 data)
	{
		int32 toWrite = static_cast<int32>(htonl(static_cast<uint32>(data)));
		Append(&toWrite, sizeof(toWrite));
		return *this;
	}

	Packet& Packet::operator<<(uint32 data)
	{
		uint32 toWrite = htonl(data);
		Append(&toWrite, sizeof(toWrite));
		return *this;
	}

	Packet& Packet::operator<<(int64 data)
	{
		// Since htonll is not available everywhere, we have to convert
		// to network byte order (big endian) manually

		uint8 toWrite[] = { static_cast<uint8>((data >> 56) & 0xFF),
								  static_cast<uint8>((data >> 48) & 0xFF),
								  static_cast<uint8>((data >> 40) & 0xFF),
								  static_cast<uint8>((data >> 32) & 0xFF),
								  static_cast<uint8>((data >> 24) & 0xFF),
								  static_cast<uint8>((data >> 16) & 0xFF),
								  static_cast<uint8>((data >> 8) & 0xFF),
								  static_cast<uint8>((data) & 0xFF) };

		Append(&toWrite, sizeof(toWrite));
		return *this;
	}

	Packet& Packet::operator<<(uint64 data)
	{
		// Since htonll is not available everywhere, we have to convert
		// to network byte order (big endian) manually

		uint8 toWrite[] = { static_cast<uint8>((data >> 56) & 0xFF),
								  static_cast<uint8>((data >> 48) & 0xFF),
								  static_cast<uint8>((data >> 40) & 0xFF),
								  static_cast<uint8>((data >> 32) & 0xFF),
								  static_cast<uint8>((data >> 24) & 0xFF),
								  static_cast<uint8>((data >> 16) & 0xFF),
								  static_cast<uint8>((data >> 8) & 0xFF),
								  static_cast<uint8>((data) & 0xFF) };

		Append(&toWrite, sizeof(toWrite));
		return *this;
	}

	Packet& Packet::operator<<(float data)
	{
		Append(&data, sizeof(data));
		return *this;
	}

	Packet& Packet::operator<<(double data)
	{
		Append(&data, sizeof(data));
		return *this;
	}

	Packet& Packet::operator<<(const char* data)
	{
		auto length = static_cast<uint32>(std::strlen(data));
		*this << length;

		Append(data, length * sizeof(char));

		return *this;
	}

	Packet& Packet::operator<<(const std::string& data)
	{
		auto length = static_cast<std::uint32_t>(data.size());
		*this << length;

		if (length > 0) Append(data.data(), length * sizeof(std::string::value_type));

		return *this;
	}

	Packet& Packet::operator<<(std::string_view& data)
	{
		auto length = static_cast<std::uint32_t>(data.size());
		*this << length;

		if (length > 0) Append(data.data(), length * sizeof(std::string_view::value_type));

		return *this;
	}

	Packet& Packet::operator<<(const wchar_t* data)
	{
		auto length = static_cast<uint32>(std::wcslen(data));
		*this << length;

		for (const wchar_t* c = data; *c != L'\0'; ++c) *this << static_cast<uint32>(*c);

		return *this;
	}

	Packet& Packet::operator<<(std::wstring_view data)
	{
		auto length = static_cast<uint32>(data.size());
		*this << length;

		if (length > 0)
		{
			for (wchar_t c : data) *this << static_cast<uint32>(c);
		}

		return *this;
	}

	bool Packet::CheckSize(uint64 size)
	{
		m_isValid = m_isValid && (m_readPosition + size <= m_data.size());

		return m_isValid;
	}

}