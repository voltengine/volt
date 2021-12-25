#pragma once

#include "../macros.hpp"

#include "math.hpp"

namespace volt::math {

template<scalar T>
class vec2 {
public:
	static const vec2<T> zero;
	static const vec2<T> one;
	static const vec2<T> left;
	static const vec2<T> right;
	static const vec2<T> down;
	static const vec2<T> up;

	union {
		struct { T x, y; };
		T data[2];
	};

	constexpr vec2();

	constexpr vec2(T all);

	constexpr vec2(T x, T y);

	template<scalar U>
	constexpr vec2(const vec2<U> &other);

#pragma region Operators

	template<scalar U>
	friend std::ostream &operator<<(std::ostream &lhs, const vec2<U> &rhs);

	T &operator[](size_t index);

	const T &operator[](size_t index) const;

	template<scalar U>
	vec2<bool> operator==(const vec2<U> &rhs) const;

	template<scalar U>
	vec2<bool> operator!=(const vec2<U> &rhs) const;

	template<scalar U>
	vec2<bool> operator<=(const vec2<U> &rhs) const;

	template<scalar U>
	vec2<bool> operator>=(const vec2<U> &rhs) const;

	template<scalar U>
	vec2<bool> operator<(const vec2<U> &rhs) const;

	template<scalar U>
	vec2<bool> operator>(const vec2<U> &rhs) const;

	template<boolean U>
	friend vec2<bool> operator!(const vec2<U> &mat);

	vec2<T> operator-() const;

	// Vector + Vector

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec2<Ret> operator+(const vec2<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec2<Ret> operator-(const vec2<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec2<Ret> operator*(const vec2<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec2<Ret> operator/(const vec2<U> &rhs) const;

	template<scalar U>
	vec2<T> &operator+=(const vec2<U> &rhs);

	template<scalar U>
	vec2<T> &operator-=(const vec2<U> &rhs);

	template<scalar U>
	vec2<T> &operator*=(const vec2<U> &rhs);

	template<scalar U>
	vec2<T> &operator/=(const vec2<U> &rhs);

	// Vector + Scalar

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec2<Ret> operator*(U rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec2<Ret> operator/(U rhs) const;

	template<scalar U>
	vec2<T> &operator*=(U rhs);

	template<scalar U>
	vec2<T> &operator/=(U rhs);

	// Scalar + Vector

	template<scalar L, scalar R, scalar Ret>
	friend vec2<Ret> operator*(L lhs, const vec2<R> &rhs);

#pragma endregion

};

using bvec2 = vec2<bool>;
using ivec2 = vec2<int32_t>;
using uvec2 = vec2<uint32_t>;
using fvec2 = vec2<float>;
using dvec2 = vec2<double>;

template<boolean T>
bool all(const vec2<T> &vec);

template<boolean T>
bool any(const vec2<T> &vec);

template<scalar A, scalar B,
		floating_point Ret = std::common_type_t<A, B>>
Ret distance(const vec2<A> &a, const vec2<B> &b);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
Ret dot(const vec2<A> &a, const vec2<B> &b);

template<scalar T, scalar Epsilon = decltype(epsilon)>
bool is_normalized(const vec2<T> &vec, Epsilon epsilon = math::epsilon);

template<floating_point T>
T length(const vec2<T> &vec);

template<floating_point T>
vec2<T> normalize(const vec2<T> &vec);

template<floating_point To, floating_point From,
		scalar Ret = std::common_type_t<To, From>>
vec2<Ret> proj(const vec2<To> &to, const vec2<From> &from);

#pragma region Component-Wise Math Wrappers

template<floating_point X>
vec2<X> fract(const vec2<X> &x);

template<scalar From, scalar To, scalar Weight,
		scalar Ret = std::common_type_t<From, To, Weight>>
vec2<Ret> lerp(const vec2<From> &from, const vec2<To> &to, Weight weight);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
vec2<Ret> max(const vec2<A> &a, const vec2<B> &b);

template<scalar A, scalar B, scalar... Others,
		scalar Ret = std::common_type_t<A, B, Others...>>
vec2<Ret> max(const vec2<A> &a, const vec2<B> &b,
		const vec2<Others> &...others);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
vec2<Ret> min(const vec2<A> &a, const vec2<B> &b);

template<scalar A, scalar B, scalar... Others,
		scalar Ret = std::common_type_t<A, B, Others...>>
vec2<Ret> min(const vec2<A> &a, const vec2<B> &b,
		const vec2<Others> &...others);

template<scalar X, scalar Y,
		scalar Ret = std::common_type_t<X, Y>>
vec2<Ret> mod(const vec2<X> &x, const vec2<Y> &y);

#pragma endregion

}

namespace std {

template<volt::math::scalar T>
struct hash<volt::math::vec2<T>> {
	std::size_t operator()(volt::math::vec2<T> vec) const;
};

}

#include "vec2.inl"
