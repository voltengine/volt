namespace volt::util {

template<typename T>
smart_ptr_view<T>::smart_ptr_view(const std::unique_ptr<T> &ptr) {
	this->ptr = ptr.get();
}

template<typename T>
smart_ptr_view<T>::smart_ptr_view(const std::shared_ptr<T> &ptr) {
	this->ptr = ptr.get();
}

template<typename T>
smart_ptr_view<T>::smart_ptr_view(const std::weak_ptr<T> &ptr) {
	this->ptr = ptr.lock().get();
}

template<typename T>
template<typename U>
smart_ptr_view<T>::operator U *() const {
	return static_cast<U *>(ptr);
}

template<typename T>
smart_ptr_view<T>::operator bool() const {
	return ptr;
}

template<typename T>
T &smart_ptr_view<T>::operator*() const {
	return *ptr;
}

template<typename T>
T *smart_ptr_view<T>::operator->() const {
	return ptr;
}

}