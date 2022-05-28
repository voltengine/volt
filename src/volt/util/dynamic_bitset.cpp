#include <volt/pch.hpp>
#include <volt/util/dynamic_bitset.hpp>

namespace volt::util {

dynamic_bitset::dynamic_bitset(size_t size) {
	resize(size);
}

dynamic_bitset::dynamic_bitset(const dynamic_bitset &other)
		: chunks(other.chunks) {}

dynamic_bitset::dynamic_bitset(dynamic_bitset &&other)
		: chunks(std::move(other.chunks)) {}

std::ostream &operator<<(std::ostream &lhs, const dynamic_bitset &rhs) {
	for (auto it = rhs.chunks.rbegin(); it != rhs.chunks.rend(); it++)
		lhs << std::bitset<dynamic_bitset::chunk_size>(*it);

	return lhs;
}

bool dynamic_bitset::operator==(const dynamic_bitset &rhs) const {
	return chunks == rhs.chunks;
}

dynamic_bitset dynamic_bitset::operator~() const {
	return dynamic_bitset(*this).flip();
}

dynamic_bitset dynamic_bitset::operator&(const dynamic_bitset &rhs) const {
	size_t chunk_count = std::max(chunks.size(), rhs.chunks.size());
	size_t size = chunk_count * chunk_size;

	dynamic_bitset bitset(size);

	for (size_t i = 0; i < chunk_count; i++)
		bitset.chunks[i] = chunks[i] & rhs.chunks[i];

	return bitset;
}

dynamic_bitset dynamic_bitset::operator|(const dynamic_bitset &rhs) const {
	size_t chunk_count = std::max(chunks.size(), rhs.chunks.size());
	size_t size = chunk_count * chunk_size;

	dynamic_bitset bitset(size);

	for (size_t i = 0; i < chunk_count; i++)
		bitset.chunks[i] = chunks[i] | rhs.chunks[i];

	return bitset;
}

dynamic_bitset dynamic_bitset::operator^(const dynamic_bitset &rhs) const {
	size_t chunk_count = std::max(chunks.size(), rhs.chunks.size());
	size_t size = chunk_count * chunk_size;

	dynamic_bitset bitset(size);

	for (size_t i = 0; i < chunk_count; i++)
		bitset.chunks[i] = chunks[i] ^ rhs.chunks[i];

	return bitset;
}

size_t dynamic_bitset::size() const {
	return chunks.size() * chunk_size;
}

size_t dynamic_bitset::count(bool value) const {
	size_t count = 0;

	for (const chunk_type &chunk : chunks) {
		for (size_t i = 0; i < chunk_size; i++) {
			if (static_cast<bool>(chunk & (static_cast<chunk_type>(1UL) << i)) == value)
				count++;
		}
	}

	return count;
}

bool dynamic_bitset::test(size_t pos) const {
	size_t chunk_index = pos / chunk_size;
	size_t index       = pos % chunk_size;

	const chunk_type &chunk = chunks[chunk_index];
	return chunk & (static_cast<chunk_type>(1UL) << index);
}

dynamic_bitset &dynamic_bitset::set(bool value) {
	for (chunk_type &chunk : chunks)
		chunk = value ? static_cast<chunk_type>(-1) : static_cast<chunk_type>(0);

	return *this;
}

dynamic_bitset &dynamic_bitset::set(size_t pos, bool value) {
	size_t chunk_index = pos / chunk_size;
	size_t index       = pos % chunk_size;

	chunk_type &chunk = chunks[chunk_index];
	if (value)
		chunk |= static_cast<chunk_type>(1UL) << index;
	else
		chunk &= ~(static_cast<chunk_type>(1UL) << index);

	return *this;
}

dynamic_bitset &dynamic_bitset::reset() {
	for (chunk_type &chunk : chunks)
		chunk = static_cast<chunk_type>(0);

	return *this;
}

dynamic_bitset &dynamic_bitset::reset(size_t pos) {
	size_t chunk_index = pos / chunk_size;
	size_t index       = pos % chunk_size;

	chunk_type &chunk = chunks[chunk_index];
	chunk &= ~(static_cast<chunk_type>(1UL) << index);

	return *this;
}

dynamic_bitset &dynamic_bitset::flip() {
	for (chunk_type &chunk : chunks)
		chunk = ~chunk;

	return *this;
}

dynamic_bitset &dynamic_bitset::flip(size_t pos) {
	size_t chunk_index = pos / chunk_size;
	size_t index       = pos % chunk_size;

	chunk_type &chunk = chunks[chunk_index];
	chunk ^= static_cast<chunk_type>(1UL) << index;

	return *this;
}

size_t dynamic_bitset::resize(size_t size) {
	size_t capacity = (size + chunk_size - 1) / chunk_size;
	chunks.resize(capacity, 0);
	return capacity;
}

}