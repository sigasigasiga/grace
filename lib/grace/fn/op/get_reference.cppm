export module grace.fn.op:get_reference;

import std;

import grace.utility;

export namespace grace::fn::op {

class [[nodiscard]] get_reference
{
public:
    template<typename T>
    [[nodiscard]] static constexpr auto operator()(T &&value)
        noexcept(noexcept(utility::get_reference(std::forward<T>(value))))
        -> decltype(utility::get_reference(std::forward<T>(value)))
    {
        return utility::get_reference(std::forward<T>(value));
    }
};

} // namespace grace::fn::op
