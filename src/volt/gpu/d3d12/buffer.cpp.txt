// #include <volt/gpu/d3d12/buffer.hpp>

// #include <volt/gpu/d3d12/adapter.hpp>
// #include <volt/gpu/d3d12/d3d12.hpp>
// #include <volt/error.hpp>

// namespace volt::gpu::d3d12 {

// buffer::buffer(std::shared_ptr<gpu::device> &&device,
// 			gpu::memory_type memory_type,
// 			gpu::command_types sync_queues,
// 			gpu::buffer_features features,
// 			size_t size)
// 			: gpu::buffer(std::move(device)) {
// 	auto allocator = static_cast<d3d12::device *>(this->device.get())->allocator;

// 	uint32_t sync_count =
// 			static_cast<bool>(sync_queues & gpu::command_type::rasterization) +
// 			static_cast<bool>(sync_queues & gpu::command_type::compute) +
// 			static_cast<bool>(sync_queues & gpu::command_type::copy);

// 	D3D12_RESOURCE_DESC resource_desc;
// 	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
// 	resource_desc.Alignment = 0;
// 	resource_desc.Width = size;
// 	resource_desc.Height = 1;
// 	resource_desc.DepthOrArraySize = 1;
// 	resource_desc.MipLevels = 1;
// 	resource_desc.Format = DXGI_FORMAT_UNKNOWN;
// 	resource_desc.SampleDesc.Count = 1;
// 	resource_desc.SampleDesc.Quality = 0;
// 	resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
// 	resource_desc.Flags = sync_count > 1 ? D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS : D3D12_RESOURCE_FLAG_NONE;
	
// 	D3D12MA::ALLOCATION_DESC allocation_desc = {};
// 	allocation_desc.HeapType = d3d12::heap_types[memory_type];

// 	D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_COMMON;

// 	if (memory_type == gpu::memory_type::staging)
// 		states |= D3D12_RESOURCE_STATE_COPY_DEST;
// 	else if (memory_type == gpu::memory_type::read)
// 		states |= D3D12_RESOURCE_STATE_GENERIC_READ; 

// 	if (features & buffer_feature::source)
// 		states |= D3D12_RESOURCE_STATE_COPY_SOURCE;
// 	if (features & buffer_feature::destination)
// 		states |= D3D12_RESOURCE_STATE_COPY_SOURCE;
// 	if (features & buffer_feature::constant || features & buffer_feature::vertex)
// 		states |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
// 	if (features & buffer_feature::index)
// 		states |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
	
// 	VOLT_D3D12_CHECK(allocator->CreateResource(
// 			&allocation_desc,
// 			&resource_desc,
// 			states,
// 			nullptr,
// 			&allocation,
// 			IID_PPV_ARGS(&resource)), "Failed to create resource.")
// }

// buffer::~buffer() {
// 	allocation->Release();
// 	resource->Release();
// }

// void buffer::map(void **ptr, size_t read_offset, size_t read_size) {
// 	D3D12_RANGE range(read_offset, read_size);
// 	VOLT_D3D12_DEBUG_CHECK(resource->Map(0, &range, ptr), "Failed to map resource.");
// }

// void buffer::unmap(size_t write_offset, size_t write_size) {
// 	D3D12_RANGE range(write_offset, write_size);
// 	resource->Unmap(0, &range);
// }

// }
