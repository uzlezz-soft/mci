#pragma once

#include "MCI/Common.h"
#include <array>
#include <vector>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace MCI
{

	struct BlockInfo;

	struct Block
	{
		uint8 Id;
		const BlockInfo* Info;
	};

	enum class MeshType : uint8
	{
		Default, Cross, Torch, Custom
	};

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		uint16 TextureOffset;
	};

	template <typename S>
	S& operator<<(S& out, const Vertex& vertex)
	{
		out << vertex.Position.x << vertex.Position.y << vertex.Position.z;
		out << vertex.TexCoord.x << vertex.TexCoord.y << vertex.TextureOffset;

		return out;
	}

	template <typename S>
	S& operator>>(S& in, Vertex& vertex)
	{
		in >> vertex.Position.x >> vertex.Position.y >> vertex.Position.z;
		in >> vertex.TexCoord.x >> vertex.TexCoord.y >> vertex.TextureOffset;

		return in;
	}

	struct BlockInfo
	{
		enum class TextureSide : uint8
		{
			Top, Front, Right, Back, Left, Bottom, Max
		};

		uint8 Id;
		std::string Name;
		std::array<uint16, (uint8)TextureSide::Max> Textures;
		std::array<uint8, 3> Emission;
		bool EmitsLight = false;
		bool Transparent = false;
		MeshType Mesh = MeshType::Default;
		std::vector<Vertex> Vertices;
	};

	class BlocksDatabase
	{
	public:

		static BlockInfo* GetBlockById(uint8 id);

		static uint8 AddBlock(BlockInfo& info);

		static uint8 GetNumberOfBlocks();

		static void Drop();

	private:

		std::unordered_map<uint8, BlockInfo> m_blocksInfo;

		BlocksDatabase() = default;
		~BlocksDatabase() = default;

		static BlocksDatabase& Get();

	};

}