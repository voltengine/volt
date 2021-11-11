#include "../util/util.hpp"

namespace volt::math {

template<scalar T>
const vec4<T> vec4<T>::zero(0);

template<scalar T>
const vec4<T> vec4<T>::one(1);

template<scalar T>
const vec4<T> vec4<T>::left(-1, 0, 0, 0);

template<scalar T>
const vec4<T> vec4<T>::right(1, 0, 0, 0);

template<scalar T>
const vec4<T> vec4<T>::down(0, -1, 0, 0);

template<scalar T>
const vec4<T> vec4<T>::up(0, 1, 0, 0);

template<scalar T>
const vec4<T> vec4<T>::forward(0, 0, -1, 0);

template<scalar T>
const vec4<T> vec4<T>::backward(0, 0, 1, 0);

template<scalar T>
const vec4<T> vec4<T>::past(0, 0, 0, -1);

template<scalar T>
const vec4<T> vec4<T>::future(0, 0, 0, 1);

template<scalar T>
constexpr vec4<T>::vec4() : vec4(0) {}

template<scalar T>
constexpr vec4<T>::vec4(T all) :
		x(all), y(all), z(all), w(all) {}

template<scalar T>
constexpr vec4<T>::vec4(T x, T y, T z, T w) :
		x(x), y(y), z(z), w(w) {}

template<scalar T>
template<scalar U>
constexpr vec4<T>::vec4(const vec4<U> &other) :
		x(other.x), y(other.y), z(other.z), w(other.w) {}

template<scalar T>
template<scalar U>
constexpr vec4<T>::vec4(const vec3<U> &other, float w) :
		x(other.x), y(other.y), z(other.z), w(w) {}

template<scalar T>
template<scalar U>
constexpr vec4<T>::vec4(const vec2<U> &other, float z, float w) :
		x(other.x), y(other.y), z(z), w(w) {}

#pragma region Operators

template<scalar T>
template<scalar U>
vec4<T>::operator vec3<U>() const {
	return vec3<U>(x, y, z);
}

template<scalar T>
template<scalar U>
vec4<T>::operator vec2<U>() const {
	return vec2<U>(x, y);
}

template<scalar U>
std::ostream &operator<<(std::ostream &lhs, const vec4<U> &rhs) {
	return lhs << '[' <<
			util::to_string(rhs.x) << ", " <<
			util::to_string(rhs.y) << ", " <<
			util::to_string(rhs.z) << ", " <<
			util::to_string(rhs.w) << ']';
}

template<scalar T>
T &vec4<T>::operator[](size_t index) {
	return data[index];
}

template<scalar T>
const T &vec4<T>::operator[](size_t index) const {
	return data[index];
}

template<scalar T>
vec4<T> vec4<T>::operator-() const {
	return vec4<T>(-x, -y, -z, -w);
}

template<boolean U>
vec4<bool> operator!(const vec4<U> &vec) {
	return vec4<bool>(!vec.x, !vec.y, !vec.z, !vec.w);
}

template<scalar T>
template<scalar U>
vec4<bool> vec4<T>::operator==(const vec4<U> &rhs) const {
	return vec4<bool>(x == rhs.x, y == rhs.y, z == rhs.z, w == rhs.w);
}

template<scalar T>
template<scalar U>
vec4<bool> vec4<T>::operator!=(const vec4<U> &rhs) const {
	return vec4<bool>(x != rhs.x, y != rhs.y, z != rhs.z, w != rhs.w);
}

template<scalar T>
template<scalar U>
vec4<bool> vec4<T>::operator<=(const vec4<U> &rhs) const {
	return vec4<bool>(x <= rhs.x, y <= rhs.y, z <= rhs.z, w <= rhs.w);
}

template<scalar T>
template<scalar U>
vec4<bool> vec4<T>::operator>=(const vec4<U> &rhs) const {
	return vec4<bool>(x >= rhs.x, y >= rhs.y, z >= rhs.z, w >= rhs.w);
}

template<scalar T>
template<scalar U>
vec4<bool> vec4<T>::operator<(const vec4<U> &rhs) const {
	return vec4<bool>(x < rhs.x, y < rhs.y, z < rhs.z, w < rhs.w);
}

template<scalar T>
template<scalar U>
vec4<bool> vec4<T>::operator>(const vec4<U> &rhs) const {
	return vec4<bool>(x > rhs.x, y > rhs.y, z > rhs.z, w > rhs.w);
}

// Vector + Vector

template<scalar T>
template<scalar U, scalar Ret>
vec4<Ret> vec4<T>::operator+(const vec4<U> &rhs) const {
	return vec4<Ret>(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

template<scalar T>
template<scalar U, scalar Ret>
vec4<Ret> vec4<T>::operator-(const vec4<U> &rhs) const {
	return vec4<Ret>(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

template<scalar T>
template<scalar U, scalar Ret>
vec4<Ret> vec4<T>::operator*(const vec4<U> &rhs) const {
	return vec4<Ret>(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
}

template<scalar T>
template<scalar U, scalar Ret>
vec4<Ret> vec4<T>::operator/(const vec4<U> &rhs) const {
	return vec4<Ret>(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
}


template<scalar T>
template<scalar U>
vec4<T> &vec4<T>::operator+=(const vec4<U> &rhs) {
	return *this = *this + rhs;
}

template<scalar T>
template<scalar U>
vec4<T> &vec4<T>::operator-=(const vec4<U> &rhs) {
	return *this = *this - rhs;
}

template<scalar T>
template<scalar U>
vec4<T> &vec4<T>::operator*=(const vec4<U> &rhs) {
	return *this = *this * rhs;
}

template<scalar T>
template<scalar U>
vec4<T> &vec4<T>::operator/=(const vec4<U> &rhs) {
	return *this = *this / rhs;
}

// Vector + Scalar

template<scalar T>
template<scalar U, scalar Ret>
vec4<Ret> vec4<T>::operator*(U rhs) const {
	return vec4<Ret>(x * rhs, y * rhs, z * rhs, w * rhs);
}

template<scalar T>
template<scalar U, scalar Ret>
vec4<Ret> vec4<T>::operator/(U rhs) const {
	return vec4<Ret>(x / rhs, y / rhs, z / rhs, w / rhs);
}

template<scalar T>
template<scalar U>
vec4<T> &vec4<T>::operator*=(U rhs) {
	return *this = *this * rhs;
}

template<scalar T>
template<scalar U>
vec4<T> &vec4<T>::operator/=(U rhs) {
	return *this = *this / rhs;
}

// Scalar + Vector

template<scalar L, scalar R, scalar Ret = std::common_type_t<L, R>>
vec4<Ret> operator*(L lhs, const vec4<R> &rhs) {
	return vec4<Ret>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
}

#pragma endregion

template<boolean T>
bool all(const vec4<T> &vec) {
	return vec.x && vec.y && vec.z && vec.w;
}

template<boolean T>
bool any(const vec4<T> &vec) {
	return vec.x || vec.y || vec.z || vec.w;
}

template<scalar A, scalar B, floating_point Ret>
Ret distance(const vec4<A> &a, const vec4<B> &b) {
	return length(a - b);
}

template<scalar A, scalar B, scalar Ret>
Ret dot(const vec4<A> &a, const vec4<B> &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template<scalar T, scalar Epsilon>
bool is_normalized(const vec4<T> &vec, Epsilon epsilon) {
	return is_approx(dot(vec, vec), 1, epsilon);
}

template<floating_point T>
T length(const vec4<T> &vec) {
	return sqrt(dot(vec, vec));
}

template<floating_point T>
vec4<T> normalize(const vec4<T> &vec) {
	return vec * (1 / length(vec));
}

template<floating_point To, floating_point From, scalar Ret>
vec4<Ret> proj(const vec4<To> &to, const vec4<From> &from) {
	return (dot(to, from) / dot(to, to)) * to;
}

#pragma region Component-Wise Math Wrappers

template<floating_point X>
vec4<X> fract(const vec4<X> &x) {
	return vec4<X>(fract(x.x), fract(x.y), fract(x.z), fract(x.w));
}

template<scalar From, scalar To, scalar Weight, scalar Ret>
vec4<Ret> lerp(const vec4<From> &from, const vec4<To> &to, Weight weight) {
	return vec4<Ret>(
			lerp(from.x, to.x, weight),
			lerp(from.y, to.y, weight),
			lerp(from.z, to.z, weight),
			lerp(from.w, to.w, weight));
}

template<scalar A, scalar B, scalar Ret>
vec4<Ret> max(const vec4<A> &a, const vec4<B> &b) {
	return vec4<Ret>(
			max(a.x, b.x),
			max(a.y, b.y),
			max(a.z, b.z),
			max(a.w, b.w));
}

template<scalar A, scalar B, scalar... Others, scalar Ret>
vec4<Ret> max(const vec4<A> &a, const vec4<B> &b,
		const vec4<Others> &...others) {
	return max(max(a, b), others...);
}

template<scalar A, scalar B, scalar Ret>
vec4<Ret> min(const vec4<A> &a, const vec4<B> &b) {
	return vec4<Ret>(
			min(a.x, b.x),
			min(a.y, b.y),
			min(a.z, b.z),
			min(a.w, b.w));
}

template<scalar A, scalar B, scalar... Others, scalar Ret>
vec4<Ret> min(const vec4<A> &a, const vec4<B> &b,
		const vec4<Others> &...others) {
	return min(min(a, b), others...);
}

template<scalar X, scalar Y, scalar Ret>
vec4<Ret> mod(const vec4<X> &x, const vec4<Y> &y) {
	return vec4<Ret>(
			mod(x.x, y.x),
			mod(x.y, y.y),
			mod(x.z, y.z),
			mod(x.w, y.w));
}

#pragma endregion

}
