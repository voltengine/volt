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

		volt::modules::_internal::load();

		std::cout << "\nLoaded modules:\n";
		for (auto &name : volt::modules::names())
			std::cout << name << '\n';

		while (true) {
			std::cout << "\n[P] - Print State; [U] - Update Systems; ";
#ifdef VOLT_DEVELOPMENT
			std::cout << "[R] - Reload \"" VOLT_DEVELOPMENT_MODULE "\"; ";
#endif
			std::cout << "[Q] - Quit\n";
			char c;
			do {
				std::cin.get(c);
			} while (c != 'p' && c != 'u' && c != 'r' && c != 'q');

			switch (c) {
			case 'p':
			{
#ifdef VOLT_DEVELOPMENT
				auto &component_names = volt::ecs::_internal::module_name_to_component_names;
				auto &system_names = volt::ecs::_internal::module_name_to_system_names;
#endif
				auto &asset_types = volt::assets::get_types();

				for (auto &module_name : volt::modules::names()) {
					std::cout << '\n' << module_name;
#ifdef VOLT_DEVELOPMENT
					if (component_names.contains(module_name)) {
						std::cout << "\n(components) ";
						for (auto &component_name : component_names.at(module_name)) {
							std::cout << component_name;
							if (component_name != *std::prev(component_names.at(module_name).end()))
								std::cout << ", ";
						}
					}
					if (system_names.contains(module_name)) {
						std::cout << "\n(systems) ";

						for (auto &system_name : system_names.at(module_name)) {
							std::cout << system_name;
							if (system_name != *std::prev(system_names.at(module_name).end()))
								std::cout << ", ";
						}
					}
#endif
					if (asset_types.contains(module_name)) {
						std::cout << "\n(assets) ";

						for (auto &asset_type : asset_types.at(module_name)) {
							std::cout << asset_type;
							if (asset_type != *std::prev(asset_types.at(module_name).end()))
								std::cout << ", ";
						}
					}
					std::cout << '\n';
				}
				continue;
			}
			case 'u':
				try {
					volt::ecs::system::_update_all(0.0F);
				} catch (volt::error &e) {
					volt::log::error(e.what(), e.where(), e.at());
				}
				continue;
#ifdef VOLT_DEVELOPMENT
			case 'r':
				try {
					volt::modules::reload_development_module();
				} catch (volt::error &e) {
					volt::log::error(e.what(), e.where(), e.at());
				}
				continue;
#endif
			default:
				break;
			}

			break;
		}

		volt::modules::_internal::unload();
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