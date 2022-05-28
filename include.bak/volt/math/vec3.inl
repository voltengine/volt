#include "../util/util.hpp"

namespace volt::math {

template<scalar T>
const vec3<T> vec3<T>::zero(0);

template<scalar T>
const vec3<T> vec3<T>::one(1);

template<scalar T>
const vec3<T> vec3<T>::left(-1, 0, 0);

template<scalar T>
const vec3<T> vec3<T>::right(1, 0, 0);

template<scalar T>
const vec3<T> vec3<T>::down(0, -1, 0);

template<scalar T>
const vec3<T> vec3<T>::up(0, 1, 0);

template<scalar T>
const vec3<T> vec3<T>::forward(0, 0, -1);

template<scalar T>
const vec3<T> vec3<T>::backward(0, 0, 1);

template<scalar T>
constexpr vec3<T>::vec3() : vec3(0) {}

template<scalar T>
constexpr vec3<T>::vec3(T all) :
		x(all), y(all), z(all) {}

template<scalar T>
constexpr vec3<T>::vec3(T x, T y, T z) :
		x(x), y(y), z(z) {}

template<scalar T>
template<scalar U>
constexpr vec3<T>::vec3(const vec3<U> &other) :
		x(other.x), y(other.y), z(other.z) {}

template<scalar T>
template<scalar U>
constexpr vec3<T>::vec3(const vec2<U> &other, float z) :
		x(other.x), y(other.y), z(z) {}

#pragma region Operators

template<scalar T>
template<scalar U>
vec3<T>::operator vec2<U>() const {
	return vec2<U>(x, y);
}

template<scalar U>
std::ostream &operator<<(std::ostream &lhs, const vec3<U> &rhs) {
	return lhs << '[' <<
			util::to_string(rhs.x) << ", " <<
			util::to_string(rhs.y) << ", " <<
			util::to_string(rhs.z) << ']';
}

template<scalar T>
T &vec3<T>::operator[](size_t index) {
	return data[index];
}

template<scalar T>
const T &vec3<T>::operator[](size_t index) const {
	return data[index];
}

template<scalar T>
vec3<T> vec3<T>::operator-() const {
	return vec3<T>(-x, -y, -z);
}

template<boolean U>
vec3<bool> operator!(const vec3<U> &vec) {
	return vec3<bool>(!vec.x, !vec.y, !vec.z);
}

template<scalar T>
template<scalar U>
vec3<bool> vec3<T>::operator==(const vec3<U> &rhs) const {
	return vec3<bool>(x == rhs.x, y == rhs.y, z == rhs.z);
}

template<scalar T>
template<scalar U>
vec3<bool> vec3<T>::operator!=(const vec3<U> &rhs) const {
	return vec3<bool>(x != rhs.x, y != rhs.y, z != rhs.z);
}

template<scalar T>
template<scalar U>
vec3<bool> vec3<T>::operator<=(const vec3<U> &rhs) const {
	return vec3<bool>(x <= rhs.x, y <= rhs.y, z <= rhs.z);
}

template<scalar T>
template<scalar U>
vec3<bool> vec3<T>::operator>=(const vec3<U> &rhs) const {
	return vec3<bool>(x >= rhs.x, y >= rhs.y, z >= rhs.z);
}

template<scalar T>
template<scalar U>
vec3<bool> vec3<T>::operator<(const vec3<U> &rhs) const {
	return vec3<bool>(x < rhs.x, y < rhs.y, z < rhs.z);
}

template<scalar T>
template<scalar U>
vec3<bool> vec3<T>::operator>(const vec3<U> &rhs) const {
	return vec3<bool>(x > rhs.x, y > rhs.y, z > rhs.z);
}

// Vector + Vector

template<scalar T>
template<scalar U, scalar Ret>
vec3<Ret> vec3<T>::operator+(const vec3<U> &rhs) const {
	return vec3<Ret>(x + rhs.x, y + rhs.y, z + rhs.z);
}

template<scalar T>
template<scalar U, scalar Ret>
vec3<Ret> vec3<T>::operator-(const vec3<U> &rhs) const {
	return vec3<Ret>(x - rhs.x, y - rhs.y, z - rhs.z);
}

template<scalar T>
template<scalar U, scalar Ret>
vec3<Ret> vec3<T>::operator*(const vec3<U> &rhs) const {
	return vec3<Ret>(x * rhs.x, y * rhs.y, z * rhs.z);
}

template<scalar T>
template<scalar U, scalar Ret>
vec3<Ret> vec3<T>::operator/(const vec3<U> &rhs) const {
	return vec3<Ret>(x / rhs.x, y / rhs.y, z / rhs.z);
}

template<scalar T>
template<scalar U>
vec3<T> &vec3<T>::operator+=(const vec3<U> &rhs) {
	return *this = *this + rhs;
}

template<scalar T>
template<scalar U>
vec3<T> &vec3<T>::operator-=(const vec3<U> &rhs) {
	return *this = *this - rhs;
}

template<scalar T>
template<scalar U>
vec3<T> &vec3<T>::operator*=(const vec3<U> &rhs) {
	return *this = *this * rhs;
}

template<scalar T>
template<scalar U>
vec3<T> &vec3<T>::operator/=(const vec3<U> &rhs) {
	return *this = *this / rhs;
}

// Vector + Scalar

template<scalar T>
template<scalar U, scalar Ret>
vec3<Ret> vec3<T>::operator*(U rhs) const {
	return vec3<Ret>(x * rhs, y * rhs, z * rhs);
}

template<scalar T>
template<scalar U, scalar Ret>
vec3<Ret> vec3<T>::operator/(U rhs) const {
	return vec3<Ret>(x / rhs, y / rhs, z / rhs);
}

template<scalar T>
template<scalar U>
vec3<T> &vec3<T>::operator*=(U rhs) {
	return *this = *this * rhs;
}

template<scalar T>
template<scalar U>
vec3<T> &vec3<T>::operator/=(U rhs) {
	return *this = *this / rhs;
}

// Scalar + Vector

template<scalar L, scalar R, scalar Ret = std::common_type_t<L, R>>
vec3<Ret> operator*(L lhs, const vec3<R> &rhs) {
	return vec3<Ret>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

#pragma endregion

template<boolean T>
bool all(const vec3<T> &vec) {
	return vec.x && vec.y && vec.z;
}

template<boolean T>
bool any(const vec3<T> &vec) {
	return vec.x || vec.y || vec.z;
}

template<scalar L, scalar R, scalar Ret>
vec3<Ret> cross(const vec3<L> &lhs, const vec3<R> &rhs) {
	return vec3<Ret>(
		(lhs.y * rhs.z) - (lhs.z * rhs.y),
		(lhs.z * rhs.x) - (lhs.x * rhs.z),
		(lhs.x * rhs.y) - (lhs.y * rhs.x)
	);
}

template<scalar A, scalar B, floating_point Ret>
Ret distance(const vec3<A> &a, const vec3<B> &b) {
	return length(a - b);
}

template<scalar A, scalar B, scalar Ret>
Ret dot(const vec3<A> &a, const vec3<B> &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<scalar T, scalar Epsilon>
bool is_normalized(const vec3<T> &vec, Epsilon epsilon) {
	return is_approx(dot(vec, vec), 1, epsilon);
}

template<floating_point T>
T length(const vec3<T> &vec) {
	return sqrt(dot(vec, vec));
}

template<floating_point T>
vec3<T> normalize(const vec3<T> &vec) {
	return vec * (1 / length(vec));
}

template<floating_point To, floating_point From, scalar Ret>
vec3<Ret> proj(const vec3<To> &to, const vec3<From> &from) {
	return (dot(to, from) / dot(to, to)) * to;
}

template<scalar Incident, scalar Normal, scalar Ret>
vec3<Ret> reflect(const vec3<Incident> &incident,
		const vec3<Normal> &normal) {
	return incident - 2 * dot(normal, incident) * normal;
}

template<scalar Incident, scalar Normal,
		scalar IorRatio, scalar Ret>
vec3<Ret> refract(const vec3<Incident> &incident,
		const vec3<Normal> &normal, IorRatio ior_ratio) {
	Ret cos_theta = dot(incident, normal);
	Ret k = 1 - ior_ratio * ior_ratio * (1 - cos_theta * cos_theta);
	if (k < 0)
		return vec3<Ret>::zero;
	
	return ior_ratio * incident -
		(ior_ratio * cos_theta + sqrt(k)) * normal;
}

#pragma region Component-Wise Math Wrappers

template<scalar X>
vec3<X> abs(const vec3<X> &x) {
	return vec3<X>(abs(x.x), abs(x.y), abs(x.z));
}

template<floating_point X>
vec3<X> fract(const vec3<X> &x) {
	return vec3<X>(fract(x.x), fract(x.y), fract(x.z));
}

template<scalar From, scalar To, scalar Weight, scalar Ret>
vec3<Ret> lerp(const vec3<From> &from, const vec3<To> &to, const vec3<Weight> &weight) {
	return vec3<Ret>(
			lerp(from.x, to.x, weight.x),
			lerp(from.y, to.y, weight.y),
			lerp(from.z, to.z, weight.z));
}

template<scalar A, scalar B, scalar Ret>
vec3<Ret> max(const vec3<A> &a, const vec3<B> &b) {
	return vec3<Ret>(
			max(a.x, b.x),
			max(a.y, b.y),
			max(a.z, b.z));
}

template<scalar A, scalar B, scalar... Others, scalar Ret>
vec3<Ret> max(const vec3<A> &a, const vec3<B> &b,
		const vec3<Others> &...others) {
	return max(max(a, b), others...);
}

template<scalar A, scalar B, scalar Ret>
vec3<Ret> min(const vec3<A> &a, const vec3<B> &b) {
	return vec3<Ret>(
			min(a.x, b.x),
			min(a.y, b.y),
			min(a.z, b.z));
}

template<scalar A, scalar B, scalar... Others, scalar Ret>
vec3<Ret> min(const vec3<A> &a, const vec3<B> &b,
		const vec3<Others> &...others) {
	return min(min(a, b), others...);
}

template<scalar X, scalar Y, scalar Ret>
vec3<Ret> mod(const vec3<X> &x, const vec3<Y> &y) {
	return vec3<Ret>(
			mod(x.x, y.x),
			mod(x.y, y.y),
			mod(x.z, y.z));
}

template<scalar X, scalar Power, scalar Ret>
vec3<Ret> pow(const vec3<X> &x, const vec3<Power> &power) {
	return vec3<Ret>(
			math::pow(x.x, power.x),
			math::pow(x.y, power.y),
			math::pow(x.z, power.z));
}

template<scalar X>
vec3<X> saturate(const vec3<X> &x) {
	return vec3<X>(
			saturate(x.x),
			saturate(x.y),
			saturate(x.z));
}

#pragma endregion

}

namespace std {

template<volt::math::scalar T>
std::size_t hash<volt::math::vec3<T>>::operator()(volt::math::vec3<T> vec) const {
	return (static_cast<size_t>(vec.x) * 859433 ^
			static_cast<size_t>(vec.y)) * 19937 ^
			static_cast<size_t>(vec.z);
}

}
