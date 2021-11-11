namespace volt::math {

// Basic

template<scalar X>
inline X sqrt(X x) {
	return std::sqrt(x);
}

template<scalar X, scalar Power, scalar Ret>
inline Ret pow(X x, Power power) {
	return std::pow(x, power);
}

template<scalar Base, scalar X, scalar Ret>
inline Ret log(Base base, X x) {
	return std::log(x) / std::log(base);
}

template<scalar X>
inline X log(X x) {
	return std::log(x);
}

template<scalar X>
inline X log2(X x) {
	return std::log2(x);
}

template<scalar Base, scalar X, scalar Ret>
inline Ret exp(Base base, X x) {
	return std::pow(base, x);
}

template<scalar X>
inline X exp(X x) {
	return std::exp(x);
}

template<scalar X>
inline X exp2(X x) {
	return std::exp2(x);
}

template<scalar X>
inline X abs(X x) {
	return std::abs(x);
}

template<scalar A, scalar B, scalar Epsilon>
bool is_approx(A a, B b, Epsilon epsilon) {
	return a == b || math::abs(a - b) < epsilon;
}

// Trigonometry

template<scalar Angle>
inline Angle sin(Angle angle) {
	return std::sin(angle);
}

template<scalar Angle>
inline Angle cos(Angle angle) {
	return std::cos(angle);
}

template<scalar Angle>
inline Angle tan(Angle angle) {
	return std::tan(angle);
}

template<scalar Y>
inline Y asin(Y y) {
	return std::asin(y);
}

template<scalar X>
inline X acos(X x) {
	return std::acos(x);
}

template<scalar YOverX>
inline YOverX atan(YOverX y_over_x) {
	return std::atan(y_over_x);
}

template<scalar Y, scalar X, scalar Ret>
inline Ret atan2(Y y, X x) {
	return std::atan2(y, x);
}

// Hyperbolic

template<scalar DoubleArea>
inline DoubleArea sinh(DoubleArea double_area) {
	return std::sinh(double_area);
}

template<scalar DoubleArea>
inline DoubleArea cosh(DoubleArea double_area) {
	return std::cosh(double_area);
}

template<scalar DoubleArea>
inline DoubleArea tanh(DoubleArea double_area) {
	return std::tanh(double_area);
}

template<scalar Y>
inline Y asinh(Y y) {
	return std::asinh(y);
}

template<scalar X>
inline X acosh(X x) {
	return std::acosh(x);
}

template<scalar YOverX>
inline YOverX atanh(YOverX y_over_x) {
	return std::atanh(y_over_x);
}

// Rounding

template<floating_point X>
inline X floor(X x) {
	return std::floor(x);
}

template<floating_point X>
inline X ceil(X x) {
	return std::ceil(x);
}

template<floating_point X>
inline X round(X x) {
	return std::round(x);
}

// Conversion

template<scalar Degrees>
inline Degrees radians(Degrees degrees) {
	return degrees * (math::pi / 180);
}

template<scalar Radians>
inline Radians degrees(Radians radians) {
	return radians * (180 / math::pi);
}

// Miscellaneous

template<scalar X, scalar Min, scalar Max, scalar Ret>
inline Ret clamp(X x, Min min, Max max) {
	return math::min(math::max(x, min), max);
}

template<floating_point X>
inline X fract(X x) {
	return x - floor(x);
}

template<scalar A, scalar B, scalar Weight, scalar Ret>
inline Ret lerp(A a, B b, Weight weight) {
	return a + (b - a) * weight;
}

template<scalar A, scalar B, scalar Ret>
inline Ret max(A a, B b) {
	return b > a ? b : a;
}

template<scalar A, scalar B, scalar... Others, scalar Ret>
inline Ret max(A a, B b, Others ...others) {
	return max(max(a, b), others...);
}

template<scalar A, scalar B, scalar Ret>
inline Ret min(A a, B b) {
	return b < a ? b : a;
}

template<scalar A, scalar B, scalar... Others, scalar Ret>
inline Ret min(A a, B b, Others ...others) {
	return min(min(a, b), others...);
}

template<scalar X, scalar Y, scalar Ret>
inline Ret mod(X x, Y y) {
	if constexpr (std::is_floating_point_v<Ret>)
		return x - y * floor(x / y);
	else
		return (y + (x % y)) % y;
}

template<scalar X>
inline X saturate(X x) {
	return clamp(x, 0, 1);
}

template<scalar X>
inline int32_t sign(X x) {
	return (0 < x) - (x < 0);
}

template<scalar From, scalar To, scalar X, scalar Ret>
inline Ret smoothstep(From from, To to, X x) {
	Ret t = saturate((x - from) / (to - from));
	return t * t * (3 - 2 * t);
}

template<scalar Edge, scalar X>
inline int32_t step(Edge edge, X x) {
	return x >= edge ? 1 : 0;
}

}
