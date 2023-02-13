#pragma once

#include "MCI/Common.h"

namespace MCI
{

	enum ClientPacket : uint8
	{
		CP_Connect = 1,

		CP_ChatMessage
	};

	enum ServerPacket : uint8
	{
		SP_Chunk = 1,

		SP_BlocksInfo,

		SP_ChatMessage,

		SP_UpdateBlock
	};

}