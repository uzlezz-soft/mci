#include "MCI/ActionPool.h"

namespace MCI
{

	void ActionPool::Execute()
	{
		std::scoped_lock<std::mutex> scopedLock(m_mutex);

		while (!m_queue.empty())
		{
			auto& action = m_queue.front();
			action();
			m_queue.pop();
		}
	}

}