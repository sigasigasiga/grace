export module grace.bit:make_bitset;

import std;

export namespace grace::bit {

// the last one in the pack is the first one in the bitset, as in `bitset`'s constructor
// you may use `fn::invoke::reverse_arguments` to fix that
template<typename... Args, std::size_t Sz = sizeof...(Args)>
[[nodiscard]] constexpr auto make_bitset(Args &&...args)
    noexcept(noexcept(std::bitset<Sz>{std::array{(std::forward<Args>(args) ? '1' : '0')..., '\0'}.data()}))
    -> decltype(std::bitset<Sz>{std::array{(std::forward<Args>(args) ? '1' : '0')..., '\0'}.data()})
{
    return std::bitset<Sz>{std::array{(std::forward<Args>(args) ? '1' : '0')..., '\0'}.data()};
}

} // namespace grace::bit
