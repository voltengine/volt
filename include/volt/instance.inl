namespace volt {

// instance_owner

instance_owner::instance_owner(serializable *ptr,
		std::list<instance_owner *> &parent_list)
		: ptr(ptr), parent_list(parent_list) {
	it = parent_list.emplace(parent_list.end(), this);
}

instance_owner::~instance_owner() noexcept {
	parent_list.erase(it);
	delete ptr;
}

// _ref_manager

_ref_manager::_ref_manager(instance_owner *owner) noexcept
		: owner(owner) {}

void _ref_manager::construct_shared() noexcept {
	shared_count++;
}

void _ref_manager::destruct_shared() {
	if (--shared_count == 0) {
		delete owner;
		if (weak_count == 0)
			delete this;
	}
}

void _ref_manager::construct_weak() noexcept {
	weak_count++;
}

void _ref_manager::destruct_weak() noexcept {
	if (--weak_count == 0 && shared_count == 0)
		delete this;
}

bool _ref_manager::owner_valid() const noexcept {
	return shared_count != 0;
}

// shared_instance

template<typename T>
shared_instance<T>::shared_instance(instance_owner *owner)
		: shared_instance(new _ref_manager(owner)) {}

template<typename T>
template<typename U>
shared_instance<T>::shared_instance(const shared_instance<U> &other) noexcept
		: shared_instance(other.manager) {}

template<typename T>
template<typename U>
shared_instance<T>::shared_instance(shared_instance<U> &&other) noexcept
		: manager(other.manager) {
	other.manager = nullptr;
}

template<typename T>
shared_instance<T>::~shared_instance() noexcept {
	if (manager)
		manager->destruct_shared();
}

template<typename T>
T *shared_instance<T>::operator->() const {
	return static_cast<T *>(manager->owner->ptr);
}

template<typename T>
shared_instance<T>::shared_instance(_ref_manager *manager) noexcept
		: manager(manager) {
	manager->construct_shared();
}

// weak_instance

template<typename T>
template<typename U>
weak_instance<T>::weak_instance(const weak_instance<U> &other) noexcept
		: weak_instance(other.manager) {}

template<typename T>
template<typename U>
weak_instance<T>::weak_instance(weak_instance<U> &&other) noexcept
		: manager(other.manager) {
	other.manager = nullptr;
}

template<typename T>
template<typename U>
weak_instance<T>::weak_instance(const shared_instance<U> &other) noexcept
		: weak_instance(other.manager) {}

template<typename T>
weak_instance<T>::~weak_instance() noexcept {
	manager->destruct_weak();
}

template<typename T>
template<typename U>
weak_instance<T>::operator shared_instance<U>() const noexcept {
	assert(valid());
	return shared_instance(manager);
}

template<typename T>
T *weak_instance<T>::operator->() const {
	assert(valid());
	return static_cast<T *>(manager->owner->ptr);
}

template<typename T>
bool weak_instance<T>::valid() const noexcept {
	return manager->owner_valid();
}

template<typename T>
weak_instance<T>::weak_instance(_ref_manager *manager) noexcept
		: manager(manager) {
	manager->construct_weak();
}

}
