#include <volt/pch.hpp>
#include <volt/util/thread_pool.hpp>

#include <volt/error.hpp>
#include <volt/log.hpp>

namespace volt::util {

thread_pool thread_pool::common_pool;

thread_pool::thread_pool(uint32_t thread_count) {
	if (thread_count == 0)
		thread_count = std::thread::hardware_concurrency();
	
	if (thread_count == 0) {
		VOLT_LOG_WARNING("std::thread::hardware_concurrency() is not supported, falling back to 8 execution threads.")
		thread_count = 8;
	}

	threads.reserve(thread_count);

	for (uint32_t i = 0; i < thread_count; i++) {
		threads.emplace_back(&thread_pool::process, this, i, nullptr);
		thread_id_to_index.emplace(threads.back().get_id(), i);
	}
}

thread_pool::~thread_pool() {
	// Threads will exit as soon as there is no more work to pick up
	terminating.test_and_set();
	notifier.notify_all();

	// Wait for threads that still might be processing
	for (std::thread &thread : threads)
		thread.join();
}

std::shared_ptr<future> thread_pool::submit(std::function<void(uint32_t)> job) {
	std::shared_ptr<util::future> future(new util::future(*this));

	{
		std::unique_lock lock(queue_mutex);
		queue.push({ std::move(job), future });
	}
	
	notifier.notify_all();
	return future;
}

uint32_t thread_pool::thread_count() const {
	return threads.size();
}

void thread_pool::process(uint32_t thread_index, const future *quit_when_ready) {
	std::function<void(uint32_t thread_index)> job;
	std::shared_ptr<future> future;
	
	while (true) {
		{
			std::unique_lock lock(queue_mutex);
			notifier.wait(lock, [&] {
				return !queue.empty() || terminating.test() || (quit_when_ready != nullptr && quit_when_ready->ready());
			});

			// If there's no more work to pick up and termination was requested
			if (queue.empty() && terminating.test())
				break;

			// Or when the quit_when_ready future is ready
			if (quit_when_ready != nullptr && quit_when_ready->ready())
				break;
			
			job = std::move(queue.front().first);
			future = std::move(queue.front().second);
			queue.pop();
		}

		try {
			job(thread_index);
		} catch (...) {
			future->exception = std::current_exception();
		}
		
		future->_ready.test_and_set();
		future->_ready.notify_all();

		// This is needed so that quit_when_ready futures can be checked again
		notifier.notify_all();
	}
}

bool future::ready() const noexcept {
	return _ready.test();
}

void future::wait() const {
	if (ready())
		return;

	std::thread::id id = std::this_thread::get_id();
	if (pool.thread_id_to_index.contains(id)) {
		// Continue some other work if it's a worker thread that requested to wait
		pool.process(pool.thread_id_to_index.at(id), this);
	} else {
		// If it's an external thread that requested to wait, then pause until future is ready
		_ready.wait(false);
	}
}

void future::rethrow() const {
	wait();

	if (exception != nullptr)
		std::rethrow_exception(exception);
}

future::future(thread_pool &pool) : pool(pool) {}

}
