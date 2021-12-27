#include <volt/gpu/vk12/jit.hpp>

#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>

namespace volt::gpu::vk12 {

bool operator==(const compute_pipeline_key &lhs, const compute_pipeline_key &rhs) {
	return lhs.compute_shader == rhs.compute_shader;
}

bool operator==(const render_pass_key &lhs, const render_pass_key &rhs) {
	return lhs.color_attachment_formats == rhs.color_attachment_formats
			&& lhs.depth_stencil_attachment_format == rhs.depth_stencil_attachment_format
			&& lhs.color_attachment_initializers == rhs.color_attachment_initializers
			&& lhs.depth_stencil_attachment_initializer == rhs.depth_stencil_attachment_initializer;
}

bool operator==(const rasterization_pipeline_key &lhs, const rasterization_pipeline_key &rhs) {
	return lhs.render_pass == rhs.render_pass
			&& lhs.vertex_shader == rhs.vertex_shader
			&& lhs.hull_shader == rhs.hull_shader
			&& lhs.domain_shader == rhs.domain_shader
			&& lhs.geometry_shader == rhs.geometry_shader
			&& lhs.pixel_shader == rhs.pixel_shader
			&& lhs.instance_inputs == rhs.instance_inputs
			&& lhs.polygon_mode == rhs.polygon_mode
			&& lhs.primitive_mode == rhs.primitive_mode
			&& lhs.culling == rhs.culling
			&& lhs.depth_test == rhs.depth_test
			&& lhs.depth_write == rhs.depth_write
			&& lhs.stencil_test == rhs.stencil_test
			&& lhs.stencil_write == rhs.stencil_write
			&& lhs.blending == rhs.blending;
}

bool operator==(const framebuffer_key &lhs, const framebuffer_key &rhs) {
	return lhs.render_pass == rhs.render_pass
			&& lhs.color_attachments == rhs.color_attachments
			&& lhs.depth_stencil_attachment == rhs.depth_stencil_attachment;
}

}

namespace std {

size_t hash<volt::gpu::vk12::compute_pipeline_key>::operator()(const volt::gpu::vk12::compute_pipeline_key &key) const {
	return reinterpret_cast<size_t>(key.compute_shader);
}

size_t hash<volt::gpu::vk12::render_pass_key>::operator()(const volt::gpu::vk12::render_pass_key &key) const {
	size_t hash = 0;
	for (auto &format : key.color_attachment_formats) {
		hash ^= static_cast<size_t>(format);
		hash *= 115249; // Prime
	}
	hash ^= static_cast<size_t>(key.depth_stencil_attachment_format);
	hash *= 53; // Prime
	for (auto &initializer : key.color_attachment_initializers) {
		hash ^= static_cast<size_t>(initializer);
		hash *= 21169; // Prime
	}
	hash ^= static_cast<size_t>(key.depth_stencil_attachment_initializer);
	return hash;
}

size_t hash<volt::gpu::vk12::rasterization_pipeline_key>::operator()(const volt::gpu::vk12::rasterization_pipeline_key &key) const {
	size_t hash = 0;
	hash ^= reinterpret_cast<size_t>(key.render_pass);
	hash *= 53; // Prime
	hash ^= reinterpret_cast<size_t>(key.vertex_shader);
	hash *= 719; // Prime
	hash ^= reinterpret_cast<size_t>(key.hull_shader);
	hash *= 89; // Prime
	hash ^= reinterpret_cast<size_t>(key.domain_shader);
	hash *= 991; // Prime
	hash ^= reinterpret_cast<size_t>(key.geometry_shader);
	hash *= 383; // Prime
	hash ^= reinterpret_cast<size_t>(key.pixel_shader);
	hash *= 11; // Prime
	for (auto &input : key.instance_inputs) {
		hash ^= std::hash<std::string>{}(input);
		hash *= 281; // Prime
	}
	
	size_t bitset = static_cast<size_t>(key.polygon_mode);
	bitset |= static_cast<size_t>(key.primitive_mode) << 2;
	bitset |= static_cast<size_t>(key.culling) << 4;
	bitset |= static_cast<size_t>(key.depth_test) << 5;
	bitset |= static_cast<size_t>(key.depth_write) << 6;
	bitset |= static_cast<size_t>(key.stencil_test) << 7;
	bitset |= static_cast<size_t>(key.stencil_write) << 8;
	bitset |= static_cast<size_t>(key.blending) << 9;

	return hash ^ bitset;
}

size_t hash<volt::gpu::vk12::framebuffer_key>::operator()(const volt::gpu::vk12::framebuffer_key &key) const {
	size_t hash = 0;
	hash ^= reinterpret_cast<size_t>(key.render_pass);
	hash *= 53; // Prime
	for (auto &attachment : key.color_attachments) {
		hash ^= reinterpret_cast<size_t>(attachment);
		hash *= 21169; // Prime
	}
	hash ^= reinterpret_cast<size_t>(key.depth_stencil_attachment);
	return hash;
}

}

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

