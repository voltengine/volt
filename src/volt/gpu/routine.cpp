#include <volt/pch.hpp>
#include <volt/gpu/routine.hpp>

namespace volt::gpu {

// Executors

copy_executor::copy_executor(_internal::routine_impl &impl)
		: impl(impl) {}

compute_executor::compute_executor(_internal::routine_impl &impl)
		: copy_executor(impl) {}

universal_executor::universal_executor(_internal::routine_impl &impl)
		: compute_executor(impl) {}

copy_executor copy_executor::_new(_internal::routine_impl &impl) {
	return copy_executor(impl);
}

compute_executor compute_executor::_new(_internal::routine_impl &impl) {
	return compute_executor(impl);
}

universal_executor universal_executor::_new(_internal::routine_impl &impl) {
	return universal_executor(impl);
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
	#ifdef VOLT_GPU_DEBUG
		VOLT_ASSERT(src->format() == dst->format(), "Textures must be of the same format to perform a copy.")
		VOLT_ASSERT(math::all(src->size() == dst->size()), "Textures must be of the same size to perform a full copy.")
	#endif

	impl.copy_texture(src, dst);
}

void copy_executor::copy_texture_level(
		const std::shared_ptr<gpu::texture> &src,
		const std::shared_ptr<gpu::texture> &dst,
		uint32_t src_level,
		uint32_t dst_level,
		math::uvec3 src_offset,
		math::uvec3 dst_offset,
		math::uvec3 size) {
	#ifdef VOLT_GPU_DEBUG
		VOLT_ASSERT(src->format() == dst->format(), "Textures must be of the same format to perform a copy.")
	#endif
	
	impl.copy_texture_level(src, dst, src_level, dst_level, src_offset, dst_offset, size);
}

void compute_executor::compute_pass(const compute_pass_info &info,
			const std::function<void(gpu::compute_pass &)> &callback) {
	impl.compute_pass(info, callback);
}

void universal_executor::rasterization_pass(const rasterization_pass_info &info,
		const std::function<void(gpu::rasterization_pass &)> &callback) {
	impl.rasterization_pass(info, callback);
}

}

namespace volt::gpu::_internal {

template<typename Executor>
routine<Executor>::~routine() {
	wait();
}

template<typename Executor>
const std::shared_ptr<gpu::device> &routine<Executor>::device() {
	return _device;
}

template<typename Executor>
routine<Executor>::routine(std::shared_ptr<gpu::device> &&device)
		: _device(std::move(device)) {}

template class routine<universal_executor>;
template class routine<compute_executor>;
template class routine<copy_executor>;

}
