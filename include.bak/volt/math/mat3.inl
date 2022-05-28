#include "../error.hpp"

namespace volt::math {

template<scalar T>
mat3<T>::mat3() : mat3(1) {}

template<scalar T>
mat3<T>::mat3(T identity) :
		x(identity, 0, 0),
		y(0, identity, 0),
		z(0, 0, identity) {}

template<scalar T>
mat3<T>::mat3(
		const vec3<T> &x,
		const vec3<T> &y,
		const vec3<T> &z) :
		x(x), y(y), z(z) {}

template<scalar T>
mat3<T>::mat3(
		T xx, T xy, T xz,
		T yx, T yy, T yz,
		T zx, T zy, T zz) :
		x(xx, xy, xz),
		y(yx, yy, yz),
		z(zx, zy, zz) {}

template<scalar T>
mat3<T>::mat3(const T *array) :
	x(array[0], array[1], array[2]),
	y(array[3], array[4], array[5]),
	z(array[6], array[7], array[8]) {}

template<scalar T>
template<scalar U>
mat3<T>::mat3(const mat3<U> &other) :
		mat3(other.x, other.y, other.z) {}

#pragma region Operators

template<scalar U>
std::ostream &operator<<(std::ostream &lhs, const mat3<U> &rhs) {
	std::stringstream lines[3], final_ss;

	for (size_t i = 0; i < 3; i++) {
		size_t max_len = 0;
		for (size_t j = 0; j < 3; j++) {
			size_t len = util::to_string(rhs[i][j]).size();
			if (len > max_len)
				max_len = len;
		}

		for (size_t j = 0; j < 3; j++) {
			lines[j] << std::setw(max_len) << util::to_string(rhs[i][j]);
			if (i != 2)
				lines[j] << " | ";
		}
	}

	for (size_t i = 0; i < 3; i++) {
		if (i != 2) lines[i] << '\n';
		final_ss << lines[i].str();
	}

	return lhs << final_ss.str();
}


template<scalar T>
vec3<T> &mat3<T>::operator[](size_t index) {
	return data[index];
}

template<scalar T>
const vec3<T> &mat3<T>::operator[](size_t index) const {
	return data[index];
}

template<scalar T>
template<scalar U>
mat3<bool> mat3<T>::operator==(const mat3<U> &rhs) const {
	return mat3<bool>(x == rhs.x, y == rhs.y, z == rhs.z);
}

template<scalar T>
template<scalar U>
mat3<bool> mat3<T>::operator!=(const mat3<U> &rhs) const {
	return mat3<bool>(x != rhs.x, y != rhs.y, z != rhs.z);
}

template<scalar T>
template<scalar U>
mat3<bool> mat3<T>::operator<=(const mat3<U> &rhs) const {
	return mat3<bool>(x <= rhs.x, y <= rhs.y, z <= rhs.z);
}

template<scalar T>
template<scalar U>
mat3<bool> mat3<T>::operator>=(const mat3<U> &rhs) const {
	return mat3<bool>(x >= rhs.x, y >= rhs.y, z >= rhs.z);
}

template<scalar T>
template<scalar U>
mat3<bool> mat3<T>::operator<(const mat3<U> &rhs) const {
	return mat3<bool>(x < rhs.x, y < rhs.y, z < rhs.z);
}

template<scalar T>
template<scalar U>
mat3<bool> mat3<T>::operator>(const mat3<U> &rhs) const {
	return mat3<bool>(x > rhs.x, y > rhs.y, z > rhs.z);
}

template<boolean U>
mat3<bool> operator!(const mat3<U> &mat) {
	return mat3<bool>(!mat.x, !mat.y, !mat.z);
}

template<scalar T>
mat3<T> mat3<T>::operator-() const {
	return mat3<T>(-x, -y, -z);
}

// Matrix + Matrix

template<scalar T>
template<scalar U>
auto mat3<T>::operator+(const mat3<U> &rhs) const {
	return mat3(x + rhs.x, y + rhs.y, z + rhs.z);
}

template<scalar T>
template<scalar U>
auto mat3<T>::operator-(const mat3<U> &rhs) const {
	return mat3(x - rhs.x, y - rhs.y, z - rhs.z);
}

template<scalar T>
template<scalar U>
auto mat3<T>::operator*(const mat3<U> &rhs) const {
	return mat3(
		x * rhs.x.x + y * rhs.x.y + z * rhs.x.z,
		x * rhs.y.x + y * rhs.y.y + z * rhs.y.z,
		x * rhs.z.x + y * rhs.z.y + z * rhs.z.z
	);
}

template<scalar T>
template<scalar U>
mat3<T> &mat3<T>::operator+=(const mat3<U> &rhs) {
	return *this = *this + rhs;
}

template<scalar T>
template<scalar U>
mat3<T> &mat3<T>::operator-=(const mat3<U> &rhs) {
	return *this = *this - rhs;
}

template<scalar T>
template<scalar U>
mat3<T> &mat3<T>::operator*=(const mat3<U> &rhs) {
	return *this = *this * rhs;
}

// Matrix + Scalar

template<scalar T>
template<scalar U>
auto mat3<T>::operator*(U rhs) const {
	return mat3(
		x.x * rhs, x.y * rhs, x.z * rhs,
		y.x * rhs, y.y * rhs, y.z * rhs,
		z.x * rhs, z.y * rhs, z.z * rhs
	);
}

template<scalar T>
template<scalar U>
auto mat3<T>::operator/(U rhs) const {
	return mat3(
		x.x / rhs, x.y / rhs, x.z / rhs,
		y.x / rhs, y.y / rhs, y.z / rhs,
		z.x / rhs, z.y / rhs, z.z / rhs
	);
}

template<scalar T>
template<scalar U>
mat3<T> &mat3<T>::operator*=(U rhs) {
	return *this = *this * rhs;
}

template<scalar T>
template<scalar U>
mat3<T> &mat3<T>::operator/=(U rhs) {
	return *this = *this / rhs;
}

// Scalar + Matrix

template<scalar L, scalar R>
auto operator*(L lhs, const mat3<R> &rhs) {
	return mat3(
		lhs * rhs.x.x, lhs * rhs.x.y, lhs * rhs.x.z,
		lhs * rhs.y.x, lhs * rhs.y.y, lhs * rhs.y.z,
		lhs * rhs.z.x, lhs * rhs.z.y, lhs * rhs.z.z
	);
}

// Matrix + Vector

template<scalar T>
template<scalar U>
auto mat3<T>::operator*(const vec3<U> &rhs) const {
	mat3<T> t = transpose(*this);
	return vec3(dot(t.x, rhs), dot(t.y, rhs), dot(t.z, rhs));
}

#pragma endregion

template<boolean T>
bool all(const mat3<T> &mat) {
	return all(mat.x) && all(mat.y) && all(mat.z);
}

template<boolean T>
bool any(const mat3<T> &mat) {
	return any(mat.x) || any(mat.y) || any(mat.z);
}

template<scalar T>
T determinant(const mat3<T> &mat) {
	return mat.x.x * (mat.y.y * mat.z.z - mat.z.y * mat.y.z) -
		   mat.y.x * (mat.x.y * mat.z.z - mat.z.y * mat.x.z) +
		   mat.z.x * (mat.x.y * mat.y.z - mat.y.y * mat.x.z);
}

template<floating_point T>
mat3<T> inverse(const mat3<T> &mat) {
	T det1 = +(mat.y.y * mat.z.z - mat.z.y * mat.y.z);
	T det2 = -(mat.x.y * mat.z.z - mat.z.y * mat.x.z);
	T det3 = +(mat.x.y * mat.y.z - mat.y.y * mat.x.z);

	T det = mat.x.x * det1 + mat.y.x * det2 + mat.z.x * det3;
	VOLT_DEVELOPMENT_ASSERT(det != 0, "Matrix is singular.")

	return mat3<T>(
		det1, det2, det3,
		-(mat.y.x * mat.z.z - mat.z.x * mat.y.z),
		+(mat.x.x * mat.z.z - mat.z.x * mat.x.z),
		-(mat.x.x * mat.y.z - mat.y.x * mat.x.z),
		+(mat.y.x * mat.z.y - mat.z.x * mat.y.y),
		-(mat.x.x * mat.z.y - mat.z.x * mat.x.y),
		+(mat.x.x * mat.y.y - mat.y.x * mat.x.y)
	) * (1 / det);
}

template<scalar A, scalar B, scalar Epsilon>
bool is_approx(const mat3<A> &a, const mat3<B> &b, Epsilon epsilon) {
	return is_approx(a.x, b.x, epsilon) &&
		   is_approx(a.y, b.y, epsilon) &&
		   is_approx(a.z, b.z, epsilon);
}

template<scalar T, scalar Epsilon>
bool is_orthogonal(const mat3<T> &mat, Epsilon epsilon) {
	return is_approx(dot(mat.x, mat.y), 0, epsilon) &&
		   is_approx(dot(mat.y, mat.z), 0, epsilon) &&
		   is_approx(dot(mat.z, mat.x), 0, epsilon);
}

template<scalar T, scalar Epsilon>
bool is_orthonormal(const mat3<T> &mat, Epsilon epsilon) {
	return is_orthogonal(mat, epsilon) &&
			is_normalized(mat.x, epsilon) &&
			is_normalized(mat.y, epsilon) &&
			is_normalized(mat.z, epsilon);
}

template<floating_point T>
mat3<T> orthogonalize(const mat3<T> &mat) {
	vec3<T> x = mat.x;

	vec3<T> y = mat.y
			- proj(x, mat.y);
	
	vec3<T> z = mat.z
			- proj(x, mat.z)
			- proj(y, mat.z);

	return mat3(x, y, z);
}

template<floating_point T>
mat3<T> orthonormalize(const mat3<T> &mat) {
	// auto ortho = orthogonalize(mat);

	// ortho.x = normalize(ortho.x);
	// ortho.y = normalize(ortho.y);
	// ortho.z = normalize(ortho.z);

	// return ortho;

	vec3<T> x = normalize(mat.x);

	vec3<T> y = normalize(mat.y
			- dot(x, mat.y) * x);
	
	vec3<T> z = normalize(mat.z
			- dot(x, mat.z) * x
			- dot(y, mat.z) * y);

	return mat3(x, y, z);
}

template<scalar T>
mat3<T> transpose(const mat3<T> &mat) {
	return mat3(
		mat.x.x, mat.y.x, mat.z.x,
		mat.x.y, mat.y.y, mat.z.y,
		mat.x.z, mat.y.z, mat.z.z
	);
}

}
