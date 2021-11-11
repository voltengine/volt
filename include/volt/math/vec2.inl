#include "../util/util.hpp"

namespace volt::math {

template<scalar T>
const vec2<T> vec2<T>::zero(0);

template<scalar T>
const vec2<T> vec2<T>::one(1);

template<scalar T>
const vec2<T> vec2<T>::left(-1, 0);

template<scalar T>
const vec2<T> vec2<T>::right(1, 0);

template<scalar T>
const vec2<T> vec2<T>::down(0, -1);

template<scalar T>
const vec2<T> vec2<T>::up(0, 1);

template<scalar T>
constexpr vec2<T>::vec2() : vec2(0) {}

template<scalar T>
constexpr vec2<T>::vec2(T all) :
		x(all), y(all) {}

template<scalar T>
constexpr vec2<T>::vec2(T x, T y) :
		x(x), y(y) {}

template<scalar T>
template<scalar U>
constexpr vec2<T>::vec2(const vec2<U> &other) :
		x(other.x), y(other.y) {}

#pragma region Operators

template<scalar U>
std::ostream &operator<<(std::ostream &lhs, const vec2<U> &rhs) {
	return lhs << '[' <<
			util::to_string(rhs.x) << ", " <<
			util::to_string(rhs.y) << ']';
}

template<scalar T>
T &vec2<T>::operator[](size_t index) {
	return data[index];
}

template<scalar T>
const T &vec2<T>::operator[](size_t index) const {
	return data[index];
}

template<scalar T>
vec2<T> vec2<T>::operator-() const {
	return vec2<T>(-x, -y);
}

template<boolean U>
vec2<bool> operator!(const vec2<U> &vec) {
	return vec2<bool>(!vec.x, !vec.y);
}

template<scalar T>
template<scalar U>
vec2<bool> vec2<T>::operator==(const vec2<U> &rhs) const {
	return vec2<bool>(x == rhs.x, y == rhs.y);
}

template<scalar T>
template<scalar U>
vec2<bool> vec2<T>::operator!=(const vec2<U> &rhs) const {
	return vec2<bool>(x != rhs.x, y != rhs.y);
}

template<scalar T>
template<scalar U>
vec2<bool> vec2<T>::operator<=(const vec2<U> &rhs) const {
	return vec2<bool>(x <= rhs.x, y <= rhs.y);
}

template<scalar T>
template<scalar U>
vec2<bool> vec2<T>::operator>=(const vec2<U> &rhs) const {
	return vec2<bool>(x >= rhs.x, y >= rhs.y);
}

template<scalar T>
template<scalar U>
vec2<bool> vec2<T>::operator<(const vec2<U> &rhs) const {
	return vec2<bool>(x < rhs.x, y < rhs.y);
}

template<scalar T>
template<scalar U>
vec2<bool> vec2<T>::operator>(const vec2<U> &rhs) const {
	return vec2<bool>(x > rhs.x, y > rhs.y);
}

// Vector + Vector

template<scalar T>
template<scalar U, scalar Ret>
vec2<Ret> vec2<T>::operator+(const vec2<U> &rhs) const {
	return vec2<Ret>(x + rhs.x, y + rhs.y);
}

template<scalar T>
template<scalar U, scalar Ret>
vec2<Ret> vec2<T>::operator-(const vec2<U> &rhs) const {
	return vec2<Ret>(x - rhs.x, y - rhs.y);
}

template<scalar T>
template<scalar U, scalar Ret>
vec2<Ret> vec2<T>::operator*(const vec2<U> &rhs) const {
	return vec2<Ret>(x * rhs.x, y * rhs.y);
}

template<scalar T>
template<scalar U, scalar Ret>
vec2<Ret> vec2<T>::operator/(const vec2<U> &rhs) const {
	return vec2<Ret>(x / rhs.x, y / rhs.y);
}

template<scalar T>
template<scalar U>
vec2<T> &vec2<T>::operator+=(const vec2<U> &rhs) {
	return *this = *this + rhs;
}

template<scalar T>
template<scalar U>
vec2<T> &vec2<T>::operator-=(const vec2<U> &rhs) {
	return *this = *this - rhs;
}

template<scalar T>
template<scalar U>
vec2<T> &vec2<T>::operator*=(const vec2<U> &rhs) {
	return *this = *this * rhs;
}

template<scalar T>
template<scalar U>
vec2<T> &vec2<T>::operator/=(const vec2<U> &rhs) {
	return *this = *this / rhs;
}

// Vector + Scalar

template<scalar T>
template<scalar U, scalar Ret>
vec2<Ret> vec2<T>::operator*(U rhs) const {
	return vec2<Ret>(x * rhs, y * rhs);
}

template<scalar T>
template<scalar U, scalar Ret>
vec2<Ret> vec2<T>::operator/(U rhs) const {
	return vec2<Ret>(x / rhs, y / rhs);
}

template<scalar T>
template<scalar U>
vec2<T> &vec2<T>::operator*=(U rhs) {
	return *this = *this * rhs;
}

template<scalar T>
template<scalar U>
vec2<T> &vec2<T>::operator/=(U rhs) {
	return *this = *this / rhs;
}

// Scalar + Vector

template<scalar L, scalar R, scalar Ret = std::common_type_t<L, R>>
vec2<Ret> operator*(L lhs, const vec2<R> &rhs) {
	return vec2<Ret>(lhs * rhs.x, lhs * rhs.y);
}

#pragma endregion

template<boolean T>
bool all(const vec2<T> &vec) {
	return vec.x && vec.y;
}

template<boolean T>
bool any(const vec2<T> &vec) {
	return vec.x || vec.y;
}

template<scalar A, scalar B, floating_point Ret>
Ret distance(const vec2<A> &a, const vec2<B> &b) {
	return length(a - b);
}

template<scalar A, scalar B, scalar Ret>
Ret dot(const vec2<A> &a, const vec2<B> &b) {
	return a.x * b.x + a.y * b.y;
}

template<scalar T, scalar Epsilon>
bool is_normalized(const vec2<T> &vec, Epsilon epsilon) {
	return is_approx(dot(vec, vec), 1, epsilon);
}

template<floating_point T>
T length(const vec2<T> &vec) {
	return sqrt(dot(vec, vec));
}

template<floating_point T>
vec2<T> normalize(const vec2<T> &vec) {
	return vec * (1 / length(vec));
}

template<floating_point To, floating_point From, scalar Ret>
vec2<Ret> proj(const vec2<To> &to, const vec2<From> &from) {
	return (dot(to, from) / dot(to, to)) * to;
}

#pragma region Component-Wise Math Wrappers

template<floating_point X>
vec2<X> fract(const vec2<X> &x) {
	return vec2<X>(fract(x.x), fract(x.y));
}

template<scalar From, scalar To, scalar Weight, scalar Ret>
vec2<Ret> lerp(const vec2<From> &from, const vec2<To> &to, Weight weight) {
	return vec2<Ret>(
			lerp(from.x, to.x, weight),
			lerp(from.y, to.y, weight));
}

template<scalar A, scalar B, scalar Ret>
vec2<Ret> max(const vec2<A> &a, const vec2<B> &b) {
	return vec2<Ret>(
			max(a.x, b.x),
			max(a.y, b.y));
}

template<scalar A, scalar B, scalar... Others, scalar Ret>
vec2<Ret> max(const vec2<A> &a, const vec2<B> &b,
		const vec2<Others> &...others) {
	return max(max(a, b), others...);
}

template<scalar A, scalar B, scalar Ret>
vec2<Ret> min(const vec2<A> &a, const vec2<B> &b) {
	return vec2<Ret>(
			min(a.x, b.x),
			min(a.y, b.y));
}

template<scalar A, scalar B, scalar... Others, scalar Ret>
vec2<Ret> min(const vec2<A> &a, const vec2<B> &b,
		const vec2<Others> &...others) {
	return min(min(a, b), others...);
}

template<scalar X, scalar Y, scalar Ret>
vec2<Ret> mod(const vec2<X> &x, const vec2<Y> &y) {
	return vec2<Ret>(
			mod(x.x, y.x),
			mod(x.y, y.y));
}

#pragma endregion

}
