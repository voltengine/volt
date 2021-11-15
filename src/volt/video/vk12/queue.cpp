#include <volt/pch.hpp>
#include <volt/video/vk12/queue.hpp>

namespace volt::video::vk12 {

queue::queue(VkQueue vk_queue) : vk_queue(vk_queue) {}

void queue::wait() {
	vkQueueWaitIdle(vk_queue);
}

}
