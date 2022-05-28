#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/d3d12/device.hpp>
#include <volt/gpu/buffer.hpp>
#include <volt/gpu/enums.hpp>
#include <volt/gpu/queue.hpp>

namespace volt::gpu::d3d12 {

class buffer : public gpu::buffer {
public:
	ID3D12Resource *resource;
	D3D12MA::Allocation *allocation;

	buffer(std::shared_ptr<gpu::device> &&device,
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::buffer_features features,
			size_t size);

	~buffer();

	void map(void **ptr, size_t read_offset = 0, size_t read_size = 0) override;

	void unmap(size_t write_offset = 0, size_t write_size = 0) override;
};

}