jit::jit(vk12::device &device) : device(device) {}

jit::~jit() {
	clear();
}

vk12::pipeline &jit::compute_pipeline(compute_pipeline_key &&key) {
	// First search in immutable cache
	auto it = immutable_cache.compute_pipelines.find(key);
	if (it != immutable_cache.compute_pipelines.end())
		return it->second;

	// Fall back to mutable cache with locking
	std::unique_lock lock(mutable_cache_mutex);

	it = mutable_cache.compute_pipelines.find(key);
	if (it != mutable_cache.compute_pipelines.end())
		return it->second;

	auto compute_shader = static_cast<vk12::shader *>(key.compute_shader);
	VOLT_ASSERT(compute_shader->stage_info.stage == VK_SHADER_STAGE_COMPUTE_BIT, "Invalid compute shader type.")

	vk12::pipeline pipeline = create_pipeline(key, {
		compute_shader
	});

	VkComputePipelineCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipeline_info.stage = compute_shader->stage_info;
	pipeline_info.layout = pipeline.pipeline_layout;

	VOLT_VK12_CHECK(vkCreateComputePipelines(device.vk_device, nullptr, // TODO: Pipeline Cache
			1, &pipeline_info, nullptr, &pipeline.vk_pipeline), "Failed to create compute pipeline.")

	
	return mutable_cache.compute_pipelines.emplace(std::move(key), std::move(pipeline)).first->second;
}

