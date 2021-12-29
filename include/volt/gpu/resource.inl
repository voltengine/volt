namespace volt::gpu {

template<typename T>
void resource::map(const std::function<void(T &)> &callback, access_modes access, size_t offset) {
	map([&](void *data) {
		callback(*reinterpret_cast<T *>(data));
	}, access, sizeof(T), offset);
}

}
