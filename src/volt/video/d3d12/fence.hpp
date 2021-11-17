#pragma once

#include <volt/pch.hpp>

#include <volt/video/fence.hpp>

namespace volt::video::d3d12 {

class fence : public video::fence {
public:
	ID3D12Fence *d3d_fence;
	HANDLE wait_event;

	fence(std::shared_ptr<video::device> &&device, uint64_t initial_value = 0);

	~fence();

	uint64_t value() override;

	void signal(uint64_t value) override;

	void wait(uint64_t value) override;
};

}
