#include <volt/gpu/vk12/pass.hpp>

#include <volt/gpu/vk12/buffer.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/shader.hpp>
#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/math/math.hpp>

namespace volt::gpu::vk12 {

rasterization_pass::rasterization_pass(vk12::routine_impl &impl) : impl(impl) {}

void rasterization_pass::draw(uint32_t index_count, uint32_t instance_count) {

}

void rasterization_pass::index_buffer(uint32_t location, const std::shared_ptr<gpu::buffer> &buffer) {

}

void rasterization_pass::vertex_buffer(uint32_t location, const std::shared_ptr<gpu::buffer> &buffer) {

}

compute_pass::compute_pass(vk12::routine_impl &impl, const compute_pass_info &info) : impl(impl) {
	auto &shader = *static_cast<vk12::shader *>(info._compute_shader.get());

	// Accumulate descriptor set layout data from all shaders
	uint32_t last_set = 0;
	for (vk12::shader::set_layout_info info : shader.set_layout_infos) {
		last_set = math::max(last_set, info.set);
		set_layout_create_info &set_layout_create_info = set_layout_create_infos[info.set];

		set_layout_create_info.bindings.insert(
			set_layout_create_info.bindings.end(),
			info.bindings.begin(),
			info.bindings.end()
		);

		set_layout_create_info.info.bindingCount = static_cast<uint32_t>(set_layout_create_info.bindings.size());
		set_layout_create_info.info.pBindings = set_layout_create_info.bindings.data();
	}

	// Create descriptor set layouts
	set_layouts.resize(last_set + 1);
	for (uint32_t i = 0; i <= last_set; i++)
		vkCreateDescriptorSetLayout(impl.vk12_device.vk_device, &set_layout_create_infos[i].info, nullptr, &set_layouts[i]);

	// Accumulate binding points
	binding_points.insert(shader.binding_points.begin(), shader.binding_points.end());

	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    pipeline_layout_info.pSetLayouts = set_layouts.data();

	VOLT_VK12_CHECK(vkCreatePipelineLayout(impl.vk12_device.vk_device, &pipeline_layout_info,
			nullptr, &pipeline_layout), "Failed to create pipeline layout.")

	VkPipelineShaderStageCreateInfo stage_info{};
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage_info.module = shader.shader_module;
    stage_info.pName = "main";

	VkComputePipelineCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_info.stage = stage_info;
    pipeline_info.layout = pipeline_layout;

	VOLT_VK12_CHECK(vkCreateComputePipelines(impl.vk12_device.vk_device, impl.vk12_device.pipeline_cache,
			1, &pipeline_info, nullptr, &pipeline), "Failed to create compute pipeline.")
}

compute_pass::~compute_pass() {
	vkDestroyPipeline(impl.vk12_device.vk_device, pipeline, nullptr);
	vkDestroyPipelineLayout(impl.vk12_device.vk_device, pipeline_layout, nullptr);
	for (VkDescriptorSetLayout layout : set_layouts)
		vkDestroyDescriptorSetLayout(impl.vk12_device.vk_device, layout, nullptr);
}

void compute_pass::dispatch(math::uvec3 group_count) {
	std::vector<VkDescriptorSet> descriptor_sets = impl._allocate_descriptor_sets(set_layouts,
			constant_buffers.size(), sampled_textures.size(), storage_buffers.size(), storage_textures.size());

	std::vector<VkWriteDescriptorSet> writes;
	writes.reserve(constant_buffers.size() + sampled_textures.size()
			+ storage_buffers.size() + storage_textures.size());

	for (auto &item : constant_buffers) {
		// Translate slot name to binding point
		vk12::shader::binding_point binding_point = binding_points[item.first];

		auto *vk12_buffer = static_cast<vk12::buffer *>(item.second);
		vk12_buffer->barrier(impl.command_buffer, vk12::buffer::state::compute_shared);

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.pBufferInfo = &vk12_buffer->descriptor_info;

		writes.push_back(std::move(write));
	}

	for (auto &item : sampled_textures) {
		// Translate slot name to binding point
		vk12::shader::binding_point binding_point = binding_points[item.first];

		auto *vk12_texture = static_cast<vk12::texture *>(item.second);
		vk12_texture->barrier(impl.command_buffer, vk12::texture::state::compute_shared);

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &vk12_texture->descriptor_info;	

		writes.push_back(std::move(write));	
	}

	for (auto &item : storage_buffers) {
		// Translate slot name to binding point
		vk12::shader::binding_point binding_point = binding_points[item.first];

		auto *vk12_buffer = static_cast<vk12::buffer *>(item.second.first);
		if (item.second.second)
			vk12_buffer->barrier(impl.command_buffer, vk12::buffer::state::compute_shared);
		else
			vk12_buffer->barrier(impl.command_buffer, vk12::buffer::state::compute_unique);

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.pBufferInfo = &vk12_buffer->descriptor_info;	

		writes.push_back(std::move(write));	
	}

	for (auto &item : storage_textures) {
		// Translate slot name to binding point
		vk12::shader::binding_point binding_point = binding_points[item.first];

		auto *vk12_texture = static_cast<vk12::texture *>(item.second.first);
		if (item.second.second)
			vk12_texture->barrier(impl.command_buffer, vk12::texture::state::compute_shared);
		else
			vk12_texture->barrier(impl.command_buffer, vk12::texture::state::compute_unique);

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		write.pImageInfo = &vk12_texture->descriptor_info;	

		writes.push_back(std::move(write));	
	}
	
	vkUpdateDescriptorSets(impl.vk12_device.vk_device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

	vkCmdBindDescriptorSets(impl.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout, 0,
			static_cast<uint32_t>(descriptor_sets.size()), descriptor_sets.data(), 0, nullptr);

	vkCmdDispatch(impl.command_buffer, group_count.x, group_count.y, group_count.z);
}

}
