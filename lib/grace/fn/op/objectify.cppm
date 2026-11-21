export module grace.fn.op:objectify;

import std;

import grace.utility;

export namespace grace::fn::op {

class [[nodiscard]] objectify
{
public:
    template<typename T>
    [[nodiscard]] static constexpr auto operator()(T &&value)
        noexcept(noexcept(utility::objectify(std::forward<T>(value))))
        -> decltype(utility::objectify(std::forward<T>(value)))
    {
        return utility::objectify(std::forward<T>(value));
    }
};

} // namespace grace::fn::op
