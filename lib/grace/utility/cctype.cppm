export module grace.utility:cctype;

import std;

export namespace grace::utility {

[[nodiscard]] inline bool is_alnum(unsigned char c) noexcept { return std::isalnum(c); }
[[nodiscard]] inline bool is_alpha(unsigned char c) noexcept { return std::isalpha(c); }
[[nodiscard]] inline bool is_blank(unsigned char c) noexcept { return std::isblank(c); }
[[nodiscard]] inline bool is_cntrl(unsigned char c) noexcept { return std::iscntrl(c); }
[[nodiscard]] inline bool is_digit(unsigned char c) noexcept { return std::isdigit(c); }
[[nodiscard]] inline bool is_graph(unsigned char c) noexcept { return std::isgraph(c); }
[[nodiscard]] inline bool is_lower(unsigned char c) noexcept { return std::islower(c); }
[[nodiscard]] inline bool is_print(unsigned char c) noexcept { return std::isprint(c); }
[[nodiscard]] inline bool is_punct(unsigned char c) noexcept { return std::ispunct(c); }
[[nodiscard]] inline bool is_space(unsigned char c) noexcept { return std::isspace(c); }
[[nodiscard]] inline bool is_upper(unsigned char c) noexcept { return std::isupper(c); }
[[nodiscard]] inline bool is_xdigit(unsigned char c) noexcept { return std::isxdigit(c); }

[[nodiscard]] inline char to_lower(unsigned char c) noexcept { return std::tolower(c); }
[[nodiscard]] inline char to_upper(unsigned char c) noexcept { return std::toupper(c); }

} // namespace grace::utility
