#pragma once

#include "../macros.hpp"

#include <numbers>

namespace volt::math {

template <typename T>
concept boolean = std::is_same_v<T, bool>;

template <typename T>
concept floating_point = std::is_floating_point_v<T>;

template <typename T>
concept scalar = std::is_scalar_v<T>;

constexpr double e       = std::numbers::e;
constexpr float  epsilon = 0.0001;
constexpr double phi     = std::numbers::phi;
constexpr double pi      = std::numbers::pi;
constexpr double sqrt2   = std::numbers::sqrt2;
constexpr double sqrt3   = std::numbers::sqrt3;

constexpr double pi_inv    = 1 / pi;
constexpr double sqrt2_inv = 1 / sqrt2;
constexpr double sqrt3_inv = 1 / sqrt3;

// Basic

template<scalar X>
inline X sqrt(X x);

template<scalar X, scalar Power,
		scalar Ret = std::common_type_t<X, Power>>
inline Ret pow(X x, Power power);

template<scalar Base, scalar X,
		scalar Ret = std::common_type_t<Base, X>>
inline Ret log(Base base, X x);

template<scalar X>
inline X log(X x);

template<scalar X>
inline X log2(X x);

template<scalar Base, scalar X,
		scalar Ret = std::common_type_t<Base, X>>
inline Ret exp(Base base, X x);

template<scalar X>
inline X exp(X x);

template<scalar X>
inline X exp2(X x);

template<scalar X>
inline X abs(X x);

template<scalar A, scalar B,
		scalar Epsilon = decltype(epsilon)>
bool is_approx(A a, B b, Epsilon epsilon = epsilon);

// Trigonometry

template<scalar Angle>
inline Angle sin(Angle angle);

template<scalar Angle>
inline Angle cos(Angle angle);

template<scalar Angle>
inline Angle tan(Angle angle);

template<scalar Y>
inline Y asin(Y y);

template<scalar X>
inline X acos(X x);

template<scalar YOverX>
inline YOverX atan(YOverX y_over_x);

template<scalar Y, scalar X,
		scalar Ret = std::common_type_t<Y, X>>
inline Ret atan2(Y y, X x);

// Hyperbolic

template<scalar DoubleArea>
inline DoubleArea sinh(DoubleArea double_area);

template<scalar DoubleArea>
inline DoubleArea cosh(DoubleArea double_area);

template<scalar DoubleArea>
inline DoubleArea tanh(DoubleArea double_area);

template<scalar Y>
inline Y asinh(Y y);

template<scalar X>
inline X acosh(X x);

template<scalar YOverX>
inline YOverX atanh(YOverX y_over_x);

// Rounding

template<floating_point X>
inline X floor(X x);

template<floating_point X>
inline X ceil(X x);

template<floating_point X>
inline X round(X x);

// Conversion

template<scalar Degrees>
inline Degrees radians(Degrees degrees);

template<scalar Radians>
inline Radians degrees(Radians radians);

// Miscellaneous

template<scalar X, scalar Min, scalar Max,
		scalar Ret = std::common_type_t<X, Min, Max>>
inline Ret clamp(X x, Min min, Max max);

template<floating_point X>
inline X fract(X x);

template<scalar A, scalar B, scalar Weight,
		scalar Ret = std::common_type_t<A, B, Weight>>
inline Ret lerp(A a, B b, Weight weight);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
inline Ret max(A a, B b);

template<scalar A, scalar B, scalar... Others,
		scalar Ret = std::common_type_t<A, B, Others...>>
inline Ret max(A a, B b, Others ...others);

template<scalar A, scalar B,
		scalar Ret = std::common_type_t<A, B>>
inline Ret min(A a, B b);

template<scalar A, scalar B, scalar... Others,
		scalar Ret = std::common_type_t<A, B, Others...>>
inline Ret min(A a, B b, Others ...others);

template<scalar X, scalar Y,
		scalar Ret = std::common_type_t<X, Y>>
inline Ret mod(X x, Y y);

template<scalar X>
inline X saturate(X x);

template<scalar X>
inline int32_t sign(X x);

template<scalar From, scalar To, scalar X,
		scalar Ret = std::common_type_t<From, To, X>>
inline Ret smoothstep(From from, To to, X x);

template<scalar Edge, scalar X>
inline int32_t step(Edge edge, X x);

}

#include "math.inl"

#include "mat3.hpp"
#include "quat.hpp"
#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
