#pragma once

#include "../macros.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace volt::util {

// Thread pool access requires external synchronization
class thread_pool {
public:
	thread_pool(uint32_t thread_count = 0);

	~thread_pool();

	// No new jobs can be submitted while waiting.
	void submit(std::function<void(uint32_t)> &&job);

	// Waits for all submitted jobs. The pool must not be accessed when this method is stalling
	void wait();

	// Does not require synchronization.
	uint32_t thread_count() const;

private:
	std::vector<std::thread> threads;

	std::queue<std::function<void(uint32_t)>> queue;
	size_t pending_jobs = 0;
	std::mutex queue_mutex, pending_jobs_mutex;

	std::condition_variable submit_notifier, wait_notifier;
	std::atomic_flag terminating;

	void process(uint32_t thread_index);
};

}
