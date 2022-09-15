#pragma once

#include "../macros.hpp"

#include <ostream>

#include "math.hpp"
#include "vec2.hpp"

namespace volt::math {

template<scalar T>
class vec3 {
public:
	static const vec3<T> zero;
	static const vec3<T> one;
	static const vec3<T> left;
	static const vec3<T> right;
	static const vec3<T> down;
	static const vec3<T> up;
	static const vec3<T> forward;
	static const vec3<T> backward;

	union {
		struct { T x, y, z; };
		T data[3];
	};

	constexpr vec3();

	constexpr vec3(T all);

	constexpr vec3(T x, T y, T z);

	template<scalar U>
	constexpr vec3(const vec3<U> &other);

	template<scalar U>
	constexpr vec3(const vec2<U> &other, float z = 0);

#pragma region Operators

	template<scalar U>
	operator vec2<U>() const;

	template<scalar U>
	friend std::ostream &operator<<(std::ostream &lhs, const vec3<U> &rhs);

	T &operator[](size_t index);

	const T &operator[](size_t index) const;

	template<scalar U>
	vec3<bool> operator==(const vec3<U> &rhs) const;

	template<scalar U>
	vec3<bool> operator!=(const vec3<U> &rhs) const;

	template<scalar U>
	vec3<bool> operator<=(const vec3<U> &rhs) const;

	template<scalar U>
	vec3<bool> operator>=(const vec3<U> &rhs) const;

	template<scalar U>
	vec3<bool> operator<(const vec3<U> &rhs) const;

	template<scalar U>
	vec3<bool> operator>(const vec3<U> &rhs) const;

	template<boolean U>
	friend vec3<bool> operator!(const vec3<U> &mat);

	vec3<T> operator-() const;

	// Vector + Vector

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec3<Ret> operator+(const vec3<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec3<Ret> operator-(const vec3<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec3<Ret> operator*(const vec3<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec3<Ret> operator/(const vec3<U> &rhs) const;

	template<scalar U>
	vec3<T> &operator+=(const vec3<U> &rhs);

	template<scalar U>
	vec3<T> &operator-=(const vec3<U> &rhs);

	template<scalar U>
	vec3<T> &operator*=(const vec3<U> &rhs);

	template<scalar U>
	vec3<T> &operator/=(const vec3<U> &rhs);

	// Vector + Scalar

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec3<Ret> operator*(U rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec3<Ret> operator/(U rhs) const;

	template<scalar U>
	vec3<T> &operator*=(U rhs);

	template<scalar U>
	vec3<T> &operator/=(U rhs);

	// Scalar + Vector

	template<scalar L, scalar R, scalar Ret>
	friend vec3<Ret> operator*(L lhs, const vec3<R> &rhs);

#pragma endregion

};

using bvec3 = vec3<bool>;
using ivec3 = vec3<int32_t>;
using uvec3 = vec3<uint32_t>;
using fvec3 = vec3<float>;
using dvec3 = vec3<double>;

template<boolean T>
bool all(const vec3<T> &vec);

template<boolean T>
bool any(const vec3<T> &vec);

template<scalar L, scalar R,
		scalar Ret = std::common_type_t<L, R>>
vec3<Ret> cross(const vec3<L> &lhs, const vec3<R> &rhs);

template<scalar A, scalar B,
		floating_point Ret = std::common_type_t<A, B>>
Ret distance(const vec3<A> &a, const vec3<B> &b);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
Ret dot(const vec3<A> &a, const vec3<B> &b);

template<scalar T, scalar Epsilon = decltype(epsilon)>
bool is_normalized(const vec3<T> &vec, Epsilon epsilon = math::epsilon);

template<floating_point T>
T length(const vec3<T> &vec);

template<floating_point T>
vec3<T> normalize(const vec3<T> &vec);

template<floating_point To, floating_point From,
		scalar Ret = std::common_type_t<To, From>>
vec3<Ret> proj(const vec3<To> &to, const vec3<From> &from);

template<scalar Incident, scalar Normal,
		scalar Ret = std::common_type_t<Incident, Normal>>
vec3<Ret> reflect(const vec3<Incident> &incident,
		const vec3<Normal> &normal);

template<scalar Incident, scalar Normal, scalar IorRatio,
		scalar Ret = std::common_type_t<Incident, Normal, IorRatio>>
vec3<Ret> refract(const vec3<Incident> &incident,
		const vec3<Normal> &normal, IorRatio ior_ratio);

#pragma region Component-Wise Math Wrappers

template<scalar X>
vec3<X> abs(const vec3<X> &x);

template<floating_point X>
vec3<X> fract(const vec3<X> &x);

template<scalar From, scalar To, scalar Weight,
		scalar Ret = std::common_type_t<From, To, Weight>>
vec3<Ret> lerp(const vec3<From> &from, const vec3<To> &to, const vec3<Weight> &weight);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
vec3<Ret> max(const vec3<A> &a, const vec3<B> &b);

template<scalar A, scalar B, scalar... Others,
		scalar Ret = std::common_type_t<A, B, Others...>>
vec3<Ret> max(const vec3<A> &a, const vec3<B> &b,
		const vec3<Others> &...others);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
vec3<Ret> min(const vec3<A> &a, const vec3<B> &b);

template<scalar A, scalar B, scalar... Others,
		scalar Ret = std::common_type_t<A, B, Others...>>
vec3<Ret> min(const vec3<A> &a, const vec3<B> &b,
		const vec3<Others> &...others);

template<scalar X, scalar Y,
		scalar Ret = std::common_type_t<X, Y>>
vec3<Ret> mod(const vec3<X> &x, const vec3<Y> &y);

template<scalar X, scalar Power,
		scalar Ret = std::common_type_t<X, Power>>
vec3<Ret> pow(const vec3<X> &x, const vec3<Power> &power);

template<scalar X>
vec3<X> saturate(const vec3<X> &x);

#pragma endregion

}

namespace std {

template<volt::math::scalar T>
class hash<volt::math::vec3<T>> {
public:
	std::size_t operator()(volt::math::vec3<T> vec) const;
};

}

#include "vec3.inl"
