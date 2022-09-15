#pragma once

#include "../macros.hpp"

#include <ostream>

#include "math.hpp"
#include "vec3.hpp"

namespace volt::math {

template<scalar T>
class vec4 {
public:
	static const vec4<T> zero;
	static const vec4<T> one;
	static const vec4<T> left;
	static const vec4<T> right;
	static const vec4<T> down;
	static const vec4<T> up;
	static const vec4<T> forward;
	static const vec4<T> backward;
	static const vec4<T> past;
	static const vec4<T> future;

	union {
		struct { T x, y, z, w; };
		T data[4];
	};

	constexpr vec4();

	constexpr vec4(T all);

	constexpr vec4(T x, T y, T z, T w);

	template<scalar U>
	constexpr vec4(const vec4<U> &other);

	template<scalar U>
	constexpr vec4(const vec3<U> &other, float w = 0);

	template<scalar U>
	constexpr vec4(const vec2<U> &other, float z = 0, float w = 0);

#pragma region Operators

	template<scalar U>
	operator vec3<U>() const;

	template<scalar U>
	operator vec2<U>() const;

	template<scalar U>
	friend std::ostream &operator<<(std::ostream &lhs, const vec4<U> &rhs);

	T &operator[](size_t index);

	const T &operator[](size_t index) const;

	template<scalar U>
	vec4<bool> operator==(const vec4<U> &rhs) const;

	template<scalar U>
	vec4<bool> operator!=(const vec4<U> &rhs) const;

	template<scalar U>
	vec4<bool> operator<=(const vec4<U> &rhs) const;

	template<scalar U>
	vec4<bool> operator>=(const vec4<U> &rhs) const;

	template<scalar U>
	vec4<bool> operator<(const vec4<U> &rhs) const;

	template<scalar U>
	vec4<bool> operator>(const vec4<U> &rhs) const;

	template<boolean U>
	friend vec4<bool> operator!(const vec4<U> &mat);

	vec4<T> operator-() const;

	// Vector + Vector

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec4<Ret> operator+(const vec4<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec4<Ret> operator-(const vec4<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec4<Ret> operator*(const vec4<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec4<Ret> operator/(const vec4<U> &rhs) const;

	template<scalar U>
	vec4<T> &operator+=(const vec4<U> &rhs);

	template<scalar U>
	vec4<T> &operator-=(const vec4<U> &rhs);

	template<scalar U>
	vec4<T> &operator*=(const vec4<U> &rhs);

	template<scalar U>
	vec4<T> &operator/=(const vec4<U> &rhs);

	// Vector + Scalar

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec4<Ret> operator*(U rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec4<Ret> operator/(U rhs) const;

	template<scalar U>
	vec4<T> &operator*=(U rhs);

	template<scalar U>
	vec4<T> &operator/=(U rhs);

	// Scalar + Vector

	template<scalar L, scalar R, scalar Ret>
	friend vec4<Ret> operator*(L lhs, const vec4<R> &rhs);

#pragma endregion

};

using bvec4 = vec4<bool>;
using ivec4 = vec4<int32_t>;
using uvec4 = vec4<uint32_t>;
using fvec4 = vec4<float>;
using dvec4 = vec4<double>;

template<boolean T>
bool all(const vec4<T> &vec);

template<boolean T>
bool any(const vec4<T> &vec);

template<scalar A, scalar B,
		floating_point Ret = std::common_type_t<A, B>>
Ret distance(const vec4<A> &a, const vec4<B> &b);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
Ret dot(const vec4<A> &a, const vec4<B> &b);

template<scalar T, scalar Epsilon = decltype(epsilon)>
bool is_normalized(const vec4<T> &vec, Epsilon epsilon = math::epsilon);

template<floating_point T>
T length(const vec4<T> &vec);

template<floating_point T>
vec4<T> normalize(const vec4<T> &vec);

template<floating_point To, floating_point From,
		scalar Ret = std::common_type_t<To, From>>
vec4<Ret> proj(const vec4<To> &to, const vec4<From> &from);

#pragma region Component-Wise Math Wrappers

template<floating_point X>
vec4<X> fract(const vec4<X> &x);

template<scalar From, scalar To, scalar Weight,
		scalar Ret = std::common_type_t<From, To, Weight>>
vec4<Ret> lerp(const vec4<From> &from, const vec4<To> &to, Weight weight);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
vec4<Ret> max(const vec4<A> &a, const vec4<B> &b);

template<scalar A, scalar B, scalar... Others,
		scalar Ret = std::common_type_t<A, B, Others...>>
vec4<Ret> max(const vec4<A> &a, const vec4<B> &b,
		const vec4<Others> &...others);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
vec4<Ret> min(const vec4<A> &a, const vec4<B> &b);

template<scalar A, scalar B, scalar... Others,
		scalar Ret = std::common_type_t<A, B, Others...>>
vec4<Ret> min(const vec4<A> &a, const vec4<B> &b,
		const vec4<Others> &...others);

template<scalar X, scalar Y,
		scalar Ret = std::common_type_t<X, Y>>
vec4<Ret> mod(const vec4<X> &x, const vec4<Y> &y);

#pragma endregion

}

namespace std {

template<volt::math::scalar T>
class hash<volt::math::vec4<T>> {
public:
	std::size_t operator()(volt::math::vec4<T> vec) const;
};

}

#include "vec4.inl"
