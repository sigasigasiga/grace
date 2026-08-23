module;

#include <algorithm>
#include <concepts>

export module grace.algorithm:clamp_to;

export namespace grace::algorithm {

template<std::integral To, std::integral From>
requires (sizeof(To) <= sizeof(From)) &&
         (std::is_signed_v<To> == std::is_signed_v<From>)
[[nodiscard]] constexpr To clamp_to(From value) {
    return static_cast<To>(std::clamp(
        value,
        static_cast<From>(std::numeric_limits<To>::min()),
        static_cast<From>(std::numeric_limits<To>::max())
    ));
}

} // namespace grace::algorithm
