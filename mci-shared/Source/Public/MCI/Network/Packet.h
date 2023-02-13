#pragma once

#include "MCI/Common.h"
#include <string>
#include <vector>

namespace MCI
{

	class Packet final
	{
	public:

		Packet() = default;

		// Creates new packet with id of type T (should be numeric)
		template <typename T>
		Packet(T id)
		{
			operator>>(id);
		}

		Packet(const Packet& other) = default;
		Packet(Packet&& other) noexcept = default;
		~Packet() = default;

		Packet& operator=(const Packet& other) = default;

		void Append(const void* data, uint64 sizeInBytes);
		uint64 GetReadPosition() const;

		void Clear();

		const void* GetData() const;
		uint64 GetDataSize() const;

		bool EndOfPacket() const;

		bool IsValid() const;
		explicit operator bool() const;

		Packet& operator>>(bool& data);
		Packet& operator>>(int8& data);
		Packet& operator>>(uint8& data);
		Packet& operator>>(int16& data);
		Packet& operator>>(uint16& data);
		Packet& operator>>(int32& data);
		Packet& operator>>(uint32& data);
		Packet& operator>>(int64& data);
		Packet& operator>>(uint64& data);

		Packet& operator>>(float& data);
		Packet& operator>>(double& data);

		Packet& operator>>(char* data);
		Packet& operator>>(std::string& data);
		Packet& operator>>(wchar_t* data);
		Packet& operator>>(std::wstring& data);

		Packet& operator<<(bool data);
		Packet& operator<<(int8 data);
		Packet& operator<<(uint8 data);
		Packet& operator<<(int16 data);
		Packet& operator<<(uint16 data);
		Packet& operator<<(int32 data);
		Packet& operator<<(uint32 data);
		Packet& operator<<(int64 data);
		Packet& operator<<(uint64 data);

		Packet& operator<<(float data);
		Packet& operator<<(double data);

		Packet& operator<<(const char* data);
		Packet& operator<<(const std::string& data);
		Packet& operator<<(std::string_view& data);
		Packet& operator<<(const wchar_t* data);
		Packet& operator<<(std::wstring_view data);

	private:

		std::vector<char> m_data;
		uint64 m_readPosition{ 0 };
		uint64 m_sendPosition{ 0 };
		bool m_isValid{ true };

		bool operator==(const Packet& other) const { return false; }
		bool operator!=(const Packet& other) const { return false; }

		bool CheckSize(uint64 size);

	};

}