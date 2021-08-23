#pragma once

#include "../modules.hpp"

#include <concepts>
#include <cstdint>
#include <vector>

#include "../serializable.hpp"

namespace volt::ecs {

class base_component_storage {
public:
	virtual ~base_component_storage() = default;

	VOLT_API size_t get_cid(size_t eid) const;

	VOLT_API size_t get_eid(size_t cid) const;

	virtual void remove(size_t cid) = 0;

	virtual const serializable *get_ptr(size_t cid) const = 0;

protected:
	std::vector<size_t> eid_to_cid, cid_to_eid;
};

template<component_type T>
class component_storage : public base_component_storage {
public:
	template<typename... Args>
	T &add(size_t eid, Args &&...args);

	T &get(size_t cid);

	const std::vector<T> &get_components() const;

	void remove(size_t cid) override;

	const volt::Component *get_ptr(size_t cid) const override;

private:
	// TODO: Implement custom
	// bucket storage like aka deque
	std::vector<T> components;
};

}

#include "component_storage.inl"
