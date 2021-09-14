#include <pch.hpp>

#include <volt/volt.hpp>

namespace fs = std::filesystem;

int main() {
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
	std::cout << "\nLoaded modules.\n";
	for (auto &name : volt::modules::get_names())
		std::cout << name << '\n';

	while (true) {
		std::cout << "\n[P] - Print State; [U] - Update Systems; "
				"[R] - Reload \"" VOLT_DEVELOPMENT_MODULE "\"; [Q] - Quit\n";
		char c;
		do {
			c = getch();
		} while (c != 'p' && c != 'u' && c != 'r' && c != 'q');

		switch (c) {
		case 'p':
		{
			auto &component_names = volt::ecs::get_component_names();
			auto &system_names = volt::ecs::get_system_names();

			for (auto &module_name : volt::modules::get_names()) {
				std::cout << '\n' << module_name;

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
				std::cout << '\n';
			}
			continue;
		}
		case 'u':
			try {
				volt::ecs::update_systems(0);
			} catch (std::exception &e) {
				std::cout << "Update failed:\n" << e.what() << '\n';
			}
			continue;
		case 'r':
			try {
				std::cout << "\nReloading " VOLT_DEVELOPMENT_MODULE "...\n";
				volt::modules::_reload_development_module([]() {
					auto src = fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "cmake" / "bin"
							/ (std::string(VOLT_DEVELOPMENT_MODULE) + volt::modules::module_extension);
					auto dst = fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "bin"
							/ (std::string(VOLT_DEVELOPMENT_MODULE) + volt::modules::module_extension);
					fs::remove(dst);
					fs::copy(src, dst);
				});
				std::cout << "Done reloading " VOLT_DEVELOPMENT_MODULE ".\n";
			} catch (std::exception &e) {
				std::cout << "Reload failed:\n" << e.what() << '\n';
			}
			continue;
		default:
			break;
		}
		
		break;
	}

	volt::modules::unload();
	std::cout << "\nUnloaded modules.\n";

	return 0;
}