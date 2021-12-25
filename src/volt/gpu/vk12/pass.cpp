#include <volt/gpu/vk12/pass.hpp>

#include <volt/gpu/vk12/buffer.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/sampler.hpp>
#include <volt/gpu/vk12/shader.hpp>
#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/math/math.hpp>

#include <initializer_list>

namespace volt::gpu::vk12 {

static std::unordered_map<VkFormat, uint8_t> format_sizes{
	{ VK_FORMAT_R32_SFLOAT, 4 }, // float
	{ VK_FORMAT_R32G32_SFLOAT, 8 },
	{ VK_FORMAT_R32G32B32_SFLOAT, 12 },
	{ VK_FORMAT_R32G32B32A32_SFLOAT, 16 },
	{ VK_FORMAT_R32_SINT, 4 }, // int
	{ VK_FORMAT_R32G32_SINT, 8 },
	{ VK_FORMAT_R32G32B32_SINT, 12 },
	{ VK_FORMAT_R32G32B32A32_SINT, 16 },
	{ VK_FORMAT_R32_UINT, 4 }, // uint
	{ VK_FORMAT_R32G32_UINT, 8 },
	{ VK_FORMAT_R32G32B32_UINT, 12 },
	{ VK_FORMAT_R32G32B32A32_UINT, 16 },
	{ VK_FORMAT_R64_SFLOAT, 8 }, // double
	{ VK_FORMAT_R64G64_SFLOAT, 16 },
	{ VK_FORMAT_R64G64B64_SFLOAT, 24 },
	{ VK_FORMAT_R64G64B64A64_SFLOAT, 32 }
};

static std::unordered_map<gpu::topology, VkPrimitiveTopology> primitive_topology{
	{ gpu::topology::triangles, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
	{ gpu::topology::lines, VK_PRIMITIVE_TOPOLOGY_LINE_LIST },
	{ gpu::topology::points, VK_PRIMITIVE_TOPOLOGY_POINT_LIST }
};

static std::unordered_map<gpu::topology, VkPolygonMode> polygon_modes{
	{ gpu::topology::triangles, VK_POLYGON_MODE_FILL },
	{ gpu::topology::lines, VK_POLYGON_MODE_LINE },
	{ gpu::topology::points, VK_POLYGON_MODE_POINT }
};

static std::unordered_map<gpu::attachment_initializer, VkAttachmentLoadOp> load_ops{
	{ gpu::attachment_initializer::none, VK_ATTACHMENT_LOAD_OP_DONT_CARE },
	{ gpu::attachment_initializer::clear, VK_ATTACHMENT_LOAD_OP_CLEAR },
	{ gpu::attachment_initializer::preserve, VK_ATTACHMENT_LOAD_OP_LOAD }
};

template<typename Pass, typename PassInfo>
static void create_sets_and_pipeline(Pass &pass, const PassInfo &info, const std::vector<vk12::shader *> &shaders) {
	// Accumulate descriptor set layout data from all shaders
	uint32_t last_set = 0;
	for (vk12::shader *shader : shaders) {
		for (vk12::shader::set_layout_info info : shader->set_layout_infos) {
			last_set = math::max(last_set, info.set);
			pass_set_layout_info &set_layout_create_info = pass.pass_set_layout_infos[info.set];

			set_layout_create_info.bindings.insert(
				set_layout_create_info.bindings.end(),
				info.bindings.begin(),
				info.bindings.end()
			);

			set_layout_create_info.info.bindingCount = static_cast<uint32_t>(set_layout_create_info.bindings.size());
			set_layout_create_info.info.pBindings = set_layout_create_info.bindings.data();
		}

		// Accumulate binding points
		pass.binding_points.insert(shader->binding_points.begin(), shader->binding_points.end());
	}

	// Create descriptor set layouts
	pass.set_layouts.resize(last_set + 1);
	for (uint32_t i = 0; i < pass.set_layouts.size(); i++) {
		vkCreateDescriptorSetLayout(pass.impl.vk12_device.vk_device,
				&pass.pass_set_layout_infos[i].info, nullptr, &pass.set_layouts[i]);
	}

	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(pass.set_layouts.size());
    pipeline_layout_info.pSetLayouts = pass.set_layouts.data();

	VOLT_VK12_CHECK(vkCreatePipelineLayout(pass.impl.vk12_device.vk_device, &pipeline_layout_info,
			nullptr, &pass.pipeline_layout), "Failed to create pipeline layout.")
}


template<typename Pass>
void update_descriptor_sets(Pass &pass) {
	std::vector<VkDescriptorSet> descriptor_sets = pass.impl._allocate_descriptor_sets(pass.set_layouts,
			pass._constant_buffers.size(), pass._sampled_textures.size(), pass._storage_buffers.size(), pass._storage_textures.size());

	std::vector<VkDescriptorBufferInfo> buffer_infos;
	std::vector<VkDescriptorImageInfo> image_infos;
	std::vector<VkWriteDescriptorSet> writes;

	buffer_infos.resize(pass._constant_buffers.size() + pass._storage_buffers.size());
	image_infos.resize(pass._sampled_textures.size() + pass._storage_textures.size());
	writes.reserve(buffer_infos.size() + image_infos.size());

	for (auto &item : pass._constant_buffers) {
		// Translate slot name to binding point
		vk12::shader::binding_point binding_point = pass.binding_points[item.first];

		auto *vk12_buffer = static_cast<vk12::buffer *>(item.second);

		if constexpr (std::is_same_v<Pass, rasterization_pass>)
			vk12_buffer->barrier(pass.impl.command_buffer, vk12::buffer::state::rasterization_shared);
		else if constexpr (std::is_same_v<Pass, compute_pass>)
			vk12_buffer->barrier(pass.impl.command_buffer, vk12::buffer::state::compute_shared);

		VkDescriptorBufferInfo &buffer_info = buffer_infos[writes.size()];
		buffer_info.buffer = vk12_buffer->vk_buffer;
		buffer_info.offset = 0;
		buffer_info.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.pBufferInfo = &buffer_info;

		writes.push_back(std::move(write));
	}

	for (auto &item : pass._sampled_textures) {
		vk12::shader::binding_point binding_point = pass.binding_points[item.first];

		auto *vk12_texture = static_cast<vk12::texture *>(item.second.first);
		auto *vk12_sampler = static_cast<vk12::sampler *>(item.second.second);

		if constexpr (std::is_same_v<Pass, rasterization_pass>)
			vk12_texture->barrier(pass.impl.command_buffer, vk12::texture::state::rasterization_shared);
		else if constexpr (std::is_same_v<Pass, compute_pass>)
			vk12_texture->barrier(pass.impl.command_buffer, vk12::texture::state::compute_shared);

		VkDescriptorImageInfo &image_info = image_infos[writes.size()];
		image_info.sampler = vk12_sampler->vk_sampler;
		image_info.imageView = vk12_texture->image_view;
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &image_info;	

		writes.push_back(std::move(write));	
	}

	for (auto &item : pass._storage_buffers) {
		vk12::shader::binding_point binding_point = pass.binding_points[item.first];

		auto *vk12_buffer = static_cast<vk12::buffer *>(item.second.first);
		if (item.second.second) {
			if constexpr (std::is_same_v<Pass, rasterization_pass>)
				vk12_buffer->barrier(pass.impl.command_buffer, vk12::buffer::state::rasterization_shared);
			else if constexpr (std::is_same_v<Pass, compute_pass>)
				vk12_buffer->barrier(pass.impl.command_buffer, vk12::buffer::state::compute_shared);
		} else {
			if constexpr (std::is_same_v<Pass, rasterization_pass>)
				vk12_buffer->barrier(pass.impl.command_buffer, vk12::buffer::state::rasterization_unique);
			else if constexpr (std::is_same_v<Pass, compute_pass>)
				vk12_buffer->barrier(pass.impl.command_buffer, vk12::buffer::state::compute_unique);
		}

		VkDescriptorBufferInfo &buffer_info = buffer_infos[writes.size()];
		buffer_info.buffer = vk12_buffer->vk_buffer;
		buffer_info.offset = 0;
		buffer_info.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.pBufferInfo = &buffer_info;	

		writes.push_back(std::move(write));	
	}

	for (auto &item : pass._storage_textures) {
		vk12::shader::binding_point binding_point = pass.binding_points[item.first];

		auto *vk12_texture = static_cast<vk12::texture *>(item.second.first);
		if (item.second.second) {
			if constexpr (std::is_same_v<Pass, rasterization_pass>)
				vk12_texture->barrier(pass.impl.command_buffer, vk12::texture::state::rasterization_shared);
			else if constexpr (std::is_same_v<Pass, compute_pass>)
				vk12_texture->barrier(pass.impl.command_buffer, vk12::texture::state::compute_shared);
		} else {
			if constexpr (std::is_same_v<Pass, rasterization_pass>)
				vk12_texture->barrier(pass.impl.command_buffer, vk12::texture::state::rasterization_unique);
			else if constexpr (std::is_same_v<Pass, compute_pass>)
				vk12_texture->barrier(pass.impl.command_buffer, vk12::texture::state::compute_unique);
		}

		VkDescriptorImageInfo &image_info = image_infos[writes.size()];
		image_info.sampler = VK_NULL_HANDLE;
		image_info.imageView = vk12_texture->image_view;
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		write.pImageInfo = &image_info;

		writes.push_back(std::move(write));	
	}
	
	vkUpdateDescriptorSets(pass.impl.vk12_device.vk_device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

	if constexpr (std::is_same_v<Pass, rasterization_pass>) {
		vkCmdBindDescriptorSets(pass.impl.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pass.pipeline_layout, 0,
				static_cast<uint32_t>(descriptor_sets.size()), descriptor_sets.data(), 0, nullptr);
	} else if constexpr (std::is_same_v<Pass, compute_pass>) {
		vkCmdBindDescriptorSets(pass.impl.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pass.pipeline_layout, 0,
				static_cast<uint32_t>(descriptor_sets.size()), descriptor_sets.data(), 0, nullptr);
	}
}

rasterization_pass::rasterization_pass(vk12::routine_impl &impl, const rasterization_pass_info &info) : impl(impl) {
	std::vector<vk12::shader *> shaders;

	// Collect shaders into a list
	shaders.push_back(static_cast<vk12::shader *>(info.vertex_shader.get()));
	if (info.hull_shader != nullptr)
		shaders.push_back(static_cast<vk12::shader *>(info.hull_shader.get()));
	if (info.domain_shader != nullptr)
		shaders.push_back(static_cast<vk12::shader *>(info.domain_shader.get()));
	if (info.geometry_shader != nullptr)
		shaders.push_back(static_cast<vk12::shader *>(info.geometry_shader.get()));
	shaders.push_back(static_cast<vk12::shader *>(info.pixel_shader.get()));

	create_sets_and_pipeline(*this, info, shaders);
	
	// Extract an array of VkPipelineShaderStageCreateInfos for pipeline info
	std::vector<VkPipelineShaderStageCreateInfo> stage_infos(shaders.size());
	std::transform(shaders.begin(), shaders.end(), stage_infos.begin(),
            [](vk12::shader *shader) { return shader->stage_info; });

	std::unordered_map<std::string, vk12::shader::vertex_input> &vertex_inputs = shaders[0]->vertex_inputs;
	std::vector<VkVertexInputAttributeDescription> attributes;
	attributes.reserve(vertex_inputs.size());

	// Collect inputs and classify per-vertex and per-instance data
	for (auto &item : vertex_inputs) {
		attributes.emplace_back();
		auto &attribute = attributes.back();

		attribute.binding = info.instance_inputs.contains(item.first) ? 1 : 0;
		attribute.location = item.second.location;
		attribute.format = item.second.format;
		// attribute.offset = ...; - done after sorting
	}

	// Sort inputs by location in ascending order
	std::sort(
		attributes.begin(),
		attributes.end(),
		[](auto &a, auto &b) {
			return a.location < b.location;
		}
	);

	uint32_t vertex_offset = 0, index_offset = 0;

	// Count input offsets
	for (auto &attribute : attributes) {
		if (attribute.binding == 0) {
			attribute.offset = vertex_offset;
			vertex_offset += format_sizes[attribute.format];
		} else {
			attribute.offset = index_offset;
			index_offset += format_sizes[attribute.format];
		}
	}

	// One buffer for vertex inputs and another for instance inputs
	VkVertexInputBindingDescription bindings[2];

	bindings[0].binding = 0;
	bindings[0].stride = vertex_offset;
	bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	bindings[1].binding = 1;
	bindings[1].stride = index_offset;
	bindings[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	// Finally, this is the combined input configuration
	VkPipelineVertexInputStateCreateInfo input_state{};
	input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	input_state.vertexBindingDescriptionCount = 2;
	input_state.pVertexBindingDescriptions = bindings;
	input_state.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
	input_state.pVertexAttributeDescriptions = attributes.data();

	// Topology
	VkPipelineInputAssemblyStateCreateInfo assembly_state{};
	assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assembly_state.topology = primitive_topology[info.primitive_mode];
	assembly_state.primitiveRestartEnable = VK_FALSE;

	// Tessellation
	// VkPipelineTessellationStateCreateInfo tessellation_state{};
	// tessellation_state.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	// tessellation_state.patchControlPoints = 3;

	VkPipelineViewportStateCreateInfo viewport_state{};
	viewport_state.viewportCount = 1;
	viewport_state.scissorCount = 1;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterization_state{};
	rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state.polygonMode = polygon_modes[info.polygon_mode];
	rasterization_state.cullMode = info.culling ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE; // Just reverse the order of vertices to achieve VK_CULL_MODE_FRONT_BIT
	rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // The only reasonable value
	// rasterization_state.lineWidth = info.line_width; TODO: Set dynamically

	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisample_state{};
	multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Depth and stencil testing
	VkStencilOpState stencil_op{
		.failOp = VK_STENCIL_OP_KEEP,
		.passOp = VK_STENCIL_OP_REPLACE,
		.depthFailOp = VK_STENCIL_OP_KEEP, // Just disable depth testing to get VK_STENCIL_OP_REPLACE
		.compareOp = info.stencil_write ? VK_COMPARE_OP_ALWAYS : VK_COMPARE_OP_EQUAL,
		.compareMask = 1,
		.writeMask = info.stencil_write ? 1Ui32 : 0Ui32,
		.reference = 1
	};

	// Boolean stencil test
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state{};
	depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_state.depthTestEnable = info.depth_test;
	depth_stencil_state.depthWriteEnable = info.depth_write;
	depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_state.stencilTestEnable = info.stencil_test;
	depth_stencil_state.front = stencil_op;
	depth_stencil_state.back = stencil_op;

	// Color blending
	VkPipelineColorBlendAttachmentState attachment_blend_state{};
	attachment_blend_state.blendEnable = VK_TRUE;
	if (info.blending == gpu::blending::alpha) {
		attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
		attachment_blend_state.colorBlendOp = VK_BLEND_OP_ADD;
		attachment_blend_state.colorWriteMask = 0b111; // Do not write alpha
	} else if (info.blending == gpu::blending::add) {
		attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
		attachment_blend_state.colorBlendOp = VK_BLEND_OP_ADD;
		attachment_blend_state.colorWriteMask = 0b1111;
	} else { // multiply
		attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
		attachment_blend_state.colorBlendOp = VK_BLEND_OP_MULTIPLY_EXT;
		attachment_blend_state.colorWriteMask = 0b1111;
	}

	VkPipelineColorBlendStateCreateInfo blend_state{};
	blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blend_state.attachmentCount = 1;
	blend_state.pAttachments = &attachment_blend_state;

	// Dynamic state
	VkDynamicState dynamic_states[2]{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state{};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = 2;
	dynamic_state.pDynamicStates = dynamic_states;

	// Render pass
	std::vector<VkAttachmentDescription> attachment_infos(info.color_attachments.size() + (info.depth_stencil_attachment.texture != nullptr));
	std::vector<VkAttachmentReference> attachment_refs(attachment_infos.size());

	for (uint32_t i = 0; i < info.color_attachments.size(); i++) {
		auto &attachment = info.color_attachments[i];
		auto &attachment_info = attachment_infos[i];
		auto &attachment_ref = attachment_refs[i];
		VkAttachmentLoadOp load_op = load_ops[attachment.initializer];

		auto &vk12_texture = *static_cast<vk12::texture *>(info.depth_stencil_attachment.texture.get());
		vk12_texture.barrier(impl.command_buffer, vk12::texture::state::color_attachment);

		attachment_info.format = vk12_texture.vk_format;
		attachment_info.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment_info.loadOp = load_op;
		attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment_info.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment_info.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO: Utilize potential transition - we need to predict the future

		attachment_ref.attachment = i;
		attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	if (info.depth_stencil_attachment.texture != nullptr) {
		auto &attachment_info = attachment_infos.back();
		auto &attachment_ref = attachment_refs.back();
		VkAttachmentLoadOp load_op = load_ops[info.depth_stencil_attachment.initializer];

		auto &vk12_texture = *static_cast<vk12::texture *>(info.depth_stencil_attachment.texture.get());
		vk12_texture.barrier(impl.command_buffer, vk12::texture::state::depth_stencil_attachment);

		attachment_info.format = vk12_texture.vk_format;
		attachment_info.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment_info.loadOp = load_op;
		attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment_info.stencilLoadOp = load_op;
		attachment_info.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment_info.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachment_info.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachment_ref.attachment = attachment_refs.size() - 1;
		attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkSubpassDescription subpass_info{};
	subpass_info.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_info.colorAttachmentCount = info.color_attachments.size();
	subpass_info.pColorAttachments = attachment_refs.data();
	subpass_info.pDepthStencilAttachment = info.depth_stencil_attachment.texture != nullptr ? &attachment_refs.back() : nullptr;

	VkRenderPassCreateInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = static_cast<uint32_t>(attachment_infos.size());
	render_pass_info.pAttachments = attachment_infos.data();
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass_info;

	vkCreateRenderPass(impl.vk12_device.vk_device, &render_pass_info, nullptr, &render_pass);

	VkGraphicsPipelineCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = static_cast<uint32_t>(stage_infos.size());
	pipeline_info.pStages = stage_infos.data();
	pipeline_info.pVertexInputState = &input_state;
	pipeline_info.pInputAssemblyState = &assembly_state;
	// pipeline_info.pTessellationState = &tessellation_state;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pRasterizationState = &rasterization_state;
	pipeline_info.pMultisampleState = &multisample_state;
	pipeline_info.pDepthStencilState = &depth_stencil_state;
	pipeline_info.pColorBlendState = &blend_state;
	pipeline_info.pDynamicState = &dynamic_state;
	pipeline_info.layout = pipeline_layout;
	pipeline_info.renderPass = render_pass;
	
	// TODO: Fill out this struct

	VOLT_VK12_CHECK(vkCreateGraphicsPipelines(impl.vk12_device.vk_device, impl.vk12_device.pipeline_cache,
			1, &pipeline_info, nullptr, &pipeline), "Failed to create graphics pipeline.")
}

rasterization_pass::~rasterization_pass() {
	vkDestroyPipeline(impl.vk12_device.vk_device, pipeline, nullptr);
	vkDestroyRenderPass(impl.vk12_device.vk_device, render_pass, nullptr);
	vkDestroyPipelineLayout(impl.vk12_device.vk_device, pipeline_layout, nullptr);
	for (VkDescriptorSetLayout layout : set_layouts)
		vkDestroyDescriptorSetLayout(impl.vk12_device.vk_device, layout, nullptr);
}

void rasterization_pass::draw(uint32_t index_count, uint32_t instance_count) {
	update_descriptor_sets(*this);
	vkCmdDrawIndexed(impl.command_buffer, index_count, instance_count, 0, 0, 0);
}

void rasterization_pass::index_buffer(const std::shared_ptr<gpu::buffer> &buffer) {
	auto &vk12_buffer = *static_cast<vk12::buffer *>(buffer.get());
	vk12_buffer.barrier(impl.command_buffer, vk12::buffer::state::index_input);

	vkCmdBindIndexBuffer(impl.command_buffer, vk12_buffer.vk_buffer, 0, VK_INDEX_TYPE_UINT32);
}

void rasterization_pass::vertex_buffer(const std::shared_ptr<gpu::buffer> &buffer) {
	auto &vk12_buffer = *static_cast<vk12::buffer *>(buffer.get());
	vk12_buffer.barrier(impl.command_buffer, vk12::buffer::state::vertex_input);

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(impl.command_buffer, 0, 1, &vk12_buffer.vk_buffer, &offset);
}

void rasterization_pass::instance_buffer(const std::shared_ptr<gpu::buffer> &buffer) {
	auto &vk12_buffer = *static_cast<vk12::buffer *>(buffer.get());
	vk12_buffer.barrier(impl.command_buffer, vk12::buffer::state::vertex_input);

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(impl.command_buffer, 1, 1, &vk12_buffer.vk_buffer, &offset);
}

void rasterization_pass::viewport(gpu::viewport viewport) {
	VkViewport vk_viewport{
		.x = viewport.width.x,
		.y = viewport.height.x,
		.width = viewport.width.y - viewport.width.x,
		.height = viewport.height.y - viewport.height.x,
		.minDepth = viewport.depth.x,
		.maxDepth = viewport.depth.y
	};

	VkRect2D vk_scissor{
		.offset = {
			static_cast<int32_t>(viewport.width.x + 0.5F),
			static_cast<int32_t>(viewport.height.x + 0.5F)
		},
		.extent = {
			static_cast<uint32_t>(viewport.width.y + 0.5F),
			static_cast<uint32_t>(viewport.height.y + 0.5F)
		},
	};

	vkCmdSetViewport(impl.command_buffer, 0, 1, &vk_viewport);
	vkCmdSetScissor(impl.command_buffer, 0, 1, &vk_scissor);
}

compute_pass::compute_pass(vk12::routine_impl &impl, const compute_pass_info &info) : impl(impl) {
	auto compute_shader = static_cast<vk12::shader *>(info.compute_shader.get());

	create_sets_and_pipeline(*this, info, {
		compute_shader
	});

	VkComputePipelineCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipeline_info.stage = compute_shader->stage_info;
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
	update_descriptor_sets(*this);
	vkCmdDispatch(impl.command_buffer, group_count.x, group_count.y, group_count.z);
}

}
