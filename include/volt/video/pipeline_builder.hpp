#pragma once

#include "../macros.hpp"

#include <memory>

#include "enums.hpp"

namespace volt::video {

class device;

class pipeline_builder {
public:
	VOLT_API pipeline_builder(pipeline_type type);

	void vertex_input(size_t vertex_size, size_t offset, , location, vertex_input_format format, bool per_instance);

	// reserve compute stage for compute pipeline
	void shader(shader_stage stage, const std::shared_ptr<shader> &shader);

	void binding(binding binding);

	void build();
};

}
