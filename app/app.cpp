#include <pch.hpp>

#include <volt/volt.hpp>

struct type : public volt::modules::serializable {
	int number = 4;
};

int main() {
#if VOLT_DEVELOPMENT
	std::cout << "Hello from Volt App! (Development Enabled At: " VOLT_DEVELOPMENT_PATH ")" << std::endl;
#else
	std::cout << "Hello from Volt App! (Development Disabled)" << std::endl;
#endif

	std::shared_ptr<int> x;

	volt::modules::load();
	std::cout << "Loaded modules.\n";

	volt::ecs::entity_manager manager;
	auto entity = manager.create();
	auto json = nlohmann::json::object();
	json["number"] = 7;
	entity.add("voltpy::component", json);
	std::cout << entity.get("voltpy::component") << std::endl;

	try {
		std::cout << "Reloading development module...\n";
		volt::modules::reload({ VOLT_DEVELOPMENT_MODULE }, [](){
			std::cout << "Development module has been unloaded.\n";
		});
		std::cout << "Done reloading development module.\n";
	} catch (std::exception &e) {
		std::cout << "Reload failed:\n" << e.what() << '\n';
	}

	std::cout << entity.get("voltpy::component") << std::endl;

	entity.destroy();
	
	volt::modules::unload();
	std::cout << "Unloaded modules.\n";
	return 0;
}