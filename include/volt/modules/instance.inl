namespace volt::modules {

// _instance_owner

_instance_owner::_instance_owner(serializable *ptr,
		std::list<_instance_owner *> &tracking_list)
		: ptr(ptr), tracking_list(tracking_list) {
	it = tracking_list.emplace(tracking_list.end(), this);
}

_instance_owner::~_instance_owner() noexcept {
	tracking_list.erase(it);
	delete ptr;
}

// _ref_manager

_ref_manager::_ref_manager(_instance_owner *owner) noexcept
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
shared_instance<T>::shared_instance()
		: manager(nullptr) {}

template<typename T>
shared_instance<T>::shared_instance(_instance_owner *owner)
		: manager(new _ref_manager(owner)) {
	manager->construct_shared();
}

template<typename T>
template<typename U>
shared_instance<T>::shared_instance(const shared_instance<U> &other) noexcept
		: manager(other.manager) {
	if (manager)
		manager->construct_shared();
}

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
template<typename U>
shared_instance<T> &shared_instance<T>::operator=(const shared_instance<U> &other) noexcept {
	if (manager)
		manager->destruct_shared();
	if (manager = other.manager)
		manager->construct_shared();
	return *this;
}

template<typename T>
template<typename U>
shared_instance<T> &shared_instance<T>::operator=(shared_instance<U> &&other) noexcept {
	std::swap(manager, other.manager);
	return *this;
}

template<typename T>
shared_instance<T>::operator bool() const noexcept {
	return manager && manager->owner->ptr;
}

template<typename T>
T *shared_instance<T>::operator->() const {
	return get();
}

template<typename T>
T *shared_instance<T>::get() const {
	return static_cast<T *>(manager->owner->ptr);
}

template<typename T>
shared_instance<T>::shared_instance(_ref_manager *manager) {
	if (manager)
		manager->construct_shared();
}

// weak_instance

template<typename T>
template<typename U>
weak_instance<T>::weak_instance(const weak_instance<U> &other) noexcept
		: manager(other.manager) {
	if (manager)
		manager->construct_weak();	
}

template<typename T>
template<typename U>
weak_instance<T>::weak_instance(weak_instance<U> &&other) noexcept
		: manager(other.manager) {
	other.manager = nullptr;
}

template<typename T>
template<typename U>
weak_instance<T>::weak_instance(const shared_instance<U> &other) noexcept
		: manager(other.manager) {
	if (manager)
		manager->construct_weak();	
}

template<typename T>
weak_instance<T>::~weak_instance() noexcept {
	if (manager)
		manager->destruct_weak();
}

template<typename T>
template<typename U>
weak_instance<T> &weak_instance<T>::operator=(const weak_instance<U> &other) noexcept {
	if (manager)
		manager->destruct_weak();
	if (manager = other.manager)
		manager->construct_weak();
	return *this;
}

template<typename T>
template<typename U>
weak_instance<T> &weak_instance<T>::operator=(weak_instance<U> &&other) noexcept {
	std::swap(manager, other.manager);
	return *this;
}

template<typename T>
template<typename U>
weak_instance<T>::operator shared_instance<U>() const noexcept {
	return shared_instance(manager);
}

template<typename T>
bool weak_instance<T>::valid() const noexcept {
	return manager && manager->owner_valid();
}

}
