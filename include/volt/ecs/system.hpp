#pragma once

#include "../macros.hpp"

#include <atomic>

#include "../util/util.hpp"

namespace volt::ecs {

// DISCLAIMER: System has no public user interface;
// All public members are for internal use
class system {
public:
    virtual ~system() = default;

    VOLT_API static void _update_all(float delta);

    VOLT_API static void _delete_all();

protected:
    system() = default;

    virtual void update(float delta) const = 0;

    template<typename... Systems>
	void after();

    template<typename... Components>
	void unique_lock();

	template<typename... Components>
	void shared_lock();

	VOLT_API void unique_lock_all();

	VOLT_API void shared_lock_all();

    // Forces the system to be executed on the main thread
    // This is particularly useful for OS event handling
    VOLT_API void use_main_thread();

private:
    VOLT_API static std::vector<std::unique_ptr<system>> instances;
    VOLT_API static std::vector<size_t> root_systems;

    // Set during construction
    util::dynamic_bitset unique_mask, shared_mask;
    std::vector<size_t> _after;
    bool _use_main_thread = false;

    // Generated after construction for use during preparation and execution
    std::vector<size_t> before;
    std::atomic_size_t unfulfilled_dependency_count;
    
    static bool is_dependent_on(size_t dependant, size_t dependency);

    template<typename T>
    void after_impl();

    template<typename T1, typename T2, typename... Tn>
    void after_impl();

    template<bool Unique>
    void lock_impl(system &system, size_t component_index);

    template<bool Unique, typename T>
    void lock_impl(system &system);

    template<bool Unique, typename T1, typename T2, typename... Tn>
    void lock_impl(system &system);
};

}

#include "system.inl"
