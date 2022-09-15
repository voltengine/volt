#pragma once

#include "../macros.hpp"

#include "math.hpp"
#include "vec3.hpp"

namespace volt::math {

template<scalar T>
class mat3 {
public:
	static const mat3<T> zero;
	static const mat3<T> identity;

	union {
		class {
		public:
			vec3<T> x, y, z;
		};
		vec3<T> data[3];
	};

	mat3();

	mat3(T identity);

	mat3(const vec3<T> &x, const vec3<T> &y, const vec3<T> &z);

	mat3(T xx, T xy, T xz,
		 T yx, T yy, T yz,
		 T zx, T zy, T zz);

	mat3(const T *array);

	template<scalar U>
	mat3(const mat3<U> &other);

#pragma region Operators

	template<scalar U>
	friend std::ostream &operator<<(std::ostream &lhs, const mat3<U> &rhs);

	vec3<T> &operator[](size_t index);

	const vec3<T> &operator[](size_t index) const;

	template<scalar U>
	mat3<bool> operator==(const mat3<U> &rhs) const;

	template<scalar U>
	mat3<bool> operator!=(const mat3<U> &rhs) const;

	template<scalar U>
	mat3<bool> operator<=(const mat3<U> &rhs) const;

	template<scalar U>
	mat3<bool> operator>=(const mat3<U> &rhs) const;

	template<scalar U>
	mat3<bool> operator<(const mat3<U> &rhs) const;

	template<scalar U>
	mat3<bool> operator>(const mat3<U> &rhs) const;

	template<boolean U>
	friend mat3<bool> operator!(const mat3<U> &mat);

	mat3<T> operator-() const;

	// Matrix + Matrix

	template<scalar U>
	auto operator+(const mat3<U> &rhs) const;
	
	template<scalar U>
	auto operator-(const mat3<U> &rhs) const;
	
	template<scalar U>
	auto operator*(const mat3<U> &rhs) const;

	template<scalar U>
	mat3<T> &operator+=(const mat3<U> &rhs);

	template<scalar U>
	mat3<T> &operator-=(const mat3<U> &rhs);

	template<scalar U>
	mat3<T> &operator*=(const mat3<U> &rhs);

	// Matrix + Scalar

	template<scalar U>
	auto operator*(U rhs) const;

	template<scalar U>
	auto operator/(U rhs) const;

	template<scalar U>
	mat3<T> &operator*=(U rhs);

	template<scalar U>
	mat3<T> &operator/=(U rhs);

	// Scalar + Matrix

	template<scalar L, scalar R>
	friend auto operator*(L lhs, const mat3<R> &rhs);

	// Matrix + Vector

	template<scalar U>
	auto operator*(const vec3<U> &rhs) const;

#pragma endregion

};

using bmat3 = mat3<bool>;
using imat3 = mat3<int32_t>;
using umat3 = mat3<uint32_t>;
using fmat3 = mat3<float>;
using dmat3 = mat3<double>;

template<scalar L, scalar R>
auto operator*(L lhs, const mat3<R> &rhs);

template<boolean T>
bool all(const mat3<T> &mat);

template<boolean T>
bool any(const mat3<T> &mat);

template<scalar T>
T determinant(const mat3<T> &mat);

template<floating_point T>
mat3<T> inverse(const mat3<T> &mat);

template<scalar A, scalar B,
		scalar Epsilon = decltype(epsilon)>
bool is_approx(const mat3<A> &a, const mat3<B> &b,
		Epsilon epsilon = math::epsilon);

template<scalar T, scalar Epsilon = decltype(epsilon)>
bool is_orthogonal(const mat3<T> &mat,
		Epsilon epsilon = math::epsilon);

template<scalar T, scalar Epsilon = decltype(epsilon)>
bool is_orthonormal(const mat3<T> &mat,
		Epsilon epsilon = math::epsilon);

template<floating_point T>
mat3<T> orthogonalize(const mat3<T> &mat);

template<floating_point T>
mat3<T> orthonormalize(const mat3<T> &mat);

template<scalar T>
mat3<T> transpose(const mat3<T> &mat);

}

#include "mat3.inl"
