#include "MCI/Block.h"
#include "MCI/Log.h"

namespace MCI
{

	BlockInfo* BlocksDatabase::GetBlockById(uint8 id)
	{
		auto pair = Get().m_blocksInfo.find(id);
		if (pair != Get().m_blocksInfo.end())
		{
			return &pair->second;
		}

		return nullptr;
	}

	uint8 BlocksDatabase::AddBlock(BlockInfo& info)
	{
		uint8 id = Get().m_blocksInfo.size() + 1;

		info.Id = id;

		Get().m_blocksInfo[id] = info;

		MCI_LOG_INFO("Registered new block with id {}: {}", id, info.Name);

		return id;
	}

	uint8 BlocksDatabase::GetNumberOfBlocks()
	{
		return Get().m_blocksInfo.size();
	}

	void BlocksDatabase::Drop()
	{
		Get().m_blocksInfo.clear();
		MCI_LOG_WARN("Blocks database dropped!");
	}

	BlocksDatabase& BlocksDatabase::Get()
	{
		static BlocksDatabase instance;
		return instance;
	}

}