#include <volt/pch.hpp>
#include <volt/util/thread_pool.hpp>

#include <volt/error.hpp>

namespace volt::util {

thread_pool::thread_pool(uint32_t thread_count) {
	if (thread_count == 0)
		thread_count = std::thread::hardware_concurrency();

	threads.reserve(thread_count);
	while (thread_count-- != 0)
		threads.emplace_back(&thread_pool::process, this, thread_count);
}

thread_pool::~thread_pool() {
	wait();

	// Halt all threads currently waiting on notifier
	terminating.test_and_set();
	submit_notifier.notify_all();

	// Wait for threads that still might be processing
	for (std::thread &thread : threads)
		thread.join();

	// Queue is empty at this point if no concurrent access is being done
}

void thread_pool::submit(std::function<void(uint32_t)> &&job) {
	{
		std::unique_lock lock(queue_mutex);
		queue.push(std::move(job));
	}
	
	submit_notifier.notify_one();

	{	
		std::unique_lock lock(pending_jobs_mutex);
		pending_jobs++;
	}
}

void thread_pool::wait() {
	std::unique_lock lock(pending_jobs_mutex);
	wait_notifier.wait(lock, [&] {
		return pending_jobs == 0;
	});
}

uint32_t thread_pool::thread_count() const {
	return threads.size();
}

void thread_pool::process(uint32_t thread_index) {
	while (true) {
		std::function<void(uint32_t thread_index)> job;

		{
			std::unique_lock lock(queue_mutex);
			submit_notifier.wait(lock, [&] {
				return !queue.empty() || terminating.test();
			});

			if (terminating.test())
				return;
			
			job = std::move(queue.front());
			queue.pop();
		}

		job(thread_index);

		{
			std::unique_lock lock(pending_jobs_mutex);
			pending_jobs--;
		}

		wait_notifier.notify_all();
	}
}

}
