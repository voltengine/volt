#include <pch.hpp>

#include <volt/modules.hpp>

class type : public volt::serializable {
public:
	int number = 7;

	type(int number) : number(number) {}

	int get_number() const {
		return number;
	}
};

int main() {
#if VOLT_DEVELOPMENT
	std::cout << "Hello from Volt App! (Development Enabled At: " VOLT_DEVELOPMENT_PATH ")" << std::endl;
#else
	std::cout << "Hello from Volt App! (Development Disabled)" << std::endl;
#endif

	volt::modules::load_all();
	std::cout << "Loaded modules.\n";

	try {
		volt::modules::reload(VOLT_DEVELOPMENT_MODULE);
		std::cout << "Reloaded modules.\n";
	} catch (std::exception &e) {
		std::cout << "Reload failed:\n" << e.what() << '\n';
	}
	
	volt::modules::unload_all();
	std::cout << "Unloaded modules.\n";
	return 0;
}