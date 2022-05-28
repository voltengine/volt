#include <volt/volt.hpp>

class text {
public:
	std::string str;
	
	text(std::string str) : str(str) {}
};

class text2 {
public:
	std::string str;
	
	text2(std::string str) : str(str) {}
};

int main() {
	volt::ecs::_internal::register_component<text>("text");
	volt::ecs::_internal::register_component<text2>("text2");

	volt::ecs::world world;
	volt::ecs::entity entity = world.create();
	entity.add<text>("aaa");
	entity.add<text2>("aaa2");

	volt::ecs::entity entity2 = world.create();
	entity2.add<text>("bbb");

	world.each<text>([](auto entity, auto &text) {
		std::cout << entity << std::endl;
		std::cout << text.str << std::endl;
	});
	return 0;
}
