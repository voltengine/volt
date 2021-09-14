namespace volt::ecs {

template<typename T>
void system::unique_lock() {
	_unique_mask.set(_component_type_index_to_index[typeid(T)]);
}

template<typename T>
void system::shared_lock() {
	_shared_mask.set(_component_type_index_to_index[typeid(T)]);
}

}
