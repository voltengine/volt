#include <volt/pch.hpp>
#include <volt/gpu/routine.hpp>

namespace volt::gpu {

// RoutineContexts

copy_routine_context::copy_routine_context(_internal::routine_impl &impl)
		: impl(impl) {}

compute_routine_context::compute_routine_context(_internal::routine_impl &impl)
		: copy_routine_context(impl) {}

universal_routine_context::universal_routine_context(_internal::routine_impl &impl)
		: compute_routine_context(impl) {}

copy_routine_context copy_routine_context::_new(_internal::routine_impl &impl) {
	return copy_routine_context(impl);
}

compute_routine_context compute_routine_context::_new(_internal::routine_impl &impl) {
	return compute_routine_context(impl);
}

universal_routine_context universal_routine_context::_new(_internal::routine_impl &impl) {
	return universal_routine_context(impl);
}

void copy_routine_context::copy_buffer(
		const std::shared_ptr<gpu::buffer> &src,
		const std::shared_ptr<gpu::buffer> &dst,
		size_t src_offset,
		size_t dst_offset,
		size_t size) {
	impl.copy_buffer(src, dst, src_offset, dst_offset, size);
}

void copy_routine_context::copy_texture(
		const std::shared_ptr<gpu::texture> &src,
		const std::shared_ptr<gpu::texture> &dst) {
	#ifdef VOLT_GPU_DEBUG
		VOLT_ASSERT(src->format() == dst->format(), "Textures must be of the same format to perform a copy.")
		VOLT_ASSERT(math::all(src->size() == dst->size()), "Textures must be of the same size to perform a full copy.")
	#endif

	impl.copy_texture(src, dst);
}

void copy_routine_context::copy_texture_level(
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

void compute_routine_context::dispatch(const dispatch_info &info) {
	impl.dispatch(info);
}

void universal_routine_context::pass(const pass_info &info,
		const std::function<void(gpu::pass_context &)> &callback) {
	impl.pass(info, callback);
}

void universal_routine_context::async_pass(const pass_info &info,
		const std::function<void(gpu::async_pass_context &)> &callback) {
	impl.async_pass(info, callback);
}

}

namespace volt::gpu::_internal {

template<typename RoutineContext>
const std::shared_ptr<gpu::device> &routine<RoutineContext>::device() {
	return _device;
}

template<typename RoutineContext>
routine<RoutineContext>::routine(std::shared_ptr<gpu::device> &&device)
		: _device(std::move(device)) {}

template class routine<universal_routine_context>;
template class routine<compute_routine_context>;
template class routine<copy_routine_context>;

}
