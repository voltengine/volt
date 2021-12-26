#pragma once

#include "../macros.hpp"

#include <functional>
#include <memory>

#include "../math/math.hpp"
#include "buffer.hpp"
#include "enums.hpp"
#include "pass.hpp"
#include "texture.hpp"

namespace volt::gpu {

struct dispatch_info {
	std::vector<constant_buffer_binding> constant_buffers;
	std::vector<sampled_texture_binding> sampled_textures;
	std::vector<storage_buffer_binding> storage_buffers;
	std::vector<storage_texture_binding> storage_textures;
	gpu::shader *compute_shader;
	math::uvec3 group_count;
};

}

namespace volt::gpu::_internal {

class routine_impl {
public:
	virtual void copy_buffer(
			const std::shared_ptr<gpu::buffer> &src,
			const std::shared_ptr<gpu::buffer> &dst,
			size_t src_offset,
			size_t dst_offset,
			size_t size) = 0;

	virtual void copy_texture(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst) = 0;

	virtual void copy_texture_level(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst,
			uint32_t src_level,
			uint32_t dst_level,
			math::uvec3 src_offset,
			math::uvec3 dst_offset,
			math::uvec3 size) = 0;

	virtual void dispatch(const dispatch_info &info) = 0;

	virtual void pass(const pass_info &info,
			const std::function<void(gpu::pass_context &)> &callback) = 0;

	virtual void async_pass(const pass_info &info,
			const std::function<void(gpu::async_pass_context &)> &callback) = 0;
};

}

namespace volt::gpu {

class copy_routine_context {
public:
	VOLT_API virtual ~copy_routine_context() = default;

	VOLT_API static copy_routine_context _new(_internal::routine_impl &impl);

	VOLT_API void copy_buffer(
			const std::shared_ptr<gpu::buffer> &src,
			const std::shared_ptr<gpu::buffer> &dst,
			size_t src_offset = 0,
			size_t dst_offset = 0,
			size_t size = 0);

	VOLT_API void copy_texture(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst);

	VOLT_API void copy_texture_level(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst,
			uint32_t src_level,
			uint32_t dst_level,
			math::uvec3 src_offset = math::uvec3::zero,
			math::uvec3 dst_offset = math::uvec3::zero,
			math::uvec3 size = math::uvec3::zero);

protected:
	_internal::routine_impl &impl;

	copy_routine_context(_internal::routine_impl &impl);
};

class compute_routine_context : public copy_routine_context {
public:
	VOLT_API virtual ~compute_routine_context() = default;

	VOLT_API static compute_routine_context _new(_internal::routine_impl &impl);

	VOLT_API void dispatch(const dispatch_info &info);

protected:
	compute_routine_context(_internal::routine_impl &impl);
};

class universal_routine_context : public compute_routine_context {
public:
	VOLT_API static universal_routine_context _new(_internal::routine_impl &impl);

	VOLT_API void pass(const pass_info &info, const
			std::function<void(gpu::pass_context &)> &callback);

	VOLT_API void async_pass(const pass_info &info, const
			std::function<void(gpu::async_pass_context &)> &callback);

private:
	universal_routine_context(_internal::routine_impl &impl);
};

}

namespace volt::gpu::_internal {

template<typename RoutineContext>
class routine {
public:
	virtual ~routine() = default;

	virtual void execute(const std::function<void(RoutineContext &)> &callback) = 0;

	virtual bool finished() = 0;

	virtual void wait() = 0;

	const std::shared_ptr<gpu::device> &device();

protected:
	std::shared_ptr<gpu::device> _device;

	routine(std::shared_ptr<gpu::device> &&device);
};

}

namespace volt::gpu {

using universal_routine = _internal::routine<gpu::universal_routine_context>;
using compute_routine = _internal::routine<gpu::compute_routine_context>;
using copy_routine = _internal::routine<gpu::copy_routine_context>;

}
