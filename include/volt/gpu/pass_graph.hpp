#pragma once

#include "../macros.hpp"

#include "pass.hpp"

namespace volt::gpu {

class pass_graph {
public:
	void add_pass(rasterization_pass &&pass) {
		task task{
			.type = task::type::rasterization_pass,
			.index = rasterization_passes.size()
		};
		tasks.push_back(std::move(task));
		rasterization_passes.push_back(std::move(pass));
	}
	
	void present() {
		tasks.push_back(std::move(task));
		rasterization_passes.push_back(std::move(pass));
	}

	virtual void execute() {
		if (!built && (built = true))
			build();
	}

protected:
	struct task {
		enum class type {
			rasterization_pass,
			compute_pass,
			copy_op,
			present_op
		};

		type type;
		size_t index;
	};

	std::vector<task> tasks;
	std::vector<rasterization_pass> rasterization_passes;
	// std::vector<compute_pass> compute_passes;
	// std::vector<copy_op> copy_ops;
	std::vector<present_op> present_ops;
	bool built = false;

	virtual void build() = 0;
};

}
