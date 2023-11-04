#pragma once

#include <ratio>

using step_ratio = std::ratio<1024, 1>;

using kibi_t = step_ratio;
using mebi_t = std::ratio_multiply<kibi_t, step_ratio>;
using gibi_t = std::ratio_multiply<mebi_t, step_ratio>;
using tebi_t = std::ratio_multiply<gibi_t, step_ratio>;
using pebi_t = std::ratio_multiply<tebi_t, step_ratio>;
using exbi_t = std::ratio_multiply<pebi_t, step_ratio>;

constexpr kibi_t kibi{};
constexpr mebi_t mebi{};
constexpr gibi_t gibi{};
constexpr tebi_t tebi{};
constexpr pebi_t pebi{};
constexpr exbi_t exbi{};

template <typename A, std::intmax_t N, std::intmax_t D>
    requires std::is_arithmetic_v<A>
constexpr A operator*(A val, std::ratio<N, D>) {
	return val * A{N} / A{D};
}

template <typename A, std::intmax_t N, std::intmax_t D>
    requires std::is_arithmetic_v<A>
constexpr A operator/(A val, std::ratio<N, D>) {
	return val * A{D} / A{N};
}
