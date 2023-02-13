#pragma once

#include "MCI/Common.h"
#include <queue>
#include <functional>
#include <mutex>

namespace MCI
{

	class ActionPool final
	{
	public:

		using Action = std::function<void()>;

		ActionPool() = default;

		FORCEINLINE void AddAction(Action action)
		{
			std::scoped_lock<std::mutex> scopedLock(m_mutex);
			m_queue.push(action);
		}

		void Execute();

	private:

		std::queue<Action> m_queue;
		std::mutex m_mutex;

	};

}