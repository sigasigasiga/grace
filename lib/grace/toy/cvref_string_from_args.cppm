export module grace.toy:cvref_string_from_args;

import std;

export namespace grace::toy {

constexpr std::string_view cvref_string_from_args(auto &) noexcept { return "auto &"; }
constexpr std::string_view cvref_string_from_args(auto const &) noexcept { return "const auto &"; }
constexpr std::string_view cvref_string_from_args(auto volatile &) noexcept { return "volatile auto &"; }
constexpr std::string_view cvref_string_from_args(auto const volatile &) noexcept { return "const volatile auto &"; }
constexpr std::string_view cvref_string_from_args(auto &&) noexcept { return "auto &&"; }
constexpr std::string_view cvref_string_from_args(auto const &&) noexcept { return "const auto &&"; }
constexpr std::string_view cvref_string_from_args(auto volatile &&) noexcept { return "volatile auto &&"; }
constexpr std::string_view cvref_string_from_args(auto const volatile &&) noexcept { return "const volatile auto &&"; }

} // namespace grace::toy
