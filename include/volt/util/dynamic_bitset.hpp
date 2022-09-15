#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <ostream>
#include <vector>

namespace volt::util {

class dynamic_bitset {
public:
    dynamic_bitset() = default;

    VOLT_API dynamic_bitset(size_t size);

    dynamic_bitset(const dynamic_bitset &other) = default;

    dynamic_bitset(dynamic_bitset &&other) = default;

	VOLT_API friend std::ostream &operator<<(std::ostream &lhs, const dynamic_bitset &rhs);

    dynamic_bitset &operator=(const dynamic_bitset &other) = default;
    
    dynamic_bitset &operator=(dynamic_bitset &&other) = default;

    VOLT_API bool operator==(const dynamic_bitset &rhs) const;
    
    VOLT_API dynamic_bitset &operator&=(const dynamic_bitset &rhs);

    VOLT_API dynamic_bitset &operator|=(const dynamic_bitset &rhs);

    VOLT_API dynamic_bitset &operator^=(const dynamic_bitset &rhs);

    VOLT_API dynamic_bitset operator~() const;

    VOLT_API dynamic_bitset operator&(const dynamic_bitset &rhs) const;

    VOLT_API dynamic_bitset operator|(const dynamic_bitset &rhs) const;

    VOLT_API dynamic_bitset operator^(const dynamic_bitset &rhs) const;

    VOLT_API size_t size() const;

    VOLT_API size_t count(bool value = true) const;

    VOLT_API bool test(size_t pos) const;

    VOLT_API bool all(bool value = true) const;

    VOLT_API dynamic_bitset &set(bool value = true);

    VOLT_API dynamic_bitset &set(size_t pos, bool value = true);

    VOLT_API dynamic_bitset &reset();

    VOLT_API dynamic_bitset &reset(size_t pos);

    VOLT_API dynamic_bitset &flip();

    VOLT_API dynamic_bitset &flip(size_t pos);

    VOLT_API size_t resize(size_t size);

private:
    using chunk_type = uint32_t;

    static constexpr size_t chunk_size = sizeof(chunk_type) * 8;

    std::vector<chunk_type> chunks;
};

}