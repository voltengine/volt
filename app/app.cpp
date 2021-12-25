#include <pch.hpp>

namespace fs = std::filesystem;

using namespace volt;

void error_callback(int code, const char *description) {
	VOLT_LOG_ERROR(std::string("[GLFW] ") + description)
}

int main() {
	try {
		glfwSetErrorCallback(error_callback);
		VOLT_ASSERT(glfwInit(), "Failed to initialize GLFW.")
		volt::config::_internal::init();

#if VOLT_DEVELOPMENT
#ifdef NDEBUG
		std::cout << "Hello from Volt App! (Development Enabled At: " VOLT_DEVELOPMENT_PATH ")" << '\n';
#else
		std::cout << "Hello from Volt App! (Development with Debugging Support Enabled At: " VOLT_DEVELOPMENT_PATH ")" << '\n';
#endif
#else
		std::cout << "Hello from Volt App! (Development Disabled)" << '\n';
#endif
		volt::modules::load();

		std::cout << "\nLoaded modules:\n";
		for (auto &name : volt::modules::get_names())
			std::cout << name << '\n';

		std::shared_ptr<os::window> window = std::make_shared<os::window>("Test Window", math::uvec2(1280, 720));
		window->set_visible(true);

		std::shared_ptr<gpu::instance> instance = gpu::create_instance(gpu::api::vk12);
		std::vector<std::shared_ptr<gpu::adapter>> adapters = instance->enumerate_adapters();
		std::cout << "Found " << adapters.size() << " adapter(s):" << std::endl;
		for (auto &adapter : adapters)
			std::cout << adapter->name() << std::endl;
		std::shared_ptr<gpu::adapter> &adapter = adapters[0];
		std::shared_ptr<gpu::device> device = adapter->create_device();
		std::shared_ptr<gpu::swapchain> swapchain = device->create_swapchain(window);
		std::shared_ptr<gpu::copy_routine> streaming_routine = device->create_copy_routine();

		// create_swapchain(window) emits an error if another swapchain is already created for that window.
		// Dropping all references to swapchain will destroy it and in turn allow another call to create_surface(window).

		auto buffer = device->create_buffer(
			gpu::memory_type::internal,
			gpu::buffer_feature::vertex | gpu::buffer_feature::copy_dst,
			1024
		);

		auto texture = device->create_2d_texture(
			gpu::memory_type::internal,
			gpu::texture_feature::sampled | gpu::texture_feature::copy_dst,
			gpu::texture_format::bc1_srgb,
			1, {2048, 2048}
		);

		while (!window->is_closing()) {
			glfwPollEvents();

			// next_frame() might return immediately if no work is available
			swapchain->next_frame([](gpu::frame frame) {
				gpu::universal_routine_context routine_context = frame.routine_context;

				gpu::pass_info info{
					.color_attachments = {
						{
							.texture = frame.texture,
							.initializer = gpu::attachment_initializer::clear
						}
					},
					.depth_stencil_attachment = {
						.texture = nullptr,
						.initializer = gpu::attachment_initializer::clear
					}
				};

				gpu::descriptor_info descriptor_info{
					.constant_buffers = {
						{ "u_ConstantBuffer", nullptr }
					},
					.sampled_textures = {
						{ "u_Texture", nullptr, nullptr }
					},
					.storage_buffers = {
						{ "u_StorageBuffer", nullptr, true }
					},
					.storage_textures = {
						{ "u_StorageTexture", nullptr, true }
					}
				};

				routine_context.async_pass(info, [](gpu::async_pass_context pass_context) {				
					world.each<model>([](uint32_t thread_index, model &model){
						gpu::draw_info draw_info{
							.descriptor_info = descriptor_info

							// Pipeline state used to query pipeline
							.vertex_shader = nullptr,
							.hull_shader = nullptr,
							.domain_shader = nullptr,
							.geometry_shader = nullptr,
							.pixel_shader = nullptr,
							.instance_inputs = {},
							.primitive_mode = gpu::topology::triangles,
							.polygon_mode = gpu::topology::triangles,
							.culling = false,
							.line_width = 1,
							.depth_test = false,
							.depth_write = false,
							.stencil_test = false,
							.stencil_write = false,
							.blending = gpu::blending::alpha,

							// Actual dynamic draw info
							.vertex_buffer = nullptr,
							.index_buffer = nullptr,
							.instance_buffer = nullptr,
							.viewport = { { 0, 1280 }, { 0, 720 }, { 0, 1 } },
							.index_count = 0,
							.instance_count = 0
						};

						pass_context.draw(thread_index, draw_info);
					}, pass_context.thread_pool);
				});

				routine_context.pass(info, [](gpu::pass_context pass_context) {				
					gpu::draw_info draw_info{
						.descriptor_info = descriptor_info

						// Pipeline state used to query pipeline
						.vertex_shader = nullptr,
						.hull_shader = nullptr,
						.domain_shader = nullptr,
						.geometry_shader = nullptr,
						.pixel_shader = nullptr,
						.instance_inputs = {},
						.primitive_mode = gpu::topology::triangles,
						.polygon_mode = gpu::topology::triangles,
						.culling = false,
						.line_width = 1,
						.depth_test = false,
						.depth_write = false,
						.stencil_test = false,
						.stencil_write = false,
						.blending = gpu::blending::alpha,

						// Actual dynamic draw info
						.vertex_buffer = nullptr,
						.index_buffer = nullptr,
						.instance_buffer = nullptr,
						.viewport = { { 0, 1280 }, { 0, 720 }, { 0, 1 } },
						.index_count = 0,
						.instance_count = 0
					};

					pass_context.draw(draw_info);
				});

				gpu::dispatch_info dispatch_info{
					.descriptor_info = descriptor_info
					.compute_shader = nullptr,
					.group_count = { 1, 1, 1 }
				};
				routine_context.dispatch(dispatch_info);
			});

				// routine_context.dispatch({ 128, 128, 4 });
				
				// routine_context.copy_texture(src, dst);

				// ...
			});

			// Then we can do some texture streaming

			// Swapchain does this check internally for each frame it tries to acquire
			if (!streaming_routine->finished())
				continue;

			streaming_routine->execute([](gpu::copy_routine_context &context) {
				// context.copy_texture(src, dst);
				// ...
			});

			streaming_routine->wait();
		}



