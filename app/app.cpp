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


		using namespace video;

		auto window = std::make_shared<os::window>("Test Window", math::uvec2(1280, 720));
		window->set_visible(true);

		auto instance = video::create_instance(video::api::vk12);
		auto adapter = instance->list_adapters()[0];
		auto device = adapter->create_device();
		auto surface = device->create_surface(window);
		// create_surface(window) emits an error if another surface is already created for that window.
		// Dropping all references to surface will destroy it and in turn allow another call to create_surface(window).

		auto buffer = device->create_buffer(
			resource::type::internal,
			queue::type::graphics | queue::type::copy,
			buffer::feature::vertex | buffer::feature::destination,
			1024
		);
		
		auto texture = device->create_texture(
			resource::type::internal,
			0,
			texture::feature::sampler | texture::feature::destination,
			{ 2048, 2048 }, 1, 1, texture::format::bc1_srgb
		);
		// auto queue = device->get_graphics_queue();
		// auto buffers = window->get_buffers();
		
		while (!window->is_closing()) { glfwPollEvents(); }


		

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
	} catch (std::exception &e) {
		VOLT_LOG_ERROR(e.what())
		// TODO: display OS alert
	} catch (volt::error &e) {
		volt::log::error(e.what(), e.where(), e.at());
		// TODO: display OS alert
	}

	// C libraries do not throw errors
	glfwTerminate();

	return 0;
}