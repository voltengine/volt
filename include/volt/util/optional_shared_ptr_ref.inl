namespace volt::util {

template<typename T>
optional_shared_ptr_ref<T>::optional_shared_ptr_ref(const std::shared_ptr<T> &ptr) {
	this->ptr = ptr.get();
}

template<typename T>
template<typename U>
optional_shared_ptr_ref<T>::operator U *() const {
	return static_cast<U *>(ptr);
}

template<typename T>
optional_shared_ptr_ref<T>::operator bool() const {
	return ptr;
}

template<typename T>
T &optional_shared_ptr_ref<T>::operator*() const {
	return *ptr;
}

template<typename T>
T *optional_shared_ptr_ref<T>::operator->() const {
	return ptr;
}

}