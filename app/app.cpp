#include <pch.hpp>

#include <variant>

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

		std::vector<uint8_t> vert = util::read_binary_file("D:/Library/Source/voltengine/voltcraft/shaders/triangle.vert.spv");
		std::vector<uint8_t> frag = util::read_binary_file("D:/Library/Source/voltengine/voltcraft/shaders/triangle.frag.spv");
		// std::vector<uint8_t> comp = util::read_binary_file("D:/Library/Source/voltengine/voltcraft/shaders/compute.comp.spv");

		auto vertex_shader = device->create_shader(vert);
		auto pixel_shader = device->create_shader(frag);
		// auto compute_shader = device->create_shader(comp);

		util::timer timer;
		timer.start();

		// static resources can be accessed without explicit entry in pass_info

		// GPU Internal
		// static_texture - read sampled texture, read storage texture - no sync
		// dynamic_texture - read-write sampled texture (attachment), read-write storage_texture

		// Host interface
		// upload_texture - sampled texture, read storage texture - streamed from CPU (for videos) - no sync
		// download_texture - write storage_texture - readback to CPU (for sophisticated compute techniques)

		// GPU Internal
		// static_buffer - read vertex buffer, read index buffer, read constant buffer, read storage buffer - no sync
		// dynamic_buffer - read-write storage_buffer
		
		// Host interface
		// upload_buffer - vertex buffer, index buffer, constant buffer, read storage buffer - streamed from CPU (for dynamic data) - no sync
		// download_buffer - write storage_buffer - readback to CPU (for compute physics)

		while (!window->is_closing()) {
			glfwPollEvents();

			device->graph([&](gpu::graph &graph) {
				swapchain->frame([&](gpu::frame &frame) {
					math::uvec2 frame_size = window->get_frame_size();

					gpu::pass_info pass_info{
						.color_attachments = {
							{ frame.texture, gpu::attachment_initializer::clear, math::fvec4(math::sin(timer.elapsed()) * 0.5F + 0.5F) }
						}
					};

					graph.pass(pass_info, [&](gpu::pass &pass) {
						gpu::draw_info draw_info{
							.vertex_shader = vertex_shader,
							.pixel_shader = pixel_shader,
							.viewport = {
								{ 0, static_cast<float>(frame_size.x) },
								{ 0, static_cast<float>(frame_size.y) }
							},
							.culling = false,
							.draw_count = 3
						};
						
						pass.draw(draw_info);
					});
					
					// gpu::dispatch_info dispatch_info{
					// 	.compute_shader = compute_shader.get(),
					// 	.group_count = { 1, 1, 1 }
					// };
					// graph.dispatch(dispatch_info);
				});
			});
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