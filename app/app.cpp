#include <iostream>

#include <volt/volt.hpp>

int main() {
#if VOLT_EDITOR
	std::cout << "Hello from Volt App! (Editor Enabled)" << std::endl;
#else
	std::cout << "Hello from Volt App! (Editor Disabled)" << std::endl;
#endif
	volt::hello();
	return 0;
}