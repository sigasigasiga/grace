export module grace.fn.op:decay_copy;

import std;

import grace.utility;

export namespace grace::fn::op {

// unlike `copy_t`, the return value may be move-constructed
// https://en.cppreference.com/w/cpp/standard_library/decay-copy
class [[nodiscard]] decay_copy
{
public:
    template<typename T>
    [[nodiscard]] static constexpr auto operator()(T &&value)
        noexcept(noexcept(utility::decay_copy(std::forward<T>(value))))
        -> decltype(utility::decay_copy(std::forward<T>(value)))
    {
        return utility::decay_copy(std::forward<T>(value));
    }
};

} // namespace grace::fn::op
