#include "../log.hpp"
#include "ecs.hpp"

namespace volt::ecs {

template<typename... Systems>
void system::after() {
    after_impl<Systems...>();
}

template<typename... Components>
void system::unique_lock() {
    lock_impl<true, Components...>(*this);
}

template<typename... Components>
void system::shared_lock() {
    lock_impl<false, Components...>(*this);
}

template<typename T>
void system::after_impl() {
    size_t index = _internal::system_type_index_to_index[typeid(T)];

#ifdef VOLT_DEVELOPMENT
    if (std::find(_after.begin(), _after.end(), index) != _after.end()) {
        VOLT_LOG_WARNING("System " + _internal::system_name(index) +
                " has already been added to \"after\" dependency list of system " +
                _internal::system_name(instances.size()) + 
                ". Ignoring. Please remove redundant code.")
        return;
    }
#endif

    _after.push_back(index);
}

template<typename T1, typename T2, typename... Tn>
void system::after_impl() {
    after_impl<T1>();
    after_impl<T2, Tn...>();
}

template<bool Unique>
void system::lock_impl(system &system, size_t component_index) {
#ifdef VOLT_DEVELOPMENT
    // Find user errors
    for (int32_t i = 0; i < 2; i++) {
        util::dynamic_bitset &mask = (i == 0 ? system.unique_mask : system.shared_mask);

        // If vector contains system
        if (mask.test(component_index)) {
            VOLT_LOG_WARNING("System " + _internal::system_name(instances.size()) + " has already " +
                    (i == 0 ? "unique" : "shared") + "-locked " + _internal::component_name(component_index) +
                    ". Ignoring. Please remove redundant code.")
            return;
        }
    }
#endif
	
    if constexpr (Unique)
        system.unique_mask.set(component_index);
    else
        system.shared_mask.set(component_index);
}

template<bool Unique, typename T>
void system::lock_impl(system &system) {
    lock_impl<Unique>(system, _internal::component_type_index_to_index[typeid(T)]);
}

template<bool Unique, typename T1, typename T2, typename... Tn>
void system::lock_impl(system &system) {
	lock_impl<Unique, T1>(system);
	lock_impl<Unique, T2, Tn...>(system);
}

}