VkRenderPass jit::render_pass(render_pass_key &&key) {
	// First search in immutable cache
	auto it = immutable_cache.render_passes.find(key);
	if (it != immutable_cache.render_passes.end())
		return it->second;

	// Fall back to mutable cache with locking
	std::unique_lock lock(mutable_cache_mutex);

	it = mutable_cache.render_passes.find(key);
	if (it != mutable_cache.render_passes.end())
		return it->second;

	bool has_depth_stencil_attachment = key.depth_stencil_attachment_format != VK_FORMAT_UNDEFINED;
	
	std::vector<VkAttachmentDescription> attachment_infos(key.color_attachment_formats.size() + has_depth_stencil_attachment);
	std::vector<VkAttachmentReference> attachment_refs(attachment_infos.size());

	for (uint32_t i = 0; i < key.color_attachment_formats.size(); i++) {
		auto &attachment_info = attachment_infos[i];
		auto &attachment_ref = attachment_refs[i];
		VkAttachmentLoadOp load_op = load_ops[key.color_attachment_initializers[i]];

		attachment_info.format = key.color_attachment_formats[i];
		attachment_info.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment_info.loadOp = load_op;
		attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment_info.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment_info.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO: Utilize potential transition - we need to predict the future

		attachment_ref.attachment = i;
		attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	if (has_depth_stencil_attachment) {
		auto &attachment_info = attachment_infos.back();
		auto &attachment_ref = attachment_refs.back();
		VkAttachmentLoadOp load_op = load_ops[key.depth_stencil_attachment_initializer];

		attachment_info.format = key.depth_stencil_attachment_format;
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
	subpass_info.colorAttachmentCount = key.color_attachment_formats.size();
	subpass_info.pColorAttachments = attachment_refs.data();
	subpass_info.pDepthStencilAttachment = has_depth_stencil_attachment ? &attachment_refs.back() : nullptr;

	VkRenderPassCreateInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = static_cast<uint32_t>(attachment_infos.size());
	render_pass_info.pAttachments = attachment_infos.data();
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass_info;

	VkRenderPass render_pass;
	vkCreateRenderPass(device.vk_device, &render_pass_info, nullptr, &render_pass);
	
	return mutable_cache.render_passes.emplace(std::move(key), render_pass).first->second;
}

pipeline &jit::rasterization_pipeline(rasterization_pipeline_key &&key) {
	// First search in immutable cache
	auto it = immutable_cache.rasterization_pipelines.find(key);
	if (it != immutable_cache.rasterization_pipelines.end())
		return it->second;

	// Fall back to mutable cache with locking
	std::unique_lock lock(mutable_cache_mutex);

	it = mutable_cache.rasterization_pipelines.find(key);
	if (it != mutable_cache.rasterization_pipelines.end())
		return it->second;

	// Collect shaders into a list
	std::vector<vk12::shader *> shaders;
	shaders.push_back(static_cast<vk12::shader *>(key.vertex_shader));
	VOLT_ASSERT(shaders.front()->stage_info.stage == VK_SHADER_STAGE_VERTEX_BIT, "Invalid vertex shader type.")

	if (key.hull_shader != nullptr) {
		shaders.push_back(static_cast<vk12::shader *>(key.hull_shader));
		VOLT_ASSERT(shaders.back()->stage_info.stage == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, "Invalid hull shader type.")
	}

	if (key.domain_shader != nullptr) {
		shaders.push_back(static_cast<vk12::shader *>(key.domain_shader));
		VOLT_ASSERT(shaders.back()->stage_info.stage == VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, "Invalid domain shader type.")
	}

	if (key.geometry_shader != nullptr) {
		shaders.push_back(static_cast<vk12::shader *>(key.geometry_shader));
		VOLT_ASSERT(shaders.back()->stage_info.stage == VK_SHADER_STAGE_GEOMETRY_BIT, "Invalid geometry shader type.")
	}

	shaders.push_back(static_cast<vk12::shader *>(key.pixel_shader));
	VOLT_ASSERT(shaders.back()->stage_info.stage == VK_SHADER_STAGE_FRAGMENT_BIT, "Invalid pixel shader type.")

	vk12::pipeline pipeline = create_pipeline(key, shaders);

	// Statect an array of VkPipelineShaderStageCreateInfos for pipeline info
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

		attribute.binding = key.instance_inputs.contains(item.first) ? 1 : 0;
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
	assembly_state.topology = vk12::primitive_topologies[key.primitive_mode];

	// Tessellation
	VkPipelineTessellationStateCreateInfo tessellation_state{};
	tessellation_state.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	tessellation_state.patchControlPoints = 3; // TODO: Support tessellation

	VkPipelineViewportStateCreateInfo viewport_state{};
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.scissorCount = 1;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterization_state{};
	rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state.polygonMode = polygon_modes[key.polygon_mode];
	rasterization_state.cullMode = key.culling ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE; // Just reverse the order of vertices to achieve VK_CULL_MODE_FRONT_BIT
	rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // The only reasonable value
	rasterization_state.lineWidth = 1;

	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisample_state{};
	multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Depth and stencil testing
	VkStencilOpState stencil_op{
		.failOp = VK_STENCIL_OP_KEEP,
		.passOp = VK_STENCIL_OP_REPLACE,
		.depthFailOp = VK_STENCIL_OP_KEEP, // Just disable depth testing to get VK_STENCIL_OP_REPLACE
		.compareOp = key.stencil_write ? VK_COMPARE_OP_ALWAYS : VK_COMPARE_OP_EQUAL,
		.compareMask = 1,
		.writeMask = key.stencil_write ? 1Ui32 : 0Ui32,
		.reference = 1
	};

	// Boolean stencil test
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state{};
	depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_state.depthTestEnable = key.depth_test;
	depth_stencil_state.depthWriteEnable = key.depth_write;
	depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_state.stencilTestEnable = key.stencil_test;
	depth_stencil_state.front = stencil_op;
	depth_stencil_state.back = stencil_op;

	// Color blending
	VkPipelineColorBlendAttachmentState attachment_blend_state{};
	attachment_blend_state.blendEnable = VK_TRUE;
	if (key.blending == gpu::blending::alpha) {
		attachment_blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		attachment_blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		attachment_blend_state.colorBlendOp = VK_BLEND_OP_ADD;
		attachment_blend_state.colorWriteMask = 0b111; // Do not write alpha
	} else if (key.blending == gpu::blending::add) {
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
	VkDynamicState dynamic_states[3]{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamic_state{};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = 3;
	dynamic_state.pDynamicStates = dynamic_states;

	VkGraphicsPipelineCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = static_cast<uint32_t>(stage_infos.size());
	pipeline_info.pStages = stage_infos.data();
	pipeline_info.pVertexInputState = &input_state;
	pipeline_info.pInputAssemblyState = &assembly_state;
	pipeline_info.pTessellationState = &tessellation_state;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pRasterizationState = &rasterization_state;
	pipeline_info.pMultisampleState = &multisample_state;
	pipeline_info.pDepthStencilState = &depth_stencil_state;
	pipeline_info.pColorBlendState = &blend_state;
	pipeline_info.pDynamicState = &dynamic_state;
	pipeline_info.layout = pipeline.pipeline_layout;
	pipeline_info.renderPass = key.render_pass;

	VOLT_VK12_CHECK(vkCreateGraphicsPipelines(device.vk_device, device.pipeline_cache,
			1, &pipeline_info, nullptr, &pipeline.vk_pipeline), "Failed to create graphics pipeline.")

	return mutable_cache.rasterization_pipelines.emplace(std::move(key), std::move(pipeline)).first->second;
}

VkFramebuffer jit::framebuffer(framebuffer_key &&key) {
	// First search in immutable cache
	auto it = immutable_cache.framebuffers.find(key);
	if (it != immutable_cache.framebuffers.end())
		return it->second;

	// Fall back to mutable cache with locking
	std::unique_lock lock(mutable_cache_mutex);

	it = mutable_cache.framebuffers.find(key);
	if (it != mutable_cache.framebuffers.end())
		return it->second;

	std::vector<VkImageView> attachments(key.color_attachments.size() + (key.depth_stencil_attachment != nullptr));
	for (int i = 0; i < key.color_attachments.size(); i++)
		attachments[i] = static_cast<vk12::texture *>(key.color_attachments[i])->image_view;
	if (key.depth_stencil_attachment)
		attachments.back() = static_cast<vk12::texture *>(key.depth_stencil_attachment)->image_view;

	math::uvec3 size;
	if (!key.color_attachments.empty())
		size = key.color_attachments[0]->size();
	else
		size = key.depth_stencil_attachment->size();

	VkFramebufferCreateInfo framebuffer_info{};
	framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_info.renderPass = key.render_pass;
	framebuffer_info.attachmentCount = attachments.size();
	framebuffer_info.pAttachments = attachments.data();
	framebuffer_info.width = size.x;
	framebuffer_info.height = size.y;
	framebuffer_info.layers = size.z;

	VkFramebuffer framebuffer;
	VOLT_VK12_CHECK(vkCreateFramebuffer(device.vk_device, &framebuffer_info,
			nullptr, &framebuffer), "Failed to create framebuffer.")

	return mutable_cache.framebuffers.emplace(std::move(key), framebuffer).first->second;
}

void jit::optimize() {
	// Move all items from mutable cache to immutable cache

	for (auto &it : mutable_cache.compute_pipelines)
		immutable_cache.compute_pipelines.emplace(std::move(it));

	for (auto &it : mutable_cache.render_passes)
		immutable_cache.render_passes.emplace(std::move(it));

	for (auto &it : mutable_cache.rasterization_pipelines)
		immutable_cache.rasterization_pipelines.emplace(std::move(it));

	for (auto &it : mutable_cache.framebuffers)
		immutable_cache.framebuffers.emplace(std::move(it));

	mutable_cache.compute_pipelines.clear();
	mutable_cache.render_passes.clear();
	mutable_cache.rasterization_pipelines.clear();
	mutable_cache.framebuffers.clear();
}

void jit::clear_framebuffers() {
	// Flushes mutable cache
	optimize();

	for (auto &item : immutable_cache.framebuffers)
		vkDestroyFramebuffer(device.vk_device, item.second, nullptr);
	immutable_cache.framebuffers.clear();
}

void jit::clear() {
	// Flushes mutable cache
	clear_framebuffers();

	for (auto &item : immutable_cache.rasterization_pipelines) {
		vkDestroyPipeline(device.vk_device, item.second.vk_pipeline, nullptr);
		vkDestroyPipelineLayout(device.vk_device, item.second.pipeline_layout, nullptr);
		for (auto &layout : item.second.set_layouts)
			vkDestroyDescriptorSetLayout(device.vk_device, layout, nullptr);
	}
	immutable_cache.rasterization_pipelines.clear();

	for (auto &item : immutable_cache.render_passes)
		vkDestroyRenderPass(device.vk_device, item.second, nullptr);
	immutable_cache.render_passes.clear();

	for (auto &item : immutable_cache.compute_pipelines) {
		vkDestroyPipeline(device.vk_device, item.second.vk_pipeline, nullptr);
		vkDestroyPipelineLayout(device.vk_device, item.second.pipeline_layout, nullptr);
		for (auto &layout : item.second.set_layouts)
			vkDestroyDescriptorSetLayout(device.vk_device, layout, nullptr);
	}
	immutable_cache.compute_pipelines.clear();
}


template<typename Key>
pipeline jit::create_pipeline(Key &key, const std::vector<vk12::shader *> &shaders) {
	pipeline pipeline;

	// Accumulate descriptor set layout data from all shaders
	struct pass_set_layout_info {
		VkDescriptorSetLayoutCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};
	std::unordered_map<uint32_t, pass_set_layout_info> pass_set_layout_infos;
	
	uint32_t last_set = 0;
	for (vk12::shader *shader : shaders) {
		for (vk12::shader::set_layout_info info : shader->set_layout_infos) {
			last_set = math::max(last_set, info.set);
			pass_set_layout_info &set_layout_create_info = pass_set_layout_infos[info.set];

			set_layout_create_info.bindings.insert(
				set_layout_create_info.bindings.end(),
				info.bindings.begin(),
				info.bindings.end()
			);

			set_layout_create_info.info.bindingCount = static_cast<uint32_t>(set_layout_create_info.bindings.size());
			set_layout_create_info.info.pBindings = set_layout_create_info.bindings.data();
		}

		// Accumulate binding points
		pipeline.binding_points.insert(shader->binding_points.begin(), shader->binding_points.end());
	}

	// Create descriptor set layouts
	pipeline.set_layouts.resize(last_set + 1);
	for (uint32_t i = 0; i < pipeline.set_layouts.size(); i++) {
		vkCreateDescriptorSetLayout(device.vk_device,
				&pass_set_layout_infos[i].info, nullptr, &pipeline.set_layouts[i]);
	}

	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(pipeline.set_layouts.size());
    pipeline_layout_info.pSetLayouts = pipeline.set_layouts.data();

	VOLT_VK12_CHECK(vkCreatePipelineLayout(device.vk_device, &pipeline_layout_info,
			nullptr, &pipeline.pipeline_layout), "Failed to create pipeline layout.")

	return pipeline;
}

}
