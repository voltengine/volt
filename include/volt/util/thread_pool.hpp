#pragma once

#include "../macros.hpp"

#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <vector>

namespace volt::util {

class future;

// Access from jobs submitted to this pool does not require synchronization
// Access from any other thread requires synchronization
class thread_pool {
public:
	static thread_pool common_pool;

	thread_pool(uint32_t thread_count = 0);

	~thread_pool();

	std::shared_ptr<future> submit(std::function<void(uint32_t)> job);

	uint32_t thread_count() const;

private:
	std::vector<std::thread> threads;
	std::unordered_map<std::thread::id, uint32_t> thread_id_to_index;
	std::queue<std::pair<std::function<void(uint32_t)>, std::shared_ptr<future>>> queue;

	std::mutex queue_mutex;
	std::condition_variable notifier;
	std::atomic_flag terminating;
	
	void process(uint32_t thread_index, const future *quit_when_ready);

	friend future;
};

class future {
public:
	bool ready() const noexcept;

	void wait() const;

	void rethrow() const;

private:
	thread_pool &pool;
	std::atomic_flag _ready;
	std::exception_ptr exception = nullptr;

	future(thread_pool &pool);

	friend thread_pool;
};

}
