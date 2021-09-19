#include <volt/macros.hpp>

#include <volt/ecs/ecs.hpp>
#include <volt/modules.hpp>

extern "C" {

VOLT_API void volt_module_main() {
	volt::modules::register_unload_callback(
			volt::ecs::_module_unload_callback);
};

}
