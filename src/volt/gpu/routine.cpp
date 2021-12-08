#include <volt/pch.hpp>
#include <volt/gpu/routine.hpp>

namespace volt::gpu {

// Executors

copy_executor::copy_executor(_internal::routine_impl &impl)
		: impl(impl) {}

compute_executor::compute_executor(_internal::routine_impl &impl)
		: copy_executor(impl) {}

graphics_executor::graphics_executor(_internal::routine_impl &impl)
		: compute_executor(impl) {}

copy_executor copy_executor::_new(_internal::routine_impl &impl) {
	return copy_executor(impl);
}

compute_executor compute_executor::_new(_internal::routine_impl &impl) {
	return compute_executor(impl);
}

graphics_executor graphics_executor::_new(_internal::routine_impl &impl) {
	return graphics_executor(impl);
}

void copy_executor::copy_buffer(
		const std::shared_ptr<gpu::buffer> &src,
		const std::shared_ptr<gpu::buffer> &dst,
		size_t src_offset,
		size_t dst_offset,
		size_t size) {
	impl.copy_buffer(src, dst, src_offset, dst_offset, size);
}

void copy_executor::copy_texture(
		const std::shared_ptr<gpu::texture> &src,
		const std::shared_ptr<gpu::texture> &dst) {
	uint32_t levels = math::min(src->levels(), dst->levels());
	for (uint32_t i = 0; i < levels; i++)
		copy_texture_level(src, dst, i, i);
}

void copy_executor::copy_texture_level(
		const std::shared_ptr<gpu::texture> &src,
		const std::shared_ptr<gpu::texture> &dst,
		uint32_t src_level,
		uint32_t dst_level,
		math::uvec3 src_offset,
		math::uvec3 dst_offset,
		math::uvec3 size) {
	impl.copy_texture_level(src, dst, src_level, dst_level, src_offset, dst_offset, size);
}

void compute_executor::compute_pass(const compute_pass_info &info,
			const std::function<void(gpu::compute_pass &)> &callback) {
	impl.compute_pass(info, callback);
}

void graphics_executor::rasterization_pass(const rasterization_pass_info &info,
		const std::function<void(gpu::rasterization_pass &)> &callback) {
	impl.rasterization_pass(info, callback);
}

}

namespace volt::gpu::_internal {

template<typename Executor>
const std::shared_ptr<gpu::device> &routine<Executor>::device() {
	return _device;
}

template<typename Executor>
routine<Executor>::routine(std::shared_ptr<gpu::device> &&device)
		: _device(std::move(device)) {}

template class routine<graphics_executor>;
template class routine<compute_executor>;
template class routine<copy_executor>;

}
