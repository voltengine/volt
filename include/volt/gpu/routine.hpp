#pragma once

#include "../macros.hpp"

#include <functional>
#include <memory>

#include "../math/math.hpp"
#include "enums.hpp"
#include "pass.hpp"

namespace volt::gpu::_internal {

class routine_impl {
public:
	virtual void copy_buffer(
			const std::shared_ptr<gpu::buffer> &src,
			const std::shared_ptr<gpu::buffer> &dst,
			size_t src_offset,
			size_t dst_offset,
			size_t size) = 0;

	virtual void copy_texture_level(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst,
			uint32_t src_level,
			uint32_t dst_level,
			math::uvec3 src_offset,
			math::uvec3 dst_offset,
			math::uvec3 size) = 0;

	virtual void compute_pass(const compute_pass_info &info,
			const std::function<void(gpu::compute_pass &)> &callback) = 0;

	virtual void rasterization_pass(const rasterization_pass_info &info,
			const std::function<void(gpu::rasterization_pass &)> &callback) = 0;
};

}

namespace volt::gpu {

class copy_executor {
public:
	VOLT_API virtual ~copy_executor() = default;

	VOLT_API static copy_executor _new(_internal::routine_impl &impl);

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

	copy_executor(_internal::routine_impl &impl);
};

class compute_executor : public copy_executor {
public:
	VOLT_API virtual ~compute_executor() = default;

	VOLT_API static compute_executor _new(_internal::routine_impl &impl);

	VOLT_API void compute_pass(const compute_pass_info &info, const std::
			function<void(gpu::compute_pass &)> &callback);

protected:
	compute_executor(_internal::routine_impl &impl);
};

class graphics_executor : public compute_executor {
public:
	VOLT_API static graphics_executor _new(_internal::routine_impl &impl);

	VOLT_API void rasterization_pass(const rasterization_pass_info &info, const
			std::function<void(gpu::rasterization_pass &)> &callback);

private:
	graphics_executor(_internal::routine_impl &impl);
};

}

namespace volt::gpu::_internal {

template<typename Executor>
class routine {
public:
	virtual ~routine() = default;

	virtual void execute(const std::function<void(Executor &)> &callback) = 0;

	virtual bool finished() = 0;

	virtual void wait() = 0;

	const std::shared_ptr<gpu::device> &device();

protected:
	std::shared_ptr<gpu::device> _device;

	routine(std::shared_ptr<gpu::device> &&device);
};

}

namespace volt::gpu {

using graphics_routine = _internal::routine<gpu::graphics_executor>;
using compute_routine = _internal::routine<gpu::compute_executor>;
using copy_routine = _internal::routine<gpu::copy_executor>;

}
