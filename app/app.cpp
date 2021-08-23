#include <pch.hpp>

#include <volt/modules.hpp>

class type : public volt::reflection::serializable {
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
	volt::modules::reload(VOLT_DEVELOPMENT_MODULE);}catch(...){}

	volt::reflection::type::define<type>("type")
			.constructor<int>()
			.field("number", &::type::number);
	auto type = volt::reflection::type::get("type");
	std::cout << type.get_name() << std::endl;
	{
		volt::reflection::shared_instance<> instance = type.instantiate(69);
		instance.get<int>("number") = 2;
		std::cout << instance.get<int>("number") << std::endl;
		std::cout << type.get_instances().size() << std::endl;
	}
	std::cout << type.get_instances().size() << std::endl;
	std::cout << "Done.\n";

	return 0;
}