// 		while (true) {
// 			std::cout << "\n[P] - Print State; [U] - Update Systems; ";
// #ifdef VOLT_DEVELOPMENT
// 			std::cout << "[R] - Reload \"" VOLT_DEVELOPMENT_MODULE "\"; ";
// #endif
// 			std::cout << "[Q] - Quit\n";
// 			char c;
// 			do {
// 				c = getch();
// 			} while (c != 'p' && c != 'u' && c != 'r' && c != 'q');

// 			switch (c) {
// 			case 'p':
// 			{
// 				auto &component_names = volt::ecs::get_component_names();
// 				auto &system_names = volt::ecs::get_system_names();
// 				auto &asset_types = volt::assets::get_types();

// 				for (auto &module_name : volt::modules::get_names()) {
// 					std::cout << '\n' << module_name;

// 					if (component_names.contains(module_name)) {
// 						std::cout << "\n(components) ";
// 						for (auto &component_name : component_names.at(module_name)) {
// 							std::cout << component_name;
// 							if (component_name != *std::prev(component_names.at(module_name).end()))
// 								std::cout << ", ";
// 						}
// 					}
// 					if (system_names.contains(module_name)) {
// 						std::cout << "\n(systems) ";

// 						for (auto &system_name : system_names.at(module_name)) {
// 							std::cout << system_name;
// 							if (system_name != *std::prev(system_names.at(module_name).end()))
// 								std::cout << ", ";
// 						}
// 					}
// 					if (asset_types.contains(module_name)) {
// 						std::cout << "\n(assets) ";

// 						for (auto &asset_type : asset_types.at(module_name)) {
// 							std::cout << asset_type;
// 							if (asset_type != *std::prev(asset_types.at(module_name).end()))
// 								std::cout << ", ";
// 						}
// 					}
// 					std::cout << '\n';
// 				}
// 				continue;
// 			}
// 			case 'u':
// 				try {
// 					volt::ecs::update_systems(0);
// 				} catch (volt::error &e) {
// 					volt::log::error(e.what(), e.where(), e.at());
// 				}
// 				continue;
// #ifdef VOLT_DEVELOPMENT
// 			case 'r':
// 				try {
// 					std::cout << "\nReloading " VOLT_DEVELOPMENT_MODULE "...\n";
// 					volt::modules::reload_development_module();
// 					std::cout << "Done reloading " VOLT_DEVELOPMENT_MODULE ".\n";
// 				} catch (volt::error &e) {
// 					volt::log::error(e.what(), e.where(), e.at());
// 				}
// 				continue;
// #endif
// 			default:
// 				break;
// 			}

// 			break;
// 		}

		volt::modules::unload();
		std::cout << "\nUnloaded modules.\n";
	} catch (volt::error &e) {
		volt::log::error(e.what(), e.where(), e.at());
		// TODO: display OS alert
	} catch (std::exception &e) {
		VOLT_LOG_ERROR(e.what())
		// TODO: display OS alert
	}

	// C libraries do not throw errors
	glfwTerminate();

	return 0;
}