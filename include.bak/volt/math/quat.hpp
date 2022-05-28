#pragma once

#include "../macros.hpp"

#include "math.hpp"
#include "mat3.hpp"
#include "vec3.hpp"

namespace volt::math {

template<scalar T>
class quat {
public:
	static const quat<T> identity;

	T w;
	vec3<T> v;

	quat();

	template<scalar U = T>
	quat(T w, const vec3<U> &v = vec3<U>(0));

	quat(T w, T x, T y, T z);

	template<scalar U>
	static quat<T> axis_angle(T angle, const vec3<U> &axis);

	template<scalar U>
	static quat<T> from_basis(const mat3<U> &basis);

	// roll (Z) -> pitch (X) -> yaw (Y)
	template<scalar U>
	static quat<T> from_euler(const vec3<U> &euler);

	mat3<T> to_basis() const;

	vec3<T> to_euler() const;

#pragma region Operators

	template<scalar U>
	friend std::ostream &operator<<(std::ostream &lhs, const quat<U> &rhs);

	template<scalar U>
	bool operator==(const quat<U> &rhs) const;

	template<scalar U>
	bool operator!=(const quat<U> &rhs) const;

	quat<T> &operator++();

	quat<T> &operator--();

	quat<T> operator++(int32_t);

	quat<T> operator--(int32_t);

	quat<T> operator-() const;

	// Quaternion + Quaternion

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	quat<Ret> operator+(const quat<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	quat<Ret> operator-(const quat<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	quat<Ret> operator*(const quat<U> &rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	quat<Ret> operator/(const quat<U> &rhs) const;

	template<scalar U>
	quat<T> &operator+=(const quat<U> &rhs);

	template<scalar U>
	quat<T> &operator-=(const quat<U> &rhs);

	template<scalar U>
	quat<T> &operator*=(const quat<U> &rhs);

	template<scalar U>
	quat<T> &operator/=(const quat<U> &rhs);

	// Quaternion + Scalar

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	quat<Ret> operator+(U rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	quat<Ret> operator-(U rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	quat<Ret> operator*(U rhs) const;

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	quat<Ret> operator/(U rhs) const;

	template<scalar U>
	quat<T> &operator+=(U rhs);

	template<scalar U>
	quat<T> &operator-=(U rhs);

	template<scalar U>
	quat<T> &operator*=(U rhs);

	template<scalar U>
	quat<T> &operator/=(U rhs);

	// Scalar + Quaternion

	template<scalar L, scalar R, scalar Ret>
	friend quat<Ret> operator+(L lhs, const quat<R> &rhs);

	template<scalar L, scalar R, scalar Ret>
	friend quat<Ret> operator-(L lhs, const quat<R> &rhs);

	template<scalar L, scalar R, scalar Ret>
	friend quat<Ret> operator*(L lhs, const quat<R> &rhs);

	template<scalar L, scalar R, scalar Ret>
	friend quat<Ret> operator/(L lhs, const quat<R> &rhs);

	// Quaternion + Vector

	template<scalar U, scalar Ret = std::common_type_t<T, U>>
	vec3<Ret> operator*(const vec3<U> &rhs) const;

#pragma endregion

};

using iquat = quat<int32_t>;
using uquat = quat<uint32_t>;
using fquat = quat<float>;
using dquat = quat<double>;

template<scalar From, scalar To,
		scalar Ret = std::common_type_t<From, To>>
Ret angle(const quat<From> &from, const quat<To> &to);

template<scalar T>
quat<T> conjugate(const quat<T> &quat);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
Ret dot(const quat<A> &lhs, const quat<B> &rhs);

template<scalar T>
quat<T> inverse(const quat<T> &quat);

template<scalar A, scalar B, scalar Epsilon = decltype(epsilon)>
bool is_approx(const quat<A> &a, const quat<B> &b,
		Epsilon epsilon = math::epsilon);

template<scalar T, scalar Epsilon = decltype(epsilon)>
bool is_normalized(const quat<T> &quat,
		Epsilon epsilon = math::epsilon);

template<scalar T>
T length(const quat<T> &quat);

template<scalar T>
quat<T> normalize(const quat<T> &quat);

template<scalar From, scalar To, scalar Weight,
		scalar Ret = std::common_type_t<From, To, Weight>>
quat<Ret> slerp(const quat<From> &from, const quat<To> &to, Weight weight);

}

#include "quat.inl"
