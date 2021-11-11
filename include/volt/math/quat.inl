#include "../util/util.hpp"

namespace volt::math {


template<scalar T>
const quat<T> quat<T>::identity(1);

template<scalar T>
quat<T>::quat() : quat(0) {}

template<scalar T>
template<scalar U>
quat<T>::quat(T w, const vec3<U> &v)
		: w(w), v(v) {}

template<scalar T>
quat<T>::quat(T w, T x, T y, T z)
		: w(w), v(x, y, z) {}

template<scalar T>
template<scalar U>
quat<T> quat<T>::axis_angle(T angle, const vec3<U> &axis) {
	T half_angle = angle * 0.5F;

	return quat<T>(
		cos(half_angle),
		sin(half_angle) * normalize(static_cast<vec3<T>>(axis))
	);
}

template<scalar T>
template<scalar U>
quat<T> quat<T>::from_basis(const mat3<U> &basis) {
	assert(is_orthonormal(basis) && "Basis must be orthonormal.");

	vec3<T> x = basis.x, y = basis.y, z = basis.z;

	T t;
	quat<T> q;

	if (z.z < 0) {
		if (x.x > y.y) {
			t = 1 + x.x - y.y - z.z;
			q = quat<T>(y.z - z.y, t, x.y + y.x, z.x + x.z);
		} else {
			t = 1 - x.x + y.y - z.z;
			q = quat<T>(z.x - x.z, x.y + y.x, t, y.z + z.y);
		}
	} else {
		if (x.x < -y.y) {
			t = 1 - x.x - y.y + z.z;
			q = quat<T>(x.y - y.x, z.x + x.z, y.z + z.y, t);
		} else {
			t = 1 + x.x + y.y + z.z;
			q = quat<T>(t, y.z - z.y, z.x - x.z, x.y - y.x);
		}
	}

	return q * (0.5F / sqrt(t));
}

template<scalar T>
template<scalar U>
quat<T> quat<T>::from_euler(const vec3<U> &euler) {
	vec3<T> half_euler = euler * 0.5F;

	T sp = sin(half_euler.x);
	T sy = sin(half_euler.y);
	T sr = sin(half_euler.z);

	T cp = cos(half_euler.x);
	T cy = cos(half_euler.y);
	T cr = cos(half_euler.z);

	return quat<T>(
		cr * cp * cy + sr * sp * sy,
		cr * sp * cy + sr * cp * sy,
		cr * cp * sy - sr * sp * cy,
		sr * cp * cy - cr * sp * sy
	);
}

template<scalar T>
vec3<T> quat<T>::to_euler() const {
	T xx = v.x * v.x;

	T pitch_y = 2 * (w * v.x - v.y * v.z);
	pitch_y = clamp(pitch_y, -1, 1);
	T pitch = asin(pitch_y);

	T yaw_y = 2 * (w * v.y + v.z * v.x);
	T yaw_x = 1 - 2 * (v.y * v.y + xx);
	T yaw = atan2(yaw_y, yaw_x);

	T roll_y = 2 * (w * v.z + v.x * v.y);
	T roll_x = 1 - 2 * (v.z * v.z + xx);
	T roll = atan2(roll_y, roll_x);

	return vec3<T>(pitch, yaw, roll);
}

template<scalar T>
mat3<T> quat<T>::to_basis() const {
	// return mat3<T>(
	// 	*this * vec3<T>::RIGHT,
	// 	*this * vec3<T>::UP,
	// 	*this * vec3<T>::BACKWARD
	// );

	return mat3<T>(
		1 - 2 * (v.y * v.y + v.z * v.z),
		2 * (v.x * v.y + v.z * w),
		2 * (v.x * v.z - v.y * w),
		2 * (v.x * v.y - v.z * w),
		1 - 2 * (v.x * v.x + v.z * v.z),
		2 * (v.y * v.z + v.x * w),
		2 * (v.x * v.z + v.y * w),
		2 * (v.y * v.z - v.x * w),
		1 - 2 * (v.x * v.x + v.y * v.y)
	);
}

#pragma region Operators

template<scalar U>
std::ostream &operator<<(std::ostream &lhs, const quat<U> &rhs) {
	bool first_var = true;

	for (int32_t i = 0; i < 4; i++) {
		U var;
		if (i == 0)
			var = rhs.w;
		else
			var = rhs.v[i - 1];

		if (var != 0) {
			if (var > 0) {
				if (!first_var)
					lhs << " + ";
			} else {
				if (!first_var)
					lhs << " - ";
				else
					lhs << '-';
			}

			var = abs(var);
			if (i == 0 || !is_approx(var, 1))
				lhs << util::to_string(var);

			if (i != 0)
				lhs << static_cast<char>('i' + (i - 1));
			
			first_var = false;
		}

	}

	if (first_var)
		lhs << '0';

	return lhs;
}

template<scalar T>
template<scalar U>
bool quat<T>::operator==(const quat<U> &rhs) const {
	return w == rhs.w && all(v == rhs.v);
}

template<scalar T>
template<scalar U>
bool quat<T>::operator!=(const quat<U> &rhs) const {
	return w != rhs.w || any(v != rhs.v);
}

template<scalar T>
quat<T> &quat<T>::operator++() {
	w++;
	return *this;
}

template<scalar T>
quat<T> &quat<T>::operator--() {
	w--;
	return *this;
}

template<scalar T>
quat<T> quat<T>::operator++(int32_t) {
	quat<T> q = *this;
	++(*this);
	return q;
}

template<scalar T>
quat<T> quat<T>::operator--(int32_t) {
	quat<T> q = *this;
	--(*this);
	return q;
}

template<scalar T>
quat<T> quat<T>::operator-() const {
	return quat<T>(-w, -v);
}

// Quaternion + Quaternion

template<scalar T>
template<scalar U, scalar Ret>
quat<Ret> quat<T>::operator+(const quat<U> &rhs) const {
	return quat<Ret>(w + rhs.w, v + rhs.v);
}

template<scalar T>
template<scalar U, scalar Ret>
quat<Ret> quat<T>::operator-(const quat<U> &rhs) const {
	return quat<Ret>(w - rhs.w, v - rhs.v);
}

template<scalar T>
template<scalar U, scalar Ret>
quat<Ret> quat<T>::operator*(const quat<U> &rhs) const {
	return quat<Ret>(
		w *   rhs.w - v.x * rhs.v.x - v.y * rhs.v.y - v.z * rhs.v.z,
		w * rhs.v.x + v.x *   rhs.w + v.y * rhs.v.z - v.z * rhs.v.y,
		w * rhs.v.y - v.x * rhs.v.z + v.y *   rhs.w + v.z * rhs.v.x,
		w * rhs.v.z + v.x * rhs.v.y - v.y * rhs.v.x + v.z *   rhs.w
	);
}

template<scalar T>
template<scalar U, scalar Ret>
quat<Ret> quat<T>::operator/(const quat<U> &rhs) const {
	return *this * inverse<Ret>(rhs);
}

template<scalar T>
template<scalar U>
quat<T> &quat<T>::operator+=(const quat<U> &rhs) {
	return *this = *this + rhs;
}

template<scalar T>
template<scalar U>
quat<T> &quat<T>::operator-=(const quat<U> &rhs) {
	return *this = *this - rhs;
}

template<scalar T>
template<scalar U>
quat<T> &quat<T>::operator*=(const quat<U> &rhs) {
	return *this = *this * rhs;
}

template<scalar T>
template<scalar U>
quat<T> &quat<T>::operator/=(const quat<U> &rhs) {
	return *this = *this / rhs;
}

// Quaternion + Scalar

template<scalar T>
template<scalar U, scalar Ret>
quat<Ret> quat<T>::operator+(U rhs) const {
	return quat<Ret>(w + rhs, v);
}

template<scalar T>
template<scalar U, scalar Ret>
quat<Ret> quat<T>::operator-(U rhs) const {
	return quat<Ret>(w - rhs, v);
}

template<scalar T>
template<scalar U, scalar Ret>
quat<Ret> quat<T>::operator*(U rhs) const {
	return quat<Ret>(w * rhs, v * rhs);
}

template<scalar T>
template<scalar U, scalar Ret>
quat<Ret> quat<T>::operator/(U rhs) const {
	return quat<Ret>(w / rhs, v / rhs);
}

template<scalar T>
template<scalar U>
quat<T> &quat<T>::operator+=(U rhs) {
	return *this = *this + rhs;
}

template<scalar T>
template<scalar U>
quat<T> &quat<T>::operator-=(U rhs) {
	return *this = *this - rhs;
}

template<scalar T>
template<scalar U>
quat<T> &quat<T>::operator*=(U rhs) {
	return *this = *this * rhs;
}

template<scalar T>
template<scalar U>
quat<T> &quat<T>::operator/=(U rhs) {
	return *this = *this / rhs;
}

// Scalar + Quaternion

template<scalar L, scalar R, scalar Ret = std::common_type_t<L, R>>
quat<Ret> operator+(L lhs, const quat<R> &rhs) {
	return quat<Ret>(lhs + rhs.w, rhs.v);
}

template<scalar L, scalar R, scalar Ret = std::common_type_t<L, R>>
quat<Ret> operator-(L lhs, const quat<R> &rhs) {
	return quat<Ret>(lhs - rhs.w, -rhs.v);
}

template<scalar L, scalar R, scalar Ret = std::common_type_t<L, R>>
quat<Ret> operator*(L lhs, const quat<R> &rhs) {
	return quat<Ret>(lhs * rhs.w, lhs * rhs.v);
}

template<scalar L, scalar R, scalar Ret = std::common_type_t<L, R>>
quat<Ret> operator/(L lhs, const quat<R> &rhs) {
	return lhs * inverse(static_cast<quat<Ret>>(rhs));
}

// Quaternion + Vector

template<scalar T>
template<scalar U, scalar Ret>
vec3<Ret> quat<T>::operator*(const vec3<U> &rhs) const {
	// return (*this * quat<Ret>(0, rhs) *
	//		inverse(static_cast<quat<Ret>>(*this))).v;
	
	vec3<Ret> c = cross(v, rhs);
	return rhs + c * (2 * w) + cross(v, c) * 2;
}

#pragma endregion

template<scalar From, scalar To, floating_point Ret>
Ret angle(const quat<From> &from, const quat<To> &to) {
	return acos(dot(normalize(static_cast<quat<Ret>>(from)),
			normalize(static_cast<quat<Ret>>(to))));
}

template<scalar T>
quat<T> conjugate(const quat<T> &quat) {
	return math::quat(quat.w, -quat.v);
}

template<scalar A, scalar B, scalar Ret>
Ret dot(const quat<A> &lhs, const quat<B> &rhs) {
	return lhs.w * rhs.w + dot(lhs.v, rhs.v);
}

template<floating_point T>
quat<T> inverse(const quat<T> &quat) {
	return conjugate(quat) / dot(quat, quat);
}

template<scalar A, scalar B, scalar Epsilon>
bool is_approx(const quat<A> &a, const quat<B> &b, Epsilon epsilon) {
	return is_approx(a.w, b.w, epsilon) &&
		   is_approx(a.v, b.v, epsilon);
}

template<scalar T, scalar Epsilon>
bool is_normalized(const quat<T> &quat, Epsilon epsilon) {
	return is_approx(dot(quat, quat), 1, epsilon);
}

template<floating_point T>
T length(const quat<T> &quat) {
	return sqrt(dot(quat, quat));
}

template<floating_point T>
quat<T> normalize(const quat<T> &quat) {
	return quat * (1 / length(quat));
}

template<scalar From, scalar To, scalar Weight, floating_point Ret>
quat<Ret> slerp(const quat<From> &from, const quat<To> &to, Weight weight) {
	// quat<T> q = to / from;
	// T angle = atan2(length(q.v), q.w) * 2;
	// return from * axis_angle(angle * weight, q.v);

	quat<Ret> q = static_cast<quat<Ret>>(to) / from;

	Ret c = q.w;
	Ret s = length(q.v);

	Ret half_angle = atan2(s, c);
	Ret inv_sine = 1 / s;

	Ret k_from = inv_sine * sin(half_angle * (1 - weight));
	Ret k_to   = inv_sine * sin(half_angle * weight);

	return (from * k_from) + (to * k_to);
}

}
