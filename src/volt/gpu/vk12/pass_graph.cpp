#include <volt/gpu/vk12/pass_graph.hpp>

#include <volt/gpu/vk12/device.hpp>

namespace volt::gpu::vk12 {

pass_graph::pass_graph(std::shared_ptr<gpu::device> &&device)
		: gpu::pass_graph(std::move(device)) {
	vk_device = static_cast<vk12::device *>(device.get())->vk_device;
}

pass_graph::~pass_graph() {
	
}

pass_graph::execute() {
	gpu::pass_graph::execute();

	// Vulkan code
}

pass_graph::build() {
	for (gpu::pass &pass : passes) {
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkAttachmentReference> attachment_refs;

		for (auto &attachment : pass.)
		VkAttachmentDescription color_attachment{};
		color_attachment.format = swap_format.format;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference attachment_ref{};
		attachment_ref.attachment = 0;
		attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription attachments[] = { color_attachment };

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = attchment_refs.data();

		VkRenderPassCreateInfo render_pass_create_info{};
		render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_create_info.attachmentCount = 1;
		render_pass_create_info.pAttachments = attachments.data();
		render_pass_create_info.subpassCount = 1;
		render_pass_create_info.pSubpasses = &subpass;
		// TODO: Repalce removed subpass dependency with fence

		VOLT_VK12_CHECK(
			vkCreateRenderPass(vk_device, &render_pass_create_info, nullptr, &render_pass) != VK_SUCCESS)
			throw std::runtime_error("Failed to create render pass.");
	}
